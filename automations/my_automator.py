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
import sys
sys.path.append(str(pathlib.Path(__file__).parent))

import build_wheel
import build_macos_so
import run_pytest
import dev_env


AUTOMATION_TREE = {
  "setup": {
    "help": "Setup automations",
    "subcommands": {
      "dev-env": {
        "help": "Sets up the development environment",
        "func": dev_env.setup_dev_env
      }
    }
  },
  "build": {
    "help": "Build targets",
    "subcommands": {
      "wheel": {
        "help": "Builds the Python wheel file",
        "func": build_wheel.run_wheel_build
      },
      "so": {
        "help": "Compiles the _cmd module from source",
        "func": build_macos_so.build_cmd_module
      }
    }
  },
  "test": {
    "help": "Runs all tests under the tests/ directory using pytest.",
    "func": run_pytest.run_pytest_suite
  }
}


if __name__ == "__main__":
  from task_automator import automator
  automator.Automator(AUTOMATION_TREE).run()
