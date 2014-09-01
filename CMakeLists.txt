#################################################
# cmake file for building ArborPFA framework
# @author Eté Rémi ,IPNL
# Copyright (c) CNRS / IPNL
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# project name
PROJECT( ArborPFA )


SET( ${PROJECT_NAME}_VERSION_MAJOR 1 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 2 )


# --------------- ilcsoft dependencies -----------
LIST( APPEND CMAKE_MODULE_PATH $ILCSOFT )

# load default settings from ILCSOFT_CMAKE_MODULES
FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )
INCLUDE( ilcsoft_default_settings )

# -------------- packages -------------
FIND_PACKAGE( PandoraSDK REQUIRED )
FIND_PACKAGE( FineGranularityContent REQUIRED )
FIND_PACKAGE( PandoraMonitoring REQUIRED )
FIND_PACKAGE( LCIO REQUIRED )
FIND_PACKAGE( ROOT 5.26.00 REQUIRED COMPONENTS Core Eve Geom RGL EG ) # minimum required ROOT version

FOREACH( pkg PandoraSDK FineGranularityContent PandoraMonitoring ROOT LCIO)
  IF( ${pkg}_FOUND )
    INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
    LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
    ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
  ENDIF()
ENDFOREACH()


SET( ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS ${PandoraSDK_INCLUDE_DIRS} ${FineGranularityContent_INCLUDE_DIRS} ${PandoraMonitoring_INCLUDE_DIRS} )
SET( ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS ${PandoraSDK_LIBRARY_DIRS} ${FineGranularityContent_LIBRARY_DIRS} ${PandoraMonitoring_LIBRARY_DIRS} )
SET( ${PROJECT_NAME}_DEPENDS_LIBRARIES    ${PandoraSDK_LIBRARIES}    ${FineGranularityContent_LIBRARIES}    ${PandoraMonitoring_LIBRARIES}    )


OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" ON )

IF( INSTALL_DOC )

    FIND_PACKAGE( Doxygen )

    IF( DOXYGEN_FOUND )
        ADD_SUBDIRECTORY( ./doc )
    ELSE()
        MESSAGE( SEND_ERROR "Could not find doxygen required to build documentation" )
        MESSAGE( "Please install doxygen or set INSTALL_DOC to OFF" )
        MESSAGE( "" )
    ENDIF()

ENDIF()


# -------- generate ArborPFAConfig.h --------------------------------------------
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.h.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.h" @ONLY )

# -------- generate FindArborPFA.cmake --------------------------------------------
#CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/Find${PROJECT_NAME}.cmake.in"
#    "${PROJECT_SOURCE_DIR}/Find${PROJECT_NAME}.cmake" @ONLY )

INSTALL( FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.h" DESTINATION "source/include/arborpfa" )


OPTION( BUILD_PROCESSOR "Set to OFF to skip build/install ArborPFA processor (Marlin)" ON )

ADD_SUBDIRECTORY( ./source )


# display some variables and write them to cache
DISPLAY_STD_VARIABLES()


# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )

