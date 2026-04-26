cmake_minimum_required(VERSION 3.15...3.26)
set(CMAKE_VERBOSE_MAKEFILE on)
# The first step is to detect on which operating system the build runs.
# This is important because based on that information different compiler and
# linker flags have to be used.
#
# The following CMake argument has to be given if CMake gets invoked
# in the command line:
# -DCMAKE_TOOLCHAIN_FILE=./vendor/vcpkg/scripts/buildsystems/vcpkg.cmake
#
# If CLion is used for the build process, be sure to add the argument to
# the "CMake profile" under "CMake options".
if(WIN32)
    message("Running on Windows.")
    set(PLATFORM WINDOWS)
    # The vcpkg target triplet has to be set BEFORE the project() function
    # of CMake!
    if(DEFINED ENV{WIN_ARCH})
        # The cibuildwheel tool builds wheel files for both 32- and 64-Bit
        # Windows, therefore the vcpkg target triplet needs to be switched
        # accordingly.
        if($ENV{WIN_ARCH} STREQUAL "x86")
            set(VCPKG_TARGET_TRIPLET x86-windows-static)
            set(ARCH_SPECIFIC_LIB_DIR ${CMAKE_SOURCE_DIR}/pre-built/windows/libx86)
        elseif($ENV{WIN_ARCH} STREQUAL "x64")
            set(VCPKG_TARGET_TRIPLET x64-windows-static)
            set(ARCH_SPECIFIC_LIB_DIR ${CMAKE_SOURCE_DIR}/pre-built/windows/libx64)
        endif()
    else()
        message(FATAL_ERROR "WIN_ARCH environment variable is not defined")
    endif()
elseif(APPLE)
    # On macOS it is necessary to set an environment variable called ARCH with the
    # arch (either x86_64 or arm64) of your Mac
    # Example:
    #   export ARCH=x86_64
    #   export ARCH=arm64
    #
    # If CLion is used, add the environment variable ARCH in the
    # "Environment" text box under the "CMake profile".
    set(PLATFORM DARWIN)
    if(DEFINED ENV{ARCH})
        set(CMAKE_OSX_ARCHITECTURES $ENV{ARCH}) # Either x86_64 or arm64
    else()
        message(FATAL_ERROR "ARCH environment variable is not defined")
    endif()
elseif(UNIX)
    # Be aware that the CMake UNIX variable always refers to GNU Linux in
    # this CMake file! This could lead to problems if something like OpenBSD
    # or FreeBSD is used.
    set(PLATFORM GNU_LINUX)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

