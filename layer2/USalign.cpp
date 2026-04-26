/**
 * TM-align algorithm implementation for PyMOL.
 *
 * Ported from USalign (Zhang lab) to modern C++17 with glm math types.
 * Original algorithm: Zhang & Skolnick, Nucl. Acids Res. 33, 2302 (2005)
 *
 * Strips all file I/O — coordinates come from PyMOL selections.
 * Replaces double** with std::vector<glm::dvec3>, NewArray/DeleteArray with
 * RAII containers, scattered output params with result structs.
 */

/*
==============================================================================
   US-align: universal structure alignment of monomeric and complex proteins
   and nucleic acids

   This program was written by Chengxin Zhang at Yang Zhang lab,
   Department of Computational Medicine and Bioinformatics,
   University of Michigan, 100 Washtenaw Ave, Ann Arbor, MI 48109-2218.
   Please report issues to zhanglab@zhanggroup.org

   References:
   * C Zhang, M Shine, A Pyle, Y Zhang (2022) Nat Methods. 19(9), 1109-1115.
   * C Zhang, L Freddolino, Y Zhang (2025) Nature Protocols,
https://doi.org/10.1038/s41596-025-01189-x

   DISCLAIMER:
     Permission to use, copy, modify, and distribute this program for
     any purpose, with or without fee, is hereby granted, provided that
     the notices on the head, the reference information, and this
     copyright notice appear in all copies or substantial portions of
     the Software. It is provided "as is" without express or implied
     warranty.
===============================================================================
*/

#include "USalign.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

#include <glm/glm.hpp>

