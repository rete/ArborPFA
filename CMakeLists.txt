########################################################
# cmake file for building ArobrPFA
# @author Jan Engels, John Marshall, Remi Ete
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


# project name
PROJECT( ArborPFA )


### PACKAGE VERSIONS ########################################################
SET( ${PROJECT_NAME}_VERSION_MAJOR 2 )
SET( ${PROJECT_NAME}_VERSION_MINOR 0 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

# versions of ArborPFA subpackages
# don't forget to update when releasing ArborPFA
# e.g. SET( PandoraSDK_version "v01-00-00" )
SET( PandoraSDK_version           "v02-00-01" )
SET( PandoraMonitoring_version    "v02-00-00" )
SET( ArborContent_version         "v00-00-00" )
SET( MarlinArbor_version          "master" )


### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )
INCLUDE( ExternalProject )

OPTION( PANDORA_MONITORING "Build PandoraMonitoring library (requires ROOT)" OFF )
MESSAGE( STATUS "PANDORA_MONITORING: ${PANDORA_MONITORING}" )

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

OPTION( MARLIN_ARBOR "Build MarlinArbor library (lcio and marlin)" OFF )
MESSAGE( STATUS "MARLIN_ARBOR: ${MARLIN_ARBOR}" )

# ----- download settings ----------------------------------------------------
SET( PandoraSDK_repository          "https://github.com/PandoraPFA/PandoraSDK.git" )
SET( PandoraMonitoring_repository   "https://github.com/PandoraPFA/PandoraMonitoring.git" )
SET( ArborContent_repository        "https://github.com/ArborPFA/ArborContent.git" )
SET( MarlinArbor_repository         "https://github.com/ArborPFA/MarlinArbor.git" )
# ----------------------------------------------------------------------------

# In order to pass semicolon-separated lists over the command line to all packages,
# we need to separate list elements with '%' instead of the standard cmake list separator ';'
# The list separator needs also to be redefined in the ExternalProject_Add calls by setting
# the variable LIST_SEPARATOR
FOREACH( _path ${CMAKE_PREFIX_PATH} )
    SET( CMAKE_PREFIX_PATH_FIXED ${CMAKE_PREFIX_PATH_FIXED}%${_path} )
ENDFOREACH()

FOREACH( _path ${CMAKE_MODULE_PATH} )
    SET( CMAKE_MODULE_PATH_FIXED ${CMAKE_MODULE_PATH_FIXED}%${_path} )
ENDFOREACH()

# ----- cmake arguments common to ALL cmake packages -------------------------
SET( common_cmake_args
     "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_FIXED}"
     "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH_FIXED}"
     "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
     "-DINSTALL_DOC=${INSTALL_DOC}"
)
# ----------------------------------------------------------------------------

ExternalProject_Add( PandoraSDK
    GIT_REPOSITORY ${PandoraSDK_repository}
    GIT_TAG ${PandoraSDK_version}
    CMAKE_ARGS ${common_cmake_args}
    LIST_SEPARATOR %
)

IF( PANDORA_MONITORING )
    ExternalProject_Add( PandoraMonitoring
        DEPENDS PandoraSDK
        GIT_REPOSITORY ${PandoraMonitoring_repository}
        GIT_TAG ${PandoraMonitoring_version}
        CMAKE_ARGS ${common_cmake_args}
        LIST_SEPARATOR %
    )

    ExternalProject_Add( ArborContent
        DEPENDS PandoraSDK PandoraMonitoring
        GIT_REPOSITORY ${ArborContent_repository}
        GIT_TAG ${ArborContent_version}
        CMAKE_ARGS ${common_cmake_args} -DPANDORA_MONITORING=1
        LIST_SEPARATOR %
    )
    
    IF( MARLIN_ARBOR )
        ExternalProject_Add( MarlinArbor
        DEPENDS PandoraSDK PandoraMonitoring ArborContent
        GIT_REPOSITORY ${MarlinArbor_repository}
        GIT_TAG ${MarlinArbor_version}
        CMAKE_ARGS ${common_cmake_args} -DPANDORA_MONITORING=1
        LIST_SEPARATOR %
    )
    ENDIF()
ELSE()
    ExternalProject_Add( ArborContent
        DEPENDS PandoraSDK
        GIT_REPOSITORY ${ArborContent_repository}
        GIT_TAG ${ArborContent_version}
        CMAKE_ARGS ${common_cmake_args}
        LIST_SEPARATOR %
    )
    
    IF( MARLIN_ARBOR )
        ExternalProject_Add( MarlinArbor
        DEPENDS PandoraSDK ArborContent
        GIT_REPOSITORY ${MarlinArbor_repository}
        GIT_TAG ${MarlinArbor_version}
        CMAKE_ARGS ${common_cmake_args}
        LIST_SEPARATOR %
    )
    ENDIF()
ENDIF()



# display some variables and write them to cache
DISPLAY_STD_VARIABLES()