project(_cmd)
# ----- OS specific section
if (${PLATFORM} STREQUAL "WINDOWS")
    message("Running Windows build")
    add_library(opengl32 STATIC IMPORTED)
    set_target_properties(opengl32 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/lib/OpenGL32.Lib)

    set(LIB_DIR
            ${CMAKE_SOURCE_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/lib
            ${ARCH_SPECIFIC_LIB_DIR}
    )
    set(ALL_COMPILER_ARGS
            /MP
            /openmp
            /std:c++17
    )
    set (OS_SPECIFIC_INCLUDES
            vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include
    )
    set (OS_SPECIFIC_LIBRARY_NAMES
            Advapi32 Ws2_32 glew32 freetype libpng16 libxml2 opengl32 netCDF::netcdf
    )
    set (OS_SPECIFIC_COMPILER_DEFS
            WIN32
            PYMOL_OPENMP
            _HAVE_LIBXML # I had linker problems using this macro
    )
endif ()

if (${PLATFORM} STREQUAL "DARWIN")
    # ----- Architecture specific section
    if (${CMAKE_OSX_ARCHITECTURES} STREQUAL "arm64")
        set(LIB_DIR ${CMAKE_SOURCE_DIR}/vcpkg_installed/arm64-osx/lib)
    else ()
        set(LIB_DIR ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-osx/lib)
    endif ()
    # --- end
    find_package(OpenGL REQUIRED)

    set(ALL_COMPILER_ARGS
            # Clang specific compiler arguments
            -Werror=return-type
            -Wunused-variable
            -Wno-switch
            -Wno-narrowing
            -Wno-char-subscripts
            -O3
            -fno-strict-aliasing
    )
    set (OS_SPECIFIC_INCLUDES
            vcpkg_installed/x64-osx/include
            pre-built/macos/include
    )
    set (OS_SPECIFIC_LINKER_OPTIONS -framework OpenGL)
    set (OS_SPECIFIC_LIBRARY_NAMES
            PNG::PNG freetype xml2 GLEW netCDF::netcdf OpenGL::GL
    )
    set (OS_SPECIFIC_COMPILER_DEFS
            _HAVE_LIBXML
            PYMOL_CURVE_VALIDATE
            _PYMOL_OSX
            # PYMOL_OPENMP  # Not used in Schrödinger's build
    )
    
endif ()

if (${PLATFORM} STREQUAL "GNU_LINUX")
    # Add Linux specific options here
    message("Running GNU Linux build")
    set(LIB_DIR ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/lib)

    add_library(opengl32 STATIC IMPORTED)
    set_target_properties(opengl32 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/pre-built/linux/libx64/libGL.so)

    set(ALL_COMPILER_ARGS
            -fopenmp
            -std=c++17
    )
    set (OS_SPECIFIC_INCLUDES
            vcpkg_installed/x64-linux/include
            # pre-built/linux/include
    )
    set (OS_SPECIFIC_LINKER_OPTIONS -fopenmp)
    set (OS_SPECIFIC_LIBRARY_NAMES
            freetype opengl32 GLEW PNG::PNG xml2 netCDF::netcdf
    )
    set (OS_SPECIFIC_COMPILER_DEFS
            _HAVE_LIBXML
    )
endif ()
# --- end
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

set(ALL_SRC
        build/generated/ShaderText.cpp
        contrib/uiuc/plugins/molfile_plugin/src/PlugIOManagerInit.c
        contrib/uiuc/plugins/molfile_plugin/src/abinitplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/avsplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/basissetplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/bgfplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/binposplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/biomoccaplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/brixplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/carplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/ccp4plugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/corplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/crdplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/cubeplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/dcdplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/dlpolyplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/dsn6plugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/dtrplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/dxplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/edmplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/fs4plugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/gamessplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/graspplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/grdplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/gridplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/gromacsplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/hash.c
        contrib/uiuc/plugins/molfile_plugin/src/inthash.c
        contrib/uiuc/plugins/molfile_plugin/src/jsplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/maeffplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/mapplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/mdfplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/mol2plugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/moldenplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/molemeshplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/msmsplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/namdbinplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/netcdfplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/offplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/parm7plugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/parmplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/pbeqplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/pdbplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/pdbxplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/phiplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/pltplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/plyplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/pqrplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/psfplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/raster3dplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/rst7plugin.c
        contrib/uiuc/plugins/molfile_plugin/src/situsplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/spiderplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/stlplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/tinkerplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/uhbdplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/vasp5xdatcarplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/vaspchgcarplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/vaspoutcarplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/vaspparchgplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/vaspposcarplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/vaspxdatcarplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/vaspxmlplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/vtfplugin.c
        contrib/uiuc/plugins/molfile_plugin/src/vtkplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/xbgfplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/xsfplugin.cpp
        contrib/uiuc/plugins/molfile_plugin/src/xyzplugin.c
        layer0/Bezier.cpp
        layer0/Block.cpp
        layer0/CarveHelper.cpp
        layer0/ContourSurf.cpp
        layer0/Crystal.cpp
        layer0/Err.cpp
        layer0/Feedback.cpp
        layer0/Field.cpp
        layer0/File.cpp
        layer0/GFXManager.cpp
        layer0/GenericBuffer.cpp
        layer0/GraphicsUtil.cpp
        layer0/Isosurf.cpp
        layer0/Map.cpp
        layer0/Match.cpp
        layer0/Matrix.cpp
        layer0/MemoryDebug.cpp
        layer0/MemoryUsage.cpp
        layer0/MyPNG.cpp
        layer0/Parse.cpp
        layer0/Pixmap.cpp
        layer0/PostProcess.cpp
        layer0/PrintUtils.cpp
        layer0/ShaderMgr.cpp
        layer0/ShaderPreprocessor.cpp
        layer0/ShaderPrg.cpp
        layer0/Sphere.cpp
        layer0/TTT.cpp
        layer0/Tetsurf.cpp
        layer0/Texture.cpp
        layer0/Tracker.cpp
        layer0/Triangle.cpp
        layer0/Util.cpp
        layer0/Util2.cpp
        layer0/Vector.cpp
        layer0/Word.cpp
        layer0/ccealignmodule.cpp
        layer0/marching_cubes.cpp
        layer0/os_gl.cpp
        layer1/Basis.cpp
        layer1/ButMode.cpp
        layer1/CGO.cpp
        layer1/CGOGL.cpp
        layer1/CGORenderer.cpp
        layer1/COLLADA.cpp
        layer1/Camera.cpp
        layer1/Character.cpp
        layer1/Color.cpp
        layer1/Control.cpp
        layer1/Extrude.cpp
        layer1/Font.cpp
        layer1/FontGLUT.cpp
        layer1/FontGLUT8x13.cpp
        layer1/FontGLUT9x15.cpp
        layer1/FontGLUTHel10.cpp
        layer1/FontGLUTHel12.cpp
        layer1/FontGLUTHel18.cpp
        layer1/FontType.cpp
        layer1/ImageUtils.cpp
        layer1/Movie.cpp
        layer1/Ortho.cpp
        layer1/P.cpp
        layer1/PConv.cpp
        layer1/Picking.cpp
        layer1/Pop.cpp
        layer1/PyMOLObject.cpp
        layer1/Ray.cpp
        layer1/Rep.cpp
        layer1/Scene.cpp
        layer1/SceneMouse.cpp
        layer1/ScenePicking.cpp
        layer1/SceneRay.cpp
        layer1/SceneRender.cpp
        layer1/SceneView.cpp
        layer1/ScrollBar.cpp
        layer1/Seq.cpp
        layer1/Setting.cpp
        layer1/Shaker.cpp
        layer1/SymOp.cpp
        layer1/SymOpPConv.cpp
        layer1/SymOpTools.cpp
        layer1/Symmetry.cpp
        layer1/Text.cpp
        layer1/TypeFace.cpp
        layer1/View.cpp
        layer1/Wizard.cpp
        layer2/AssemblyHelpers.cpp
        layer2/AtomInfo.cpp
        layer2/AtomInfoHistory.cpp
        layer2/BondTypeHistory.cpp
        layer2/CifFile.cpp
        layer2/CifMoleculeReader.cpp
        layer2/CoordSet.cpp
        layer2/DistSet.cpp
        layer2/GadgetSet.cpp
        layer2/HydrogenAdder.cpp
        layer2/MmodTyping.cpp
        layer2/MmtfMoleculeReader.cpp
        layer2/Mol2Typing.cpp
        layer2/MolV3000.cpp
        layer2/ObjectAlignment.cpp
        layer2/ObjectCGO.cpp
        layer2/ObjectCallback.cpp
        layer2/ObjectCurve.cpp
        layer2/ObjectDist.cpp
        layer2/ObjectGadget.cpp
        layer2/ObjectGadgetRamp.cpp
        layer2/ObjectGroup.cpp
        layer2/ObjectMap.cpp
        layer2/ObjectMesh.cpp
        layer2/ObjectMolecule.cpp
        layer2/ObjectMolecule2.cpp
        layer2/ObjectMolecule3.cpp
        layer2/ObjectSlice.cpp
        layer2/ObjectSurface.cpp
        layer2/ObjectVolume.cpp
        layer2/RepAngle.cpp
        layer2/RepCartoon.cpp
        layer2/RepCylBond.cpp
        layer2/RepDihedral.cpp
        layer2/RepDistDash.cpp
        layer2/RepDistLabel.cpp
        layer2/RepDot.cpp
        layer2/RepEllipsoid.cpp
        layer2/RepLabel.cpp
        layer2/RepMesh.cpp
        layer2/RepNonbonded.cpp
        layer2/RepNonbondedSphere.cpp
        layer2/RepRibbon.cpp
        layer2/RepSphere.cpp
        layer2/RepSphereGenerate.cpp
        layer2/RepSphereImmediate.cpp
        layer2/RepSurface.cpp
        layer2/RepWireBond.cpp
        layer2/Sculpt.cpp
        layer2/SculptCache.cpp
        layer2/SideChainHelper.cpp
        layer2/VFont.cpp
        layer3/AtomIterators.cpp
        layer3/CifDataValueFormatter.cpp
        layer3/Editor.cpp
        layer3/Executive.cpp
        layer3/ExecutivePython.cpp
        layer3/Interactions.cpp
        layer3/MaeExportHelpers.cpp
        layer3/MoleculeExporter.cpp
        layer3/MovieScene.cpp
        layer3/PlugIOManager.cpp
        layer3/RingFinder.cpp
        layer3/Seeker.cpp
        layer3/Selector.cpp
        layer3/SelectorTmp.cpp
        layer3/SpecRec.cpp
        layer3/SpecRecSpecial.cpp
        layer4/Cmd.cpp
        layer4/Menu.cpp
        layer4/PopUp.cpp
        layer5/PyMOL.cpp
        layer5/TestPyMOL.cpp
        layer5/main.cpp
        ov/src/OVContext.cpp
        ov/src/OVHeap.cpp
        ov/src/OVHeapArray.cpp
        ov/src/OVLexicon.cpp
        ov/src/OVOneToAny.cpp
        ov/src/OVOneToOne.cpp
        ov/src/OVRandom.cpp
        ov/src/ov_utility.cpp
)
python_add_library(_cmd MODULE WITH_SOABI ${ALL_SRC}) # Default target
install(TARGETS _cmd DESTINATION pymol)
# ----- Target compile section
target_compile_options(_cmd PRIVATE ${ALL_COMPILER_ARGS})
target_compile_features(_cmd PRIVATE cxx_std_17)

set(ALL_COMPILER_DEFS
        _PYMOL_LIBPNG
        _PYMOL_FREETYPE
        _PYMOL_VMD_PLUGINS
        _PYMOL_NO_MSGPACKC
        _PYMOL_NO_MAIN
        _PYMOL_NUMPY
        ${OS_SPECIFIC_COMPILER_DEFS}
)
target_compile_definitions(_cmd PUBLIC ${ALL_COMPILER_DEFS})
# --- end

# ----- Include section
set (ALL_INCLUDE_DIR
        include
        contrib/uiuc/plugins/include
        contrib/uiuc/plugins/molfile_plugin/src
        ov/src
        layer0
        layer1
        layer2
        layer3
        layer4
        layer5
        build/generated
        pre-built/include
        ${OS_SPECIFIC_INCLUDES}
        ${Python3_INCLUDE_DIRS}
)
target_include_directories(_cmd PUBLIC ${ALL_INCLUDE_DIR})
# --- end

# ----- Packages section
# The variable for the Python target is needed, otherwise CLion
# does not recognize the build target as valid target and therefore CMake fails.
find_package(freetype CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(PNG CONFIG REQUIRED)
find_package(libxml2 CONFIG REQUIRED)
find_package(netCDF CONFIG REQUIRED)
# --- end

# ----- Linker section
target_link_directories(_cmd PRIVATE ${LIB_DIR}) # Refactor LIB_DIR to something more meaningful
target_link_options(_cmd PRIVATE ${OS_SPECIFIC_LINKER_OPTIONS})
target_link_libraries(_cmd PRIVATE ${OS_SPECIFIC_LIBRARY_NAMES})
# --- end

# ----- Champ section
set(ALL_CHAMP_SRC
        contrib/champ/champ.c
        contrib/champ/champ_module.c
        contrib/champ/chiral.c
        contrib/champ/err2.c
        contrib/champ/feedback2.c
        contrib/champ/list.c
        contrib/champ/os_memory.c
        contrib/champ/sort.c
        contrib/champ/strblock.c
        contrib/champ/vla.c
)
python_add_library(_champ MODULE WITH_SOABI ${ALL_CHAMP_SRC})

# ----- Include section
set(ALL_CHAMP_INCLUDE_DIR
        contrib/champ
        pre-built/include
        ${OS_SPECIFIC_INCLUDES}
        ${Python3_INCLUDE_DIRS}
)
target_include_directories(_champ PUBLIC ${ALL_INCLUDE_DIR})
# --- end

# ----- Linker section
target_link_directories(_champ PRIVATE ${LIB_DIR})
target_link_libraries(_champ PRIVATE ${OS_SPECIFIC_LIBRARY_NAMES})
# --- end