namespace pymol::usalign
{

// ========================================================================
// DPWorkspace
// ========================================================================

void DPWorkspace::resize(int xlen, int ylen)
{
  rows = xlen + 1;
  cols = ylen + 1;
  score_flat.assign(static_cast<size_t>(rows) * cols, 0.0);
  val_flat.assign(static_cast<size_t>(rows) * cols, 0.0);
  path_flat.assign(static_cast<size_t>(rows) * cols, false);

  int minlen = std::min(xlen, ylen);
  xtm.resize(minlen);
  ytm.resize(minlen);
  xt.resize(xlen);
  r1.resize(minlen);
  r2.resize(minlen);
}

// ========================================================================
// Parameter initialization (from param_set.h)
// ========================================================================

struct TMParameters {
  double d0 = 0.0;
  double d0_search = 0.0;
  double Lnorm = 0.0;
  double score_d8 = 0.0;
  double D0_MIN = 0.0;
  double dcu0 = 0.0;
};

static TMParameters compute_search_params(int xlen, int ylen)
{
  TMParameters p;
  p.D0_MIN = 0.5;
  p.dcu0 = 4.25;

  p.Lnorm = std::min(xlen, ylen);
  if (p.Lnorm <= 19)
    p.d0 = 0.168;
  else
    p.d0 = 1.24 * std::pow(p.Lnorm - 15.0, 1.0 / 3.0) - 1.8;
  // During the search phase, d0 is inflated by 0.8 compared to final scoring.
  // This wider distance cutoff helps the iterative refinement explore more seeds.
  p.D0_MIN = p.d0 + 0.8;
  p.d0 = p.D0_MIN;

  p.d0_search = p.d0;
  if (p.d0_search > 8.0)
    p.d0_search = 8.0;
  if (p.d0_search < 4.5)
    p.d0_search = 4.5;

  p.score_d8 = 1.5 * std::pow(p.Lnorm, 0.3) + 3.5;
  return p;
}

static void compute_final_params(
    double len, double& D0_MIN, double& Lnorm, double& d0, double& d0_search)
{
  D0_MIN = 0.5;
  Lnorm = len;
  if (Lnorm <= 21)
    d0 = 0.5;
  else
    d0 = 1.24 * std::pow(Lnorm - 15.0, 1.0 / 3.0) - 1.8;
  if (d0 < D0_MIN)
    d0 = D0_MIN;
  d0_search = d0;
  if (d0_search > 8.0)
    d0_search = 8.0;
  if (d0_search < 4.5)
    d0_search = 4.5;
}

// ========================================================================
// Geometry utilities
// ========================================================================

static inline double dist2(const glm::dvec3& a, const glm::dvec3& b)
{
  auto d = a - b;
  return glm::dot(d, d);
}

static inline glm::dvec3 apply_transform(
    const glm::dvec3& v, const Superposition& sup)
{
  return sup.rotation * v + sup.translation;
}

static void do_rotation(const std::vector<glm::dvec3>& src,
    std::vector<glm::dvec3>& dst, int n, const Superposition& sup)
{
  for (int i = 0; i < n; i++)
    dst[i] = apply_transform(src[i], sup);
}

// ========================================================================
// Kabsch superposition (from Kabsch.h)
// ========================================================================

static bool kabsch(const std::vector<glm::dvec3>& x,
    const std::vector<glm::dvec3>& y, int n, int mode, double& rms,
    Superposition& sup)
{
  double e0, rms1, d, h, g;
  double cth, sth, sqrth, p, det, sigma;
  double xc[3], yc[3];
  double a[3][3], b[3][3], r[3][3], e[3], rr[6], ss[6];
  const double sqrt3 = 1.73205080756888;
  const double tol = 0.01;
  const int ip[] = {0, 1, 3, 1, 2, 4, 3, 4, 5};
  const int ip2312[] = {1, 2, 0, 1};

  int a_failed = 0, b_failed = 0;
  const double epsilon = 0.00000001;

  rms = 0;
  rms1 = 0;
  e0 = 0;

  double t[3];
  double u[3][3];

  double s1[3] = {0, 0, 0};
  double s2[3] = {0, 0, 0};
  double sx[3] = {0, 0, 0};
  double sy[3] = {0, 0, 0};
  double sz[3] = {0, 0, 0};

  for (int i = 0; i < 3; i++) {
    xc[i] = 0.0;
    yc[i] = 0.0;
    t[i] = 0.0;
    for (int j = 0; j < 3; j++) {
      u[i][j] = (i == j) ? 1.0 : 0.0;
      r[i][j] = 0.0;
      a[i][j] = (i == j) ? 1.0 : 0.0;
    }
  }

  if (n < 1)
    return false;

  // compute centers
  for (int i = 0; i < n; i++) {
    double c1[3] = {x[i].x, x[i].y, x[i].z};
    double c2[3] = {y[i].x, y[i].y, y[i].z};
    for (int j = 0; j < 3; j++) {
      s1[j] += c1[j];
      s2[j] += c2[j];
    }
    for (int j = 0; j < 3; j++) {
      sx[j] += c1[0] * c2[j];
      sy[j] += c1[1] * c2[j];
      sz[j] += c1[2] * c2[j];
    }
  }
  for (int i = 0; i < 3; i++) {
    xc[i] = s1[i] / n;
    yc[i] = s2[i] / n;
  }

  if (mode == 2 || mode == 0) {
    for (int mm = 0; mm < n; mm++) {
      double xx[3] = {x[mm].x, x[mm].y, x[mm].z};
      double yy[3] = {y[mm].x, y[mm].y, y[mm].z};
      for (int nn = 0; nn < 3; nn++)
        e0 += (xx[nn] - xc[nn]) * (xx[nn] - xc[nn]) +
              (yy[nn] - yc[nn]) * (yy[nn] - yc[nn]);
    }
  }

  for (int j = 0; j < 3; j++) {
    r[j][0] = sx[j] - s1[0] * s2[j] / n;
    r[j][1] = sy[j] - s1[1] * s2[j] / n;
    r[j][2] = sz[j] - s1[2] * s2[j] / n;
  }

  // compute determinant
  det = r[0][0] * (r[1][1] * r[2][2] - r[1][2] * r[2][1]) -
        r[0][1] * (r[1][0] * r[2][2] - r[1][2] * r[2][0]) +
        r[0][2] * (r[1][0] * r[2][1] - r[1][1] * r[2][0]);
  sigma = det;

  // compute trans(r)*r
  int m = 0;
  for (int j = 0; j < 3; j++)
    for (int i = 0; i <= j; i++)
      rr[m++] = r[0][i] * r[0][j] + r[1][i] * r[1][j] + r[2][i] * r[2][j];

  double spur = (rr[0] + rr[2] + rr[5]) / 3.0;
  double cof =
      (((((rr[2] * rr[5] - rr[4] * rr[4]) + rr[0] * rr[5]) - rr[3] * rr[3]) +
           rr[0] * rr[2]) -
          rr[1] * rr[1]) /
      3.0;
  det = det * det;

  for (int i = 0; i < 3; i++)
    e[i] = spur;

  if (spur > 0) {
    d = spur * spur;
    h = d - cof;
    g = (spur * cof - det) / 2.0 - spur * h;

    if (h > 0) {
      sqrth = std::sqrt(h);
      d = h * h * h - g * g;
      if (d < 0.0)
        d = 0.0;
      d = std::atan2(std::sqrt(d), -g) / 3.0;
      cth = sqrth * std::cos(d);
      sth = sqrth * sqrt3 * std::sin(d);
      e[0] = (spur + cth) + cth;
      e[1] = (spur - cth) + sth;
      e[2] = (spur - cth) - sth;

      if (mode != 0) {
        for (int l = 0; l < 3; l += 2) {
          d = e[l];
          ss[0] = (d - rr[2]) * (d - rr[5]) - rr[4] * rr[4];
          ss[1] = (d - rr[5]) * rr[1] + rr[3] * rr[4];
          ss[2] = (d - rr[0]) * (d - rr[5]) - rr[3] * rr[3];
          ss[3] = (d - rr[2]) * rr[3] + rr[1] * rr[4];
          ss[4] = (d - rr[0]) * rr[4] + rr[1] * rr[3];
          ss[5] = (d - rr[0]) * (d - rr[2]) - rr[1] * rr[1];

          for (int i = 0; i < 6; i++)
            if (std::fabs(ss[i]) <= epsilon)
              ss[i] = 0.0;

          int j;
          if (std::fabs(ss[0]) >= std::fabs(ss[2])) {
            j = 0;
            if (std::fabs(ss[0]) < std::fabs(ss[5]))
              j = 2;
          } else if (std::fabs(ss[2]) >= std::fabs(ss[5]))
            j = 1;
          else
            j = 2;

          d = 0.0;
          j = 3 * j;
          for (int i = 0; i < 3; i++) {
            int k = ip[i + j];
            a[i][l] = ss[k];
            d += ss[k] * ss[k];
          }

          if (d > epsilon)
            d = 1.0 / std::sqrt(d);
          else
            d = 0.0;
          for (int i = 0; i < 3; i++)
            a[i][l] *= d;
        }

        d = a[0][0] * a[0][2] + a[1][0] * a[1][2] + a[2][0] * a[2][2];
        int m1, m0;
        if ((e[0] - e[1]) > (e[1] - e[2])) {
          m1 = 2;
          m0 = 0;
        } else {
          m1 = 0;
          m0 = 2;
        }
        p = 0;
        for (int i = 0; i < 3; i++) {
          a[i][m1] -= d * a[i][m0];
          p += a[i][m1] * a[i][m1];
        }
        if (p <= tol) {
          p = 1.0;
          int j = 0;
          for (int i = 0; i < 3; i++) {
            if (p < std::fabs(a[i][m0]))
              continue;
            p = std::fabs(a[i][m0]);
            j = i;
          }
          int k = ip2312[j];
          int l = ip2312[j + 1];
          p = std::sqrt(a[k][m0] * a[k][m0] + a[l][m0] * a[l][m0]);
          if (p > tol) {
            a[j][m1] = 0.0;
            a[k][m1] = -a[l][m0] / p;
            a[l][m1] = a[k][m0] / p;
          } else
            a_failed = 1;
        } else {
          p = 1.0 / std::sqrt(p);
          for (int i = 0; i < 3; i++)
            a[i][m1] *= p;
        }
        if (a_failed != 1) {
          a[0][1] = a[1][2] * a[2][0] - a[1][0] * a[2][2];
          a[1][1] = a[2][2] * a[0][0] - a[2][0] * a[0][2];
          a[2][1] = a[0][2] * a[1][0] - a[0][0] * a[1][2];
        }
      }
    }

    if (mode != 0 && a_failed != 1) {
      for (int l = 0; l < 2; l++) {
        d = 0.0;
        for (int i = 0; i < 3; i++) {
          b[i][l] = r[i][0] * a[0][l] + r[i][1] * a[1][l] + r[i][2] * a[2][l];
          d += b[i][l] * b[i][l];
        }
        if (d > epsilon)
          d = 1.0 / std::sqrt(d);
        else
          d = 0.0;
        for (int i = 0; i < 3; i++)
          b[i][l] *= d;
      }
      d = b[0][0] * b[0][1] + b[1][0] * b[1][1] + b[2][0] * b[2][1];
      p = 0.0;
      for (int i = 0; i < 3; i++) {
        b[i][1] -= d * b[i][0];
        p += b[i][1] * b[i][1];
      }
      if (p <= tol) {
        p = 1.0;
        int j = 0;
        for (int i = 0; i < 3; i++) {
          if (p < std::fabs(b[i][0]))
            continue;
          p = std::fabs(b[i][0]);
          j = i;
        }
        int k = ip2312[j];
        int l = ip2312[j + 1];
        p = std::sqrt(b[k][0] * b[k][0] + b[l][0] * b[l][0]);
        if (p > tol) {
          b[j][1] = 0.0;
          b[k][1] = -b[l][0] / p;
          b[l][1] = b[k][0] / p;
        } else
          b_failed = 1;
      } else {
        p = 1.0 / std::sqrt(p);
        for (int i = 0; i < 3; i++)
          b[i][1] *= p;
      }
      if (b_failed != 1) {
        b[0][2] = b[1][0] * b[2][1] - b[1][1] * b[2][0];
        b[1][2] = b[2][0] * b[0][1] - b[2][1] * b[0][0];
        b[2][2] = b[0][0] * b[1][1] - b[0][1] * b[1][0];
        for (int i = 0; i < 3; i++)
          for (int j = 0; j < 3; j++)
            u[i][j] = b[i][0] * a[j][0] + b[i][1] * a[j][1] + b[i][2] * a[j][2];
      }
      for (int i = 0; i < 3; i++)
        t[i] = ((yc[i] - u[i][0] * xc[0]) - u[i][1] * xc[1]) - u[i][2] * xc[2];
    }
  } else {
    for (int i = 0; i < 3; i++)
      t[i] = ((yc[i] - u[i][0] * xc[0]) - u[i][1] * xc[1]) - u[i][2] * xc[2];
  }

  // compute rms
  for (int i = 0; i < 3; i++) {
    if (e[i] < 0)
      e[i] = 0;
    e[i] = std::sqrt(e[i]);
  }
  d = e[2];
  if (sigma < 0.0)
    d = -d;
  d = (d + e[1]) + e[0];

  if (mode == 2 || mode == 0) {
    rms1 = (e0 - d) - d;
    if (rms1 < 0.0)
      rms1 = 0.0;
  }

  rms = rms1;

  // convert to glm types
  sup.translation = glm::dvec3(t[0], t[1], t[2]);
  sup.rotation = glm::dmat3(u[0][0], u[1][0], u[2][0], u[0][1], u[1][1],
      u[2][1], u[0][2], u[1][2], u[2][2]);

  return true;
}

// ========================================================================
// Secondary structure assignment (from se.h:make_sec)
// ========================================================================

static char sec_str_classify(double dis13, double dis14, double dis15,
    double dis24, double dis25, double dis35)
{
  double delta = 2.1;
  if (std::fabs(dis15 - 6.37) < delta && std::fabs(dis14 - 5.18) < delta &&
      std::fabs(dis25 - 5.18) < delta && std::fabs(dis13 - 5.45) < delta &&
      std::fabs(dis24 - 5.45) < delta && std::fabs(dis35 - 5.45) < delta)
    return 'H';

  delta = 1.42;
  if (std::fabs(dis15 - 13.0) < delta && std::fabs(dis14 - 10.4) < delta &&
      std::fabs(dis25 - 10.4) < delta && std::fabs(dis13 - 6.1) < delta &&
      std::fabs(dis24 - 6.1) < delta && std::fabs(dis35 - 6.1) < delta)
    return 'E';

  if (dis15 < 8)
    return 'T';
  return 'C';
}

static void assign_secondary_structure(
    const std::vector<glm::dvec3>& ca, std::vector<char>& sec)
{
  int len = static_cast<int>(ca.size());
  sec.assign(len, 'C');

  for (int i = 0; i < len; i++) {
    int j1 = i - 2, j2 = i - 1, j3 = i, j4 = i + 1, j5 = i + 2;
    if (j1 >= 0 && j5 < len) {
      double d13 = std::sqrt(dist2(ca[j1], ca[j3]));
      double d14 = std::sqrt(dist2(ca[j1], ca[j4]));
      double d15 = std::sqrt(dist2(ca[j1], ca[j5]));
      double d24 = std::sqrt(dist2(ca[j2], ca[j4]));
      double d25 = std::sqrt(dist2(ca[j2], ca[j5]));
      double d35 = std::sqrt(dist2(ca[j3], ca[j5]));
      sec[i] = sec_str_classify(d13, d14, d15, d24, d25, d35);
    }
  }
}

// ========================================================================
// Scoring functions (from TMalign.h:score_fun8)
// ========================================================================

// Lnorm > 0: normalize by Lnorm (used during main search)
// Lnorm <= 0: normalize by n_ali (used during standard/final scoring)
static int score_fun8(const std::vector<glm::dvec3>& xa,
    const std::vector<glm::dvec3>& ya, int n_ali, double d,
    std::vector<int>& i_ali, double& score1, int score_sum_method,
    double score_d8, double d0, double Lnorm = 0.0)
{
  double score_sum = 0;
  double d_tmp = d * d;
  double d02 = d0 * d0;
  double score_d8_cut = score_d8 * score_d8;

  int n_cut, inc = 0;
  while (true) {
    n_cut = 0;
    score_sum = 0;
    for (int i = 0; i < n_ali; i++) {
      double di = dist2(xa[i], ya[i]);
      if (di < d_tmp) {
        i_ali[n_cut] = i;
        n_cut++;
      }
      if (score_sum_method == 8) {
        if (di <= score_d8_cut)
          score_sum += 1.0 / (1.0 + di / d02);
      } else
        score_sum += 1.0 / (1.0 + di / d02);
    }
    if (n_cut < 3 && n_ali > 3) {
      inc++;
      double dinc = d + inc * 0.5;
      d_tmp = dinc * dinc;
    } else
      break;
  }
  score1 = score_sum / (Lnorm > 0.0 ? Lnorm : n_ali);
  return n_cut;
}

// ========================================================================
// TMscore8_search — iterative fragment refinement (from TMalign.h)
// ========================================================================

// Lnorm > 0: normalize by Lnorm (used during main search)
// Lnorm <= 0: normalize by n_ali (used during standard/final scoring)
static double TMscore8_search(DPWorkspace& ws, int Lali, Superposition& sup_out,
    int simplify_step, int score_sum_method, double local_d0_search,
    double score_d8, double d0, double Lnorm = 0.0)
{
  double score_max = -1, score, rmsd;
  const int n_it = 20;
  const int n_init_max = 6;
  int L_ini[6];
  int L_ini_min = 4;
  if (Lali < L_ini_min)
    L_ini_min = Lali;

  int n_init = 0;
  int i;
  for (i = 0; i < n_init_max - 1; i++) {
    n_init++;
    L_ini[i] = static_cast<int>(Lali / std::pow(2.0, static_cast<double>(i)));
    if (L_ini[i] <= L_ini_min) {
      L_ini[i] = L_ini_min;
      break;
    }
  }
  if (i == n_init_max - 1) {
    n_init++;
    L_ini[i] = L_ini_min;
  }

  std::vector<int> i_ali(Lali);
  std::vector<int> k_ali(Lali);
  Superposition sup_tmp;

  for (int i_init = 0; i_init < n_init; i_init++) {
    int L_frag = L_ini[i_init];
    int iL_max = Lali - L_frag;

    i = 0;
    while (true) {
      for (int k = 0; k < L_frag; k++) {
        int kk = k + i;
        ws.r1[k] = ws.xtm[kk];
        ws.r2[k] = ws.ytm[kk];
        k_ali[k] = kk;
      }
      kabsch(ws.r1, ws.r2, L_frag, 1, rmsd, sup_tmp);
      do_rotation(ws.xtm, ws.xt, Lali, sup_tmp);

      double d_cut = local_d0_search - 1;
      int n_cut = score_fun8(ws.xt, ws.ytm, Lali, d_cut, i_ali, score,
          score_sum_method, score_d8, d0, Lnorm);
      if (score > score_max) {
        score_max = score;
        sup_out = sup_tmp;
      }

      // iterative refinement
      d_cut = local_d0_search + 1;
      for (int it = 0; it < n_it; it++) {
        int ka = n_cut;
        for (int k = 0; k < n_cut; k++) {
          int m = i_ali[k];
          ws.r1[k] = ws.xtm[m];
          ws.r2[k] = ws.ytm[m];
          k_ali[k] = m;
        }

        kabsch(ws.r1, ws.r2, n_cut, 1, rmsd, sup_tmp);
        do_rotation(ws.xtm, ws.xt, Lali, sup_tmp);
        n_cut = score_fun8(ws.xt, ws.ytm, Lali, d_cut, i_ali, score,
            score_sum_method, score_d8, d0, Lnorm);
        if (score > score_max) {
          score_max = score;
          sup_out = sup_tmp;
        }

        if (n_cut == ka) {
          int k;
          for (k = 0; k < n_cut; k++)
            if (i_ali[k] != k_ali[k])
              break;
          if (k == n_cut)
            break;
        }
      }

      if (i < iL_max) {
        i += simplify_step;
        if (i > iL_max)
          i = iL_max;
      } else
        break;
    }
  }
  return score_max;
}

// ========================================================================
// Detailed search (from TMalign.h:detailed_search)
// ========================================================================

// Lnorm > 0: normalize by Lnorm (used during main search)
// Lnorm <= 0: normalize by n_ali (used during standard/final scoring)
static double detailed_search(DPWorkspace& ws, const std::vector<glm::dvec3>& x,
    const std::vector<glm::dvec3>& y, int xlen, int ylen,
    const std::vector<int>& invmap, Superposition& sup, int simplify_step,
    int score_sum_method, double local_d0_search, double score_d8, double d0,
    double Lnorm = 0.0)
{
  int k = 0;
  for (int i = 0; i < ylen; i++) {
    int j = invmap[i];
    if (j >= 0) {
      ws.xtm[k] = x[j];
      ws.ytm[k] = y[i];
      k++;
    }
  }
  return TMscore8_search(
      ws, k, sup, simplify_step, score_sum_method, local_d0_search,
      score_d8, d0, Lnorm);
}

// ========================================================================
// Get score fast — quick 3-pass evaluation (from TMalign.h:get_score_fast)
// ========================================================================

static double get_score_fast(DPWorkspace& ws, const std::vector<glm::dvec3>& x,
    const std::vector<glm::dvec3>& y, int xlen, int ylen,
    const std::vector<int>& invmap, double d0, double d0_search,
    Superposition& sup)
{
  double rms;
  int k = 0;
  for (int j = 0; j < ylen; j++) {
    int i = invmap[j];
    if (i >= 0) {
      ws.r1[k] = x[i];
      ws.r2[k] = y[j];
      ws.xtm[k] = x[i];
      ws.ytm[k] = y[j];
      k++;
    }
  }
  kabsch(ws.r1, ws.r2, k, 1, rms, sup);

  double d02 = d0 * d0;
  double d00 = d0_search;
  double d002 = d00 * d00;
  int n_ali = k;

  std::vector<double> dis(n_ali);
  double tmscore = 0;
  for (int kk = 0; kk < n_ali; kk++) {
    auto xrot = apply_transform(ws.xtm[kk], sup);
    double di = dist2(xrot, ws.ytm[kk]);
    dis[kk] = di;
    tmscore += 1.0 / (1.0 + di / d02);
  }

  // second iteration
  double d002t = d002;
  {
    std::vector<double> dis_sorted(dis.begin(), dis.end());
    std::sort(dis_sorted.begin(), dis_sorted.end());
    if (n_ali > 2 && d002t < dis_sorted[2])
      d002t = dis_sorted[2];
  }
  while (true) {
    int j = 0;
    for (int kk = 0; kk < n_ali; kk++) {
      if (dis[kk] <= d002t) {
        ws.r1[j] = ws.xtm[kk];
        ws.r2[j] = ws.ytm[kk];
        j++;
      }
    }
    if (j < 3 && n_ali > 3)
      d002t += 0.5;
    else
      break;
  }

  double tmscore1, tmscore2;
  int j_count;
  {
    // count how many passed
    j_count = 0;
    for (int kk = 0; kk < n_ali; kk++)
      if (dis[kk] <= d002t)
        j_count++;
  }

  if (n_ali != j_count) {
    Superposition sup2;
    kabsch(ws.r1, ws.r2, j_count, 1, rms, sup2);
    tmscore1 = 0;
    for (int kk = 0; kk < n_ali; kk++) {
      auto xrot = apply_transform(ws.xtm[kk], sup2);
      double di = dist2(xrot, ws.ytm[kk]);
      dis[kk] = di;
      tmscore1 += 1.0 / (1.0 + di / d02);
    }

    // third iteration
    d002t = d002 + 1;
    {
      std::vector<double> dis_sorted(dis.begin(), dis.end());
      std::sort(dis_sorted.begin(), dis_sorted.end());
      if (n_ali > 2 && d002t < dis_sorted[2])
        d002t = dis_sorted[2];
    }
    while (true) {
      int j = 0;
      for (int kk = 0; kk < n_ali; kk++) {
        if (dis[kk] <= d002t) {
          ws.r1[j] = ws.xtm[kk];
          ws.r2[j] = ws.ytm[kk];
          j++;
        }
      }
      if (j < 3 && n_ali > 3)
        d002t += 0.5;
      else {
        j_count = j;
        break;
      }
    }

    Superposition sup3;
    kabsch(ws.r1, ws.r2, j_count, 1, rms, sup3);
    tmscore2 = 0;
    for (int kk = 0; kk < n_ali; kk++) {
      auto xrot = apply_transform(ws.xtm[kk], sup3);
      double di = dist2(xrot, ws.ytm[kk]);
      tmscore2 += 1.0 / (1.0 + di / d02);
    }
  } else {
    tmscore1 = tmscore;
    tmscore2 = tmscore;
  }

  if (tmscore1 >= tmscore)
    tmscore = tmscore1;
  if (tmscore2 >= tmscore)
    tmscore = tmscore2;
  return tmscore;
}

// ========================================================================
// Needleman-Wunsch DP variants (from NW.h)
// ========================================================================

// NW with precomputed score matrix
static void nw_align_score(
    DPWorkspace& ws, int len1, int len2, double gap_open, std::vector<int>& j2i)
{
  for (int i = 0; i <= len1; i++) {
    ws.val(i, 0) = 0;
    ws.set_path(i, 0, false);
  }
  for (int j = 0; j <= len2; j++) {
    ws.val(0, j) = 0;
    ws.set_path(0, j, false);
  }
  for (int j = 0; j < len2; j++)
    j2i[j] = -1;

  for (int i = 1; i <= len1; i++) {
    for (int j = 1; j <= len2; j++) {
      double d = ws.val(i - 1, j - 1) + ws.score(i, j);
      double h = ws.val(i - 1, j);
      if (ws.path(i - 1, j))
        h += gap_open;
      double v = ws.val(i, j - 1);
      if (ws.path(i, j - 1))
        v += gap_open;

      if (d >= h && d >= v) {
        ws.set_path(i, j, true);
        ws.val(i, j) = d;
      } else {
        ws.set_path(i, j, false);
        ws.val(i, j) = (v >= h) ? v : h;
      }
    }
  }

  // traceback
  int i = len1, j = len2;
  while (i > 0 && j > 0) {
    if (ws.path(i, j)) {
      j2i[j - 1] = i - 1;
      i--;
      j--;
    } else {
      double h = ws.val(i - 1, j);
      if (ws.path(i - 1, j))
        h += gap_open;
      double v = ws.val(i, j - 1);
      if (ws.path(i, j - 1))
        v += gap_open;
      if (v >= h)
        j--;
      else
        i--;
    }
  }
}

// NW with geometric scoring using rotation
static void nw_align_transform(DPWorkspace& ws,
    const std::vector<glm::dvec3>& x, const std::vector<glm::dvec3>& y,
    int len1, int len2, const Superposition& sup, double d02, double gap_open,
    std::vector<int>& j2i)
{
  for (int i = 0; i <= len1; i++) {
    ws.val(i, 0) = 0;
    ws.set_path(i, 0, false);
  }
  for (int j = 0; j <= len2; j++) {
    ws.val(0, j) = 0;
    ws.set_path(0, j, false);
  }
  for (int j = 0; j < len2; j++)
    j2i[j] = -1;

  for (int i = 1; i <= len1; i++) {
    auto xx = apply_transform(x[i - 1], sup);
    for (int j = 1; j <= len2; j++) {
      double dij = dist2(xx, y[j - 1]);
      double d = ws.val(i - 1, j - 1) + 1.0 / (1.0 + dij / d02);
      double h = ws.val(i - 1, j);
      if (ws.path(i - 1, j))
        h += gap_open;
      double v = ws.val(i, j - 1);
      if (ws.path(i, j - 1))
        v += gap_open;

      if (d >= h && d >= v) {
        ws.set_path(i, j, true);
        ws.val(i, j) = d;
      } else {
        ws.set_path(i, j, false);
        ws.val(i, j) = (v >= h) ? v : h;
      }
    }
  }

  int i = len1, j = len2;
  while (i > 0 && j > 0) {
    if (ws.path(i, j)) {
      j2i[j - 1] = i - 1;
      i--;
      j--;
    } else {
      double h = ws.val(i - 1, j);
      if (ws.path(i - 1, j))
        h += gap_open;
      double v = ws.val(i, j - 1);
      if (ws.path(i, j - 1))
        v += gap_open;
      if (v >= h)
        j--;
      else
        i--;
    }
  }
}

// NW with secondary structure matching
static void nw_align_ss(DPWorkspace& ws, const std::vector<char>& secx,
    const std::vector<char>& secy, int len1, int len2, double gap_open,
    std::vector<int>& j2i)
{
  for (int i = 0; i <= len1; i++) {
    ws.val(i, 0) = 0;
    ws.set_path(i, 0, false);
  }
  for (int j = 0; j <= len2; j++) {
    ws.val(0, j) = 0;
    ws.set_path(0, j, false);
  }
  for (int j = 0; j < len2; j++)
    j2i[j] = -1;

  for (int i = 1; i <= len1; i++) {
    for (int j = 1; j <= len2; j++) {
      double d = ws.val(i - 1, j - 1) + 1.0 * (secx[i - 1] == secy[j - 1]);
      double h = ws.val(i - 1, j);
      if (ws.path(i - 1, j))
        h += gap_open;
      double v = ws.val(i, j - 1);
      if (ws.path(i, j - 1))
        v += gap_open;

      if (d >= h && d >= v) {
        ws.set_path(i, j, true);
        ws.val(i, j) = d;
      } else {
        ws.set_path(i, j, false);
        ws.val(i, j) = (v >= h) ? v : h;
      }
    }
  }

  int i = len1, j = len2;
  while (i > 0 && j > 0) {
    if (ws.path(i, j)) {
      j2i[j - 1] = i - 1;
      i--;
      j--;
    } else {
      double h = ws.val(i - 1, j);
      if (ws.path(i - 1, j))
        h += gap_open;
      double v = ws.val(i, j - 1);
      if (ws.path(i, j - 1))
        v += gap_open;
      if (v >= h)
        j--;
      else
        i--;
    }
  }
}

// ========================================================================
// Initial alignment seeds
// ========================================================================

// Seed 1: gapless threading
static double get_initial(DPWorkspace& ws, const std::vector<glm::dvec3>& x,
    const std::vector<glm::dvec3>& y, int xlen, int ylen, std::vector<int>& y2x,
    double d0, double d0_search, bool fast_opt, Superposition& sup)
{
  int min_len = std::min(xlen, ylen);
  int min_ali = min_len / 2;
  if (min_ali <= 5)
    min_ali = 5;
  int n1 = -ylen + min_ali;
  int n2 = xlen - min_ali;

  int k_best = n1;
  double tmscore_max = -1;

  for (int k = n1; k <= n2; k += (fast_opt ? 5 : 1)) {
    for (int j = 0; j < ylen; j++) {
      int i = j + k;
      y2x[j] = (i >= 0 && i < xlen) ? i : -1;
    }

    double tmscore =
        get_score_fast(ws, x, y, xlen, ylen, y2x, d0, d0_search, sup);
    if (tmscore >= tmscore_max) {
      tmscore_max = tmscore;
      k_best = k;
    }
  }

  for (int j = 0; j < ylen; j++) {
    int i = j + k_best;
    y2x[j] = (i >= 0 && i < xlen) ? i : -1;
  }
  return tmscore_max;
}

// Seed 2: secondary structure alignment
static void get_initial_ss(DPWorkspace& ws, const std::vector<char>& secx,
    const std::vector<char>& secy, int xlen, int ylen, std::vector<int>& y2x)
{
  nw_align_ss(ws, secx, secy, xlen, ylen, -1.0, y2x);
}

// Seed 3: local fragment superposition
static bool get_initial5(DPWorkspace& ws, const std::vector<glm::dvec3>& x,
    const std::vector<glm::dvec3>& y, int xlen, int ylen, std::vector<int>& y2x,
    double d0, double d0_search, bool fast_opt, double D0_MIN)
{
  double rmsd;
  Superposition sup;

  double d01 = d0 + 1.5;
  if (d01 < D0_MIN)
    d01 = D0_MIN;
  double d02 = d01 * d01;

  double GLmax = 0;
  int aL = std::min(xlen, ylen);
  std::vector<int> invmap(ylen, -1);

  int n_jump1;
  if (xlen > 250)
    n_jump1 = 45;
  else if (xlen > 200)
    n_jump1 = 35;
  else if (xlen > 150)
    n_jump1 = 25;
  else
    n_jump1 = 15;
  if (n_jump1 > xlen / 3)
    n_jump1 = xlen / 3;

  int n_jump2;
  if (ylen > 250)
    n_jump2 = 45;
  else if (ylen > 200)
    n_jump2 = 35;
  else if (ylen > 150)
    n_jump2 = 25;
  else
    n_jump2 = 15;
  if (n_jump2 > ylen / 3)
    n_jump2 = ylen / 3;

  int n_frag[2] = {20, 100};
  if (n_frag[0] > aL / 3)
    n_frag[0] = aL / 3;
  if (n_frag[1] > aL / 2)
    n_frag[1] = aL / 2;

  if (fast_opt) {
    n_jump1 *= 5;
    n_jump2 *= 5;
  }

  bool flag = false;
  for (int i_frag = 0; i_frag < 2; i_frag++) {
    int m1 = xlen - n_frag[i_frag] + 1;
    int m2 = ylen - n_frag[i_frag] + 1;

    for (int i = 0; i < m1; i += n_jump1) {
      for (int j = 0; j < m2; j += n_jump2) {
        for (int k = 0; k < n_frag[i_frag]; k++) {
          ws.r1[k] = x[k + i];
          ws.r2[k] = y[k + j];
        }

        kabsch(ws.r1, ws.r2, n_frag[i_frag], 1, rmsd, sup);
        nw_align_transform(ws, x, y, xlen, ylen, sup, d02, 0.0, invmap);
        double GL =
            get_score_fast(ws, x, y, xlen, ylen, invmap, d0, d0_search, sup);
        if (GL > GLmax) {
          GLmax = GL;
          for (int ii = 0; ii < ylen; ii++)
            y2x[ii] = invmap[ii];
          flag = true;
        }
      }
    }
  }
  return flag;
}

// Score matrix for ssplus seed
static void score_matrix_rmsd_sec(DPWorkspace& ws,
    const std::vector<char>& secx, const std::vector<char>& secy,
    const std::vector<glm::dvec3>& x, const std::vector<glm::dvec3>& y,
    int xlen, int ylen, const std::vector<int>& y2x, double D0_MIN, double d0)
{
  Superposition sup;
  double rmsd;
  double d01 = d0 + 1.5;
  if (d01 < D0_MIN)
    d01 = D0_MIN;
  double d02 = d01 * d01;

  int k = 0;
  for (int j = 0; j < ylen; j++) {
    int i = y2x[j];
    if (i >= 0) {
      ws.r1[k] = x[i];
      ws.r2[k] = y[j];
      k++;
    }
  }
  kabsch(ws.r1, ws.r2, k, 1, rmsd, sup);

  for (int ii = 0; ii < xlen; ii++) {
    auto xx = apply_transform(x[ii], sup);
    for (int jj = 0; jj < ylen; jj++) {
      double dij = dist2(xx, y[jj]);
      if (secx[ii] == secy[jj])
        ws.score(ii + 1, jj + 1) = 1.0 / (1.0 + dij / d02) + 0.5;
      else
        ws.score(ii + 1, jj + 1) = 1.0 / (1.0 + dij / d02);
    }
  }
}

// Seed 4: secondary structure plus structural distance
static void get_initial_ssplus(DPWorkspace& ws, const std::vector<char>& secx,
    const std::vector<char>& secy, const std::vector<glm::dvec3>& x,
    const std::vector<glm::dvec3>& y, int xlen, int ylen,
    const std::vector<int>& y2x0, std::vector<int>& y2x, double D0_MIN,
    double d0)
{
  score_matrix_rmsd_sec(ws, secx, secy, x, y, xlen, ylen, y2x0, D0_MIN, d0);
  nw_align_score(ws, xlen, ylen, -1.0, y2x);
}

// Find max continuous fragment
static void find_max_frag(const std::vector<glm::dvec3>& x, int len,
    int& start_max, int& end_max, double dcu0, bool fast_opt)
{
  int fra_min = fast_opt ? 8 : 4;
  int r_min = static_cast<int>(len / 3.0);
  if (r_min > fra_min)
    r_min = fra_min;

  int Lfr_max = 0;
  double dcu_cut = dcu0 * dcu0;
  int inc = 0;

  while (Lfr_max < r_min) {
    Lfr_max = 0;
    int j = 1;
    int start = 0;
    for (int i = 1; i < len; i++) {
      if (dist2(x[i - 1], x[i]) < dcu_cut) {
        j++;
        if (i == len - 1) {
          if (j > Lfr_max) {
            Lfr_max = j;
            start_max = start;
            end_max = i;
          }
          j = 1;
        }
      } else {
        if (j > Lfr_max) {
          Lfr_max = j;
          start_max = start;
          end_max = i - 1;
        }
        j = 1;
        start = i;
      }
    }
    if (Lfr_max < r_min) {
      inc++;
      double dinc = std::pow(1.1, static_cast<double>(inc)) * dcu0;
      dcu_cut = dinc * dinc;
    }
  }
}

// Seed 5: fragment gapless threading
static double get_initial_fgt(DPWorkspace& ws, const std::vector<glm::dvec3>& x,
    const std::vector<glm::dvec3>& y, int xlen, int ylen, std::vector<int>& y2x,
    double d0, double d0_search, double dcu0, bool fast_opt, Superposition& sup)
{
  int fra_min = fast_opt ? 8 : 4;
  int fra_min1 = fra_min - 1;

  int xstart = 0, ystart = 0, xend = 0, yend = 0;
  find_max_frag(x, xlen, xstart, xend, dcu0, fast_opt);
  find_max_frag(y, ylen, ystart, yend, dcu0, fast_opt);

  int Lx = xend - xstart + 1;
  int Ly = yend - ystart + 1;
  int L_fr = std::min(Lx, Ly);
  std::vector<int> ifr(L_fr);
  std::vector<int> y2x_(ylen, -1);

  if (Lx < Ly || (Lx == Ly && xlen <= ylen)) {
    for (int i = 0; i < L_fr; i++)
      ifr[i] = xstart + i;
  } else {
    for (int i = 0; i < L_fr; i++)
      ifr[i] = ystart + i;
  }

  int L0 = std::min(xlen, ylen);
  if (L_fr == L0) {
    int n1 = static_cast<int>(L0 * 0.1);
    int n2 = static_cast<int>(L0 * 0.89);
    int j = 0;
    for (int i = n1; i <= n2; i++)
      ifr[j++] = ifr[i];
    L_fr = j;
  }

  double tmscore_max = -1;

  if (Lx < Ly || (Lx == Ly && xlen <= ylen)) {
    int L1 = L_fr;
    int min_len = std::min(L1, ylen);
    int min_ali = static_cast<int>(min_len / 2.5);
    if (min_ali <= fra_min1)
      min_ali = fra_min1;
    int n1 = -ylen + min_ali;
    int n2 = L1 - min_ali;

    for (int k = n1; k <= n2; k += (fast_opt ? 3 : 1)) {
      for (int j = 0; j < ylen; j++) {
        int i = j + k;
        y2x_[j] = (i >= 0 && i < L1) ? ifr[i] : -1;
      }

      double tmscore =
          get_score_fast(ws, x, y, xlen, ylen, y2x_, d0, d0_search, sup);
      if (tmscore >= tmscore_max) {
        tmscore_max = tmscore;
        for (int j = 0; j < ylen; j++)
          y2x[j] = y2x_[j];
      }
    }
  } else {
    int L2 = L_fr;
    int min_len = std::min(xlen, L2);
    int min_ali = static_cast<int>(min_len / 2.5);
    if (min_ali <= fra_min1)
      min_ali = fra_min1;
    int n1 = -L2 + min_ali;
    int n2 = xlen - min_ali;

    for (int k = n1; k <= n2; k++) {
      for (int j = 0; j < ylen; j++)
        y2x_[j] = -1;

      for (int j = 0; j < L2; j++) {
        int i = j + k;
        if (i >= 0 && i < xlen)
          y2x_[ifr[j]] = i;
      }

      double tmscore =
          get_score_fast(ws, x, y, xlen, ylen, y2x_, d0, d0_search, sup);
      if (tmscore >= tmscore_max) {
        tmscore_max = tmscore;
        for (int j = 0; j < ylen; j++)
          y2x[j] = y2x_[j];
      }
    }
  }

  return tmscore_max;
}

// ========================================================================
// DP_iter — iterative DP refinement (from TMalign.h:DP_iter)
// ========================================================================

static double DP_iter(DPWorkspace& ws, const std::vector<glm::dvec3>& x,
    const std::vector<glm::dvec3>& y, int xlen, int ylen, Superposition& sup,
    std::vector<int>& invmap0, int g1, int g2, int iteration_max,
    double local_d0_search, double D0_MIN, double Lnorm, double d0,
    double score_d8)
{
  double gap_open[2] = {-0.6, 0};
  double rmsd;
  std::vector<int> invmap(ylen, -1);

  double tmscore_max = -1, tmscore_old = 0;
  int score_sum_method = 8, simplify_step = 40;
  double d02 = d0 * d0;

  for (int g = g1; g < g2; g++) {
    for (int iteration = 0; iteration < iteration_max; iteration++) {
      nw_align_transform(ws, x, y, xlen, ylen, sup, d02, gap_open[g], invmap);

      int k = 0;
      for (int j = 0; j < ylen; j++) {
        int i = invmap[j];
        if (i >= 0) {
          ws.xtm[k] = x[i];
          ws.ytm[k] = y[j];
          k++;
        }
      }

      double tmscore = TMscore8_search(ws, k, sup, simplify_step,
          score_sum_method, local_d0_search, score_d8, d0, Lnorm);

      if (tmscore > tmscore_max) {
        tmscore_max = tmscore;
        for (int i = 0; i < ylen; i++)
          invmap0[i] = invmap[i];
      }

      if (iteration > 0) {
        if (std::fabs(tmscore_old - tmscore) < 0.000001)
          break;
      }
      tmscore_old = tmscore;
    }
  }
  return tmscore_max;
}

// ========================================================================
// Main orchestrator: tm_align
// ========================================================================

TMAlignResult TMalign(const std::vector<glm::dvec3>& target_ca,
    const std::vector<glm::dvec3>& mobile_ca, const std::string& target_seq,
    const std::string& mobile_seq, bool fast)
{
  // In USalign convention: x = mobile (structure to move), y = target
  // The invmap maps target indices to mobile indices: invmap[j_target] =
  // i_mobile
  const auto& x = mobile_ca;
  const auto& y = target_ca;
  const auto& seqx = mobile_seq;
  const auto& seqy = target_seq;
  int xlen = static_cast<int>(x.size());
  int ylen = static_cast<int>(y.size());

  TMAlignResult result;

  if (xlen < 3 || ylen < 3)
    return result;

  // Secondary structure
  std::vector<char> secx, secy;
  assign_secondary_structure(x, secx);
  assign_secondary_structure(y, secy);

  // Allocate workspace
  DPWorkspace ws;
  ws.resize(xlen, ylen);

  // Parameters
  auto params = compute_search_params(xlen, ylen);
  int simplify_step = 40;
  int score_sum_method = 8;

  std::vector<int> invmap0(ylen, -1);
  std::vector<int> invmap(ylen, -1);
  double TMmax = -1;
  Superposition sup, sup_best;

  double ddcc = (params.Lnorm <= 40) ? 0.1 : 0.4;
  double local_d0_search = params.d0_search;

  // ============================================
  // Seed 1: gapless threading
  // ============================================
  get_initial(
      ws, x, y, xlen, ylen, invmap0, params.d0, params.d0_search, fast, sup);
  double TM = detailed_search(ws, x, y, xlen, ylen, invmap0, sup, simplify_step,
      score_sum_method, local_d0_search, params.score_d8, params.d0,
      params.Lnorm);
  if (TM > TMmax)
    TMmax = TM;

  TM = DP_iter(ws, x, y, xlen, ylen, sup, invmap, 0, 2, fast ? 2 : 30,
      local_d0_search, params.D0_MIN, params.Lnorm, params.d0, params.score_d8);
  if (TM > TMmax) {
    TMmax = TM;
    for (int i = 0; i < ylen; i++)
      invmap0[i] = invmap[i];
  }

  // ============================================
  // Seed 2: secondary structure alignment
  // ============================================
  get_initial_ss(ws, secx, secy, xlen, ylen, invmap);
  TM = detailed_search(ws, x, y, xlen, ylen, invmap, sup, simplify_step,
      score_sum_method, local_d0_search, params.score_d8, params.d0,
      params.Lnorm);
  if (TM > TMmax) {
    TMmax = TM;
    for (int i = 0; i < ylen; i++)
      invmap0[i] = invmap[i];
  }
  if (TM > TMmax * 0.2) {
    TM = DP_iter(ws, x, y, xlen, ylen, sup, invmap, 0, 2, fast ? 2 : 30,
        local_d0_search, params.D0_MIN, params.Lnorm, params.d0,
        params.score_d8);
    if (TM > TMmax) {
      TMmax = TM;
      for (int i = 0; i < ylen; i++)
        invmap0[i] = invmap[i];
    }
  }

  // ============================================
  // Seed 3: local fragment superposition
  // ============================================
  if (get_initial5(ws, x, y, xlen, ylen, invmap, params.d0, params.d0_search,
          fast, params.D0_MIN)) {
    TM = detailed_search(ws, x, y, xlen, ylen, invmap, sup, simplify_step,
        score_sum_method, local_d0_search, params.score_d8, params.d0,
        params.Lnorm);
    if (TM > TMmax) {
      TMmax = TM;
      for (int i = 0; i < ylen; i++)
        invmap0[i] = invmap[i];
    }
    if (TM > TMmax * ddcc) {
      TM = DP_iter(ws, x, y, xlen, ylen, sup, invmap, 0, 2, 2, local_d0_search,
          params.D0_MIN, params.Lnorm, params.d0, params.score_d8);
      if (TM > TMmax) {
        TMmax = TM;
        for (int i = 0; i < ylen; i++)
          invmap0[i] = invmap[i];
      }
    }
  }

  // ============================================
  // Seed 4: SS + structural distance
  // ============================================
  get_initial_ssplus(ws, secx, secy, x, y, xlen, ylen, invmap0, invmap,
      params.D0_MIN, params.d0);
  TM = detailed_search(ws, x, y, xlen, ylen, invmap, sup, simplify_step,
      score_sum_method, local_d0_search, params.score_d8, params.d0,
      params.Lnorm);
  if (TM > TMmax) {
    TMmax = TM;
    for (int i = 0; i < ylen; i++)
      invmap0[i] = invmap[i];
  }
  if (TM > TMmax * ddcc) {
    TM = DP_iter(ws, x, y, xlen, ylen, sup, invmap, 0, 2, fast ? 2 : 30,
        local_d0_search, params.D0_MIN, params.Lnorm, params.d0,
        params.score_d8);
    if (TM > TMmax) {
      TMmax = TM;
      for (int i = 0; i < ylen; i++)
        invmap0[i] = invmap[i];
    }
  }

  // ============================================
  // Seed 5: fragment gapless threading
  // ============================================
  get_initial_fgt(ws, x, y, xlen, ylen, invmap, params.d0, params.d0_search,
      params.dcu0, fast, sup);
  TM = detailed_search(ws, x, y, xlen, ylen, invmap, sup, simplify_step,
      score_sum_method, local_d0_search, params.score_d8, params.d0,
      params.Lnorm);
  if (TM > TMmax) {
    TMmax = TM;
    for (int i = 0; i < ylen; i++)
      invmap0[i] = invmap[i];
  }
  if (TM > TMmax * ddcc) {
    TM = DP_iter(ws, x, y, xlen, ylen, sup, invmap, 1, 2, 2, local_d0_search,
        params.D0_MIN, params.Lnorm, params.d0, params.score_d8);
    if (TM > TMmax) {
      TMmax = TM;
      for (int i = 0; i < ylen; i++)
        invmap0[i] = invmap[i];
    }
  }

  // ============================================
  // Verify alignment exists
  // ============================================
  {
    bool flag = false;
    for (int i = 0; i < ylen; i++) {
      if (invmap0[i] >= 0) {
        flag = true;
        break;
      }
    }
    if (!flag)
      return result;
  }

  // ============================================
  // Final detailed search with simplify_step=1
  // ============================================
  simplify_step = fast ? 40 : 1;
  score_sum_method = 8;
  detailed_search(ws, x, y, xlen, ylen, invmap0, sup, simplify_step,
      score_sum_method, local_d0_search, params.score_d8, params.d0);

  // Apply rotation to mobile and select pairs within score_d8
  {
    std::vector<glm::dvec3> xt(xlen);
    do_rotation(x, xt, xlen, sup);

    std::vector<int> m1, m2; // aligned indices
    int n_ali = 0;
    for (int j = 0; j < ylen; j++) {
      int i = invmap0[j];
      if (i >= 0) {
        n_ali++;
        double d = std::sqrt(dist2(xt[i], y[j]));
        if (d <= params.score_d8) {
          m1.push_back(i);
          m2.push_back(j);

          ws.xtm[static_cast<int>(m1.size()) - 1] = x[i];
          ws.ytm[static_cast<int>(m1.size()) - 1] = y[j];
          ws.r1[static_cast<int>(m1.size()) - 1] = xt[i];
          ws.r2[static_cast<int>(m1.size()) - 1] = y[j];
        }
      }
    }
    int n_ali8 = static_cast<int>(m1.size());

    if (n_ali8 < 1)
      return result;

    // RMSD of aligned residues
    double rmsd0;
    Superposition sup_rmsd;
    kabsch(ws.r1, ws.r2, n_ali8, 0, rmsd0, sup_rmsd);
    rmsd0 = std::sqrt(rmsd0 / n_ali8);

    // ============================================
    // Final TM-scores normalized by each length
    // ============================================
    simplify_step = 1;
    score_sum_method = 0;
    Superposition sup_final;

    // Normalized by target length (ylen)
    double D0_MIN_f, Lnorm_f, d0_target, d0_search_f;
    compute_final_params(
        static_cast<double>(ylen), D0_MIN_f, Lnorm_f, d0_target, d0_search_f);
    double TM1 = TMscore8_search(ws, n_ali8, sup_final, simplify_step,
        score_sum_method, d0_search_f, params.score_d8, d0_target, Lnorm_f);

    // Normalized by mobile length (xlen)
    double d0_mobile;
    compute_final_params(
        static_cast<double>(xlen), D0_MIN_f, Lnorm_f, d0_mobile, d0_search_f);
    Superposition sup_mobile;
    double TM2 = TMscore8_search(ws, n_ali8, sup_mobile, simplify_step,
        score_sum_method, d0_search_f, params.score_d8, d0_mobile, Lnorm_f);

    // Use the rotation from target-normalized scoring for the final transform
    // (matches original TMalign behavior: t0, u0 come from TM1 computation)

    // ============================================
    // Build alignment strings
    // ============================================
    // d0 for alignment display (same as target normalization)
    double d0_out = d0_target;

    // Re-rotate with best transform for alignment string generation
    do_rotation(x, xt, xlen, sup_final);

    int ali_len = xlen + ylen;
    std::string seqxA(ali_len, '-');
    std::string seqM(ali_len, ' ');
    std::string seqyA(ali_len, '-');

    int kk = 0, i_old = 0, j_old = 0;
    double Liden = 0;
    for (int k = 0; k < n_ali8; k++) {
      for (int i = i_old; i < m1[k]; i++) {
        seqxA[kk] = (i < static_cast<int>(seqx.size())) ? seqx[i] : 'X';
        seqyA[kk] = '-';
        seqM[kk] = ' ';
        kk++;
      }
      for (int j = j_old; j < m2[k]; j++) {
        seqxA[kk] = '-';
        seqyA[kk] = (j < static_cast<int>(seqy.size())) ? seqy[j] : 'X';
        seqM[kk] = ' ';
        kk++;
      }

      char cx = (m1[k] < static_cast<int>(seqx.size())) ? seqx[m1[k]] : 'X';
      char cy = (m2[k] < static_cast<int>(seqy.size())) ? seqy[m2[k]] : 'X';
      seqxA[kk] = cx;
      seqyA[kk] = cy;
      Liden += (cx == cy) ? 1 : 0;
      double dd = std::sqrt(dist2(xt[m1[k]], y[m2[k]]));
      seqM[kk] = (dd < d0_out) ? ':' : '.';
      kk++;
      i_old = m1[k] + 1;
      j_old = m2[k] + 1;
    }
    // tail
    for (int i = i_old; i < xlen; i++) {
      seqxA[kk] = (i < static_cast<int>(seqx.size())) ? seqx[i] : 'X';
      seqyA[kk] = '-';
      seqM[kk] = ' ';
      kk++;
    }
    for (int j = j_old; j < ylen; j++) {
      seqxA[kk] = '-';
      seqyA[kk] = (j < static_cast<int>(seqy.size())) ? seqy[j] : 'X';
      seqM[kk] = ' ';
      kk++;
    }

    // Fill result
    result.tm_score_target = TM1;
    result.tm_score_mobile = TM2;
    result.d0_target = d0_target;
    result.d0_mobile = d0_mobile;
    result.rmsd = rmsd0;
    result.aligned_length = n_ali8;
    result.seq_identity = (n_ali8 > 0) ? Liden / n_ali8 : 0.0;
    result.transform = sup_final;
    result.mobile_indices.assign(m1.begin(), m1.end());
    result.target_indices.assign(m2.begin(), m2.end());
    result.seq_mobile = seqxA.substr(0, kk);
    result.seq_target = seqyA.substr(0, kk);
    result.seq_match = seqM.substr(0, kk);
  }

  return result;
}

} // namespace pymol::usalign
