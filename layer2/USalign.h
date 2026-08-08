#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace pymol::usalign {

struct Superposition {
  glm::dvec3 translation{0.0};
  glm::dmat3 rotation{1.0};
};

struct TMAlignResult {
  double tm_score_mobile = 0.0;  // normalized by mobile length
  double tm_score_target = 0.0;  // normalized by target length
  double d0_target = 0.0;        // d0 used for target normalization
  double d0_mobile = 0.0;        // d0 used for mobile normalization
  double rmsd = 0.0;
  int aligned_length = 0;        // aligned pairs within distance cutoff
  double seq_identity = 0.0;
  Superposition transform;
  std::vector<int> mobile_indices;  // paired residue indices into mobile CA array
  std::vector<int> target_indices;  // paired residue indices into target CA array
  std::string seq_mobile;           // alignment string for mobile
  std::string seq_target;           // alignment string for target
  std::string seq_match;            // ':' close, '.' far, ' ' gap
};

// DP workspace — single allocation reused across all seeds
struct DPWorkspace {
  std::vector<double> score_flat;
  std::vector<double> val_flat;
  std::vector<char> path_flat;
  int rows = 0;
  int cols = 0;

  // Scratch buffers for TMscore8_search and scoring.
  // xtm, ytm, r1, r2 are sized to min(xlen, ylen); xt is sized to xlen.
  // All score_fun8 n_cut values are bounded by min(xlen, ylen).
  std::vector<glm::dvec3> xtm, ytm, xt, r1, r2;

  void resize(int xlen, int ylen);

  double& score(int i, int j) { return score_flat[i * cols + j]; }
  double& val(int i, int j) { return val_flat[i * cols + j]; }
  bool path(int i, int j) const { return path_flat[i * cols + j] != 0; }
  void set_path(int i, int j, bool v) { path_flat[i * cols + j] = v ? 1 : 0; }
};

/**
 * Perform TM-score structural alignment between two protein structures.
 * 
 * @param target_ca Target structure CA coordinates (remains fixed)
 * @param mobile_ca Mobile structure CA coordinates (will be aligned to target)
 * @param target_seq Target sequence (single-letter amino acid codes)
 * @param mobile_seq Mobile sequence (single-letter amino acid codes)
 * @param fast Use fast mode with fewer iterations (default: false)
 * @return TMAlignResult containing TM-scores, RMSD, alignment, and transform
 * 
 * @note TM-score ranges from 0 to 1; score > 0.5 indicates same fold
 * @note Complexity: O(n²) where n = min(target_len, mobile_len)
 * @note may use a lot of memory for structures above 10K residues.
 */
TMAlignResult TMalign(
    const std::vector<glm::dvec3>& target_ca,
    const std::vector<glm::dvec3>& mobile_ca,
    const std::string& target_seq,
    const std::string& mobile_seq,
    bool fast = false);

} // namespace pymol::usalign
