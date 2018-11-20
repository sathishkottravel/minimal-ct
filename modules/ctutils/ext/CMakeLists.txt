cmake_minimum_required(VERSION 3.1)

# libigl
option(LIBIGL_USE_STATIC_LIBRARY "Use libigl as static library" ON)
option(LIBIGL_WITH_ANTTWEAKBAR       "Use AntTweakBar"    OFF)
option(LIBIGL_WITH_CGAL              "Use CGAL"           OFF)
option(LIBIGL_WITH_COMISO            "Use CoMiso"         OFF)
option(LIBIGL_WITH_CORK              "Use Cork"           OFF)
option(LIBIGL_WITH_EMBREE            "Use Embree"         OFF)
option(LIBIGL_WITH_LIM               "Use LIM"            OFF)
option(LIBIGL_WITH_MATLAB            "Use Matlab"         OFF)
option(LIBIGL_WITH_MOSEK             "Use MOSEK"          OFF)
option(LIBIGL_WITH_NANOGUI           "Use Nanogui menu"   OFF)
option(LIBIGL_WITH_OPENGL            "Use OpenGL"         OFF)
option(LIBIGL_WITH_OPENGL_GLFW_IMGUI "Use ImGui"          OFF)
option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"           OFF)
option(LIBIGL_WITH_PNG               "Use PNG"            OFF)
option(LIBIGL_WITH_PYTHON            "Use Python"         OFF)
option(LIBIGL_WITH_TETGEN            "Use Tetgen"         OFF)
option(LIBIGL_WITH_TRIANGLE          "Use Triangle"       OFF)
option(LIBIGL_WITH_VIEWER            "Use OpenGL viewer"  OFF)
option(LIBIGL_WITH_XML               "Use XML"            OFF)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/libigl/cmake)

#set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/libigl/cmake")
set(ENV{LIBIGLROOT} libigl)

IF ( MSVC )

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libigl/libigl_patch.diff.1")
    message("IGL already patched for MSVC...")    
else()
    message("Applying Patch for IGL...")
    file(COPY libigl_patch.diff.1 DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/libigl)
    #execute_process(COMMAND cd  ${CMAKE_CURRENT_SOURCE_DIR}/libigl)
    execute_process(COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/libigl/libigl_patch.diff.1 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libigl)
endif()

ENDIF(MSVC)

add_subdirectory(libigl)

#message("MODULE PATH", ${CMAKE_MODULE_PATH})
#message("LIBIGLROOT", ${LIBIGLROOT})

#--------------------------------------------------------------------
# Supress warnings
ivw_suppress_compiler_warnings(igl)

#--------------------------------------------------------------------
# Install library into target folder
ivw_default_install_comp_targets(core igl)

#--------------------------------------------------------------------
# Creates VS folder structure
ivw_folder(igl ext/CTUtils)

source_group("CMake Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

#--------------------------------------------------------------------
# Supress warnings
ivw_suppress_compiler_warnings(igl)

source_group("CMake Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

#--------------------------------------------------------------------
# Make package (for other projects to find)
ivw_make_package(IGL igl)



#ContourTree
set(CONTOURTREE ct/ContourTree)
#--------------------------------------------------------------------
# Add header files
set(CONTOURTREE_HEADER_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/ContourTreeData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/DisjointSets.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/Grid3D.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/HyperVolume.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/MergeTree.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/Persistence.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/ScalarFunction.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/SimFunction.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/SimplifyCT.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/TopologicalFeatures.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/TriMesh.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/ContourTree.hpp
)

#--------------------------------------------------------------------
# Add source files
set(CONTOURTREE_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/ContourTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/ContourTreeData.cpp    
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/HyperVolume.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/MergeTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/Persistence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/SimplifyCT.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/TopologicalFeatures.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CONTOURTREE}/TriMesh.cpp
)

add_library(contourtree STATIC ${CONTOURTREE_SOURCE_FILES} ${CONTOURTREE_HEADER_FILES})

target_include_directories(contourtree  PUBLIC ct/ContourTree)
#--------------------------------------------------------------------
# Supress warnings
ivw_suppress_compiler_warnings(contourtree)

#--------------------------------------------------------------------
# Install library into target folder
ivw_default_install_comp_targets(core contourtree)

#--------------------------------------------------------------------
# Creates VS folder structure
ivw_folder(contourtree ext/CTUtils)

source_group("CMake Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

#--------------------------------------------------------------------
# Make package (for other projects to find)
ivw_make_package(ContourTree contourtree)



