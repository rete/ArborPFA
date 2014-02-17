#######################################################
# cmake file for building ArborPFA package
# @author rete , IPNL
#######################################################

# include directories
INCLUDE_DIRECTORIES( include )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type")

#IF( OPENMP_FOUND )
#	ADD_DEFINITIONS("-fopenmp")
#ENDIF()

FILE (
	GLOB_RECURSE
	ARBORPFA_SRC_FILES
	src/*
)

ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${ARBORPFA_SRC_FILES} )

INSTALL(
	TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib 
)

	
IF( BUILD_PROCESSOR )
  ADD_SUBDIRECTORY( marlin )
ENDIF()



