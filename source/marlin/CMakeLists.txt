#######################################################
# cmake file for building Arbor PFA Processor
# @author rete , IPNL
#######################################################

# find ilcsoft packages
FIND_PACKAGE( GEAR REQUIRED )
FIND_PACKAGE( LCIO REQUIRED )
FIND_PACKAGE( Marlin REQUIRED )
FIND_PACKAGE( MarlinUtil REQUIRED )
FIND_PACKAGE( REQUIRED )

FOREACH( pkg GEAR LCIO Marlin MarlinUtil )
IF( ${pkg}_FOUND )
  INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
  LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
  ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
ENDIF()
ENDFOREACH()


# include directories
INCLUDE_DIRECTORIES( include )


# build the SDHCAL arbor processor library
# ===========================================
SET (
  SDHCAL_ARBOR_PROCESSOR_SRC_FILES
  src/SDHCALArborProcessor.cc
)

ADD_SHARED_LIBRARY( SDHCALArborProcessor ${SDHCAL_ARBOR_PROCESSOR_SRC_FILES} )
TARGET_LINK_LIBRARIES( SDHCALArborProcessor ArborPFA )


# build the ArborPFA processor library
# ===========================================
SET (
  ARBOR_PFA_PROCESSOR_SRC_FILES
  src/ExternalClusteringAlgorithm.cc
  src/MCParticleCreator.cc
  src/TrackCreator.cc
  src/CaloHitCreator.cc
  src/GeometryCreator.cc
  src/PfoCreator.cc
  src/SimpleBFieldCalculator.cc
  src/ArborPFAProcessor.cc
)

ADD_SHARED_LIBRARY( ArborPFAProcessor ${ARBOR_PFA_PROCESSOR_SRC_FILES} )
TARGET_LINK_LIBRARIES( ArborPFAProcessor ArborPFA )

# install libraries
INSTALL(
  TARGETS SDHCALArborProcessor ArborPFAProcessor
  LIBRARY DESTINATION lib  
)



