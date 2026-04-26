"""
#A* -------------------------------------------------------------------
#B* This file contains source code for running automation tasks related
#-* to the build process of the PyMOL computer program
#C* Copyright 2025 by Martin Urban.
#D* -------------------------------------------------------------------
#E* It is unlawful to modify or remove this copyright notice.
#F* -------------------------------------------------------------------
#G* Please see the accompanying LICENSE file for further information.
#H* -------------------------------------------------------------------
#I* Additional authors of this source file include:
#-*
#-*
#-*
#Z* -------------------------------------------------------------------
"""
import pathlib
import subprocess
from task_automator.utils import github_utils
import const
import utils


def setup_dev_env() -> None:
  """Installs the dependencies needed for building the _cmd extension module."""
  # <editor-fold desc="OS specific code">
  if const.WIN32:
    tmp_python_exe_filepath = pathlib.Path(const.PROJECT_ROOT_DIR / ".venv/Scripts/python")
    tmp_vcpkg_bootstrap_filename = "bootstrap-vcpkg.bat"
  elif const.__APPLE__ or const.__linux__:
    tmp_python_exe_filepath = pathlib.Path(const.PROJECT_ROOT_DIR / ".venv/bin/python")
    tmp_vcpkg_bootstrap_filename = "bootstrap-vcpkg.sh"
  else:
    exit(const.invalid_platform())
  # </editor-fold>
  # <editor-fold desc="Setup pymol-open-source repository">
  # if not pathlib.Path(f"{const.PROJECT_ROOT_DIR}/vendor/pymol-open-source").exists():
  #   github_utils.git_clone(
  #     "https://github.com/schrodinger/pymol-open-source.git",
  #     pathlib.Path(f"{const.PROJECT_ROOT_DIR}/vendor/pymol-open-source")
  #   )
  #   github_utils.git_checkout(
  #     "0313aeba9d75f464e4dddccc3bdbee71a5afb049",
  #     pathlib.Path(f"{const.PROJECT_ROOT_DIR}/vendor/pymol-open-source")
  #   )
  #   subprocess.run(
  #     [tmp_python_exe_filepath, pathlib.Path(f"{const.PROJECT_ROOT_DIR}/scripts/python/create_generated_files.py")]
  #   )
  # else:
  #   print("pymol-open-source already setup.")
  subprocess.run(
    [tmp_python_exe_filepath, pathlib.Path(f"{const.PROJECT_ROOT_DIR}/scripts/python/create_generated_files.py")]
  )
  # </editor-fold>
  # <editor-fold desc="Setup vcpkg repository">
  if not pathlib.Path(f"{const.PROJECT_ROOT_DIR}/vendor/vcpkg").exists():
    github_utils.git_clone(
      "https://github.com/microsoft/vcpkg.git",
      pathlib.Path(f"{const.PROJECT_ROOT_DIR}/vendor/vcpkg")
    )
    if const.__APPLE__ or const.__linux__:
      subprocess.run(["chmod", "+x", pathlib.Path(f"./{tmp_vcpkg_bootstrap_filename}")], cwd=pathlib.Path(const.PROJECT_ROOT_DIR / "vendor/vcpkg"))
    #subprocess.run([pathlib.Path(f"./{tmp_vcpkg_bootstrap_filename}")], shell=True, cwd=pathlib.Path(const.PROJECT_ROOT_DIR / "vendor/vcpkg"))
    subprocess.run([str(pathlib.Path(const.PROJECT_ROOT_DIR / "vendor/vcpkg" / tmp_vcpkg_bootstrap_filename))], shell=True, cwd=pathlib.Path(const.PROJECT_ROOT_DIR / "vendor/vcpkg"))
    tmp_vcpkg_install_cmd = [pathlib.Path(const.PROJECT_ROOT_DIR / "vendor/vcpkg" / "vcpkg"), "install"]
    if const.WIN32:
      tmp_vcpkg_install_cmd.append("--triplet=x64-windows-static")
      subprocess.run(tmp_vcpkg_install_cmd, shell=True)
    elif const.__APPLE__ or const.__linux__:
      subprocess.run(tmp_vcpkg_install_cmd)
  else:
    print("vcpkg already setup.")
  # </editor-fold>
  #utils.copy_pymol_sources()
