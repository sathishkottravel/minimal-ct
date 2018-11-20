cmake_minimum_required(VERSION 3.1)


#Quartet
set(QUARTET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/qtet/quartet/src")


#--------------------------------------------------------------------
# Add header files
set(QUARTET_HEADER_FILES 
    ${QUARTET_ROOT}/predicates.cpp
    ${QUARTET_ROOT}/geometry_queries.cpp
    ${QUARTET_ROOT}/sdf.cpp
    ${QUARTET_ROOT}/trimesh.cpp
    ${QUARTET_ROOT}/tet_mesh.cpp
    ${QUARTET_ROOT}/feature.cpp
    ${QUARTET_ROOT}/read_obj.cpp
    ${QUARTET_ROOT}/tet_quality.cpp
    ${QUARTET_ROOT}/match_features.cpp
    ${QUARTET_ROOT}/optimize_tet_mesh.cpp
    ${QUARTET_ROOT}/make_signed_distance.cpp
    ${QUARTET_ROOT}/make_tet_mesh.cpp    
    ${CMAKE_CURRENT_SOURCE_DIR}/qtet/quartet.cpp    
)

#--------------------------------------------------------------------
# Add source files
set(QUARTET_SOURCE_FILES    
    ${QUARTET_ROOT}/geometry_queries.h
    ${QUARTET_ROOT}/sdf.h
    ${QUARTET_ROOT}/trimesh.h
    ${QUARTET_ROOT}/tet_mesh.h
    ${QUARTET_ROOT}/feature.h
    ${QUARTET_ROOT}/read_obj.h
    ${QUARTET_ROOT}/tet_quality.h
    ${QUARTET_ROOT}/match_features.h
    ${QUARTET_ROOT}/optimize_tet_mesh.h
    ${QUARTET_ROOT}/make_signed_distance.h
    ${QUARTET_ROOT}/make_tet_mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qtet/quartet.h   
)

add_library(quartet STATIC ${QUARTET_SOURCE_FILES} ${QUARTET_HEADER_FILES})

target_include_directories(quartet PUBLIC "${QUARTET_ROOT}")
#--------------------------------------------------------------------
# Supress warnings
ivw_suppress_compiler_warnings(quartet)

#--------------------------------------------------------------------
# Install library into target folder
ivw_default_install_comp_targets(core quartet)

#--------------------------------------------------------------------
# Creates VS folder structure
ivw_folder(quartet ext/TetMesherUtils)

source_group("CMake Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

#--------------------------------------------------------------------
# Make package (for other projects to find)
ivw_make_package(QuarTet quartet)



#Qhull
set(QHULL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/qhull)

list(APPEND CMAKE_MODULE_PATH ${QHULL_ROOT}/CMakeModules)

add_subdirectory(${QHULL_ROOT})

#--------------------------------------------------------------------
# Supress warnings
ivw_suppress_compiler_warnings(qhull)
#--------------------------------------------------------------------
# Install library into target folder
ivw_default_install_comp_targets(core qhull)
#--------------------------------------------------------------------
# Creates VS folder structure
ivw_folder(qhull ext/TopoUtil)
source_group("CMake Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
#--------------------------------------------------------------------
# Make package (for other projects to find)
ivw_make_package(QHull qhull)

#target_include_directories(${item}  PUBLIC ${QHULL_ROOT} ${QHULL_ROOT}/src)	

set(
    qhull_TARGETS_INSTALL
        ${qhull_CPP} ${qhull_STATIC} ${qhull_STATICR} ${qhull_SHAREDR}
        qhull rbox qconvex qdelaunay qvoronoi qhalf 
        ${qhull_SHARED} ${qhull_SHAREDP}  # Deprecated, use qhull_r instead
		libqhull qhull_p qhull_r qhullcpp qhullstatic qhullstatic_r testqset testqset_r qhullp
		user_eg user_eg2 user_eg3 user_egp

)

foreach(item in ${qhull_TARGETS_INSTALL})
	if(TARGET ${item})
		get_property(target_type TARGET ${item} PROPERTY TYPE)
    	if (NOT ${target_type} STREQUAL  "INTERFACE_LIBRARY")
    		#message("item: ${item}")
			ivw_folder(${item} ext/TetMesherUtils/qhull)		
			target_include_directories(${item}  PUBLIC ${QHULL_ROOT} ${QHULL_ROOT}/src)	
			ivw_suppress_compiler_warnings(${item})
		endif()
	endif()
endforeach()
