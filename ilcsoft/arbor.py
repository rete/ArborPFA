##################################################
#
# ArborPFA module
#
# Author: Jan Engels, DESY
# Date: Jan, 2007
#
##################################################

# custom imports
from baseilc import BaseILC
from util import *


class ArborPFA(BaseILC):
    """ Responsible for the ArborPFA software installation process. """
    
    def __init__(self, userInput):
        BaseILC.__init__(self, userInput, "ArborPFA", "ArborPFA")

        self.reqfiles = [["lib/libArborPFA.so"]]

        # PandoraPFA and ROOT are required for building ArborPFA
        self.reqmodules = [ "PandoraPFA", "ROOT" ]
        
        self.download.supportedTypes = [ "wget" ]

        self.envcmake['BUILD_PROCESSOR']='ON'
    
    def setMode(self, mode):
        BaseILC.setMode(self, mode)

        self.download.url = 'https://github.com/rete/ArborPFA/archive/%s.tar.gz' % self.version
            
    def compile(self):
        """ compile ArborPFA """
        
        os.chdir( self.installPath )

        os.chdir( "build" )

        if( self.rebuild ):
            tryunlink( "CMakeCache.txt" )

        # build software
        if( os.system( self.genCMakeCmd() + " 2>&1 | tee -a " + self.logfile ) != 0 ):
            self.abort( "failed to configure!!" )
        
        if( os.system( "make ${MAKEOPTS} 2>&1 | tee -a " + self.logfile ) != 0 ):
            self.abort( "failed to compile!!" )

        if( os.system( "make install 2>&1 | tee -a " + self.logfile ) != 0 ):
            self.abort( "failed to install!!" )


    def preCheckDeps(self):
        BaseILC.preCheckDeps(self)
        if( self.mode == "install" ):
            if self.cmakeBoolOptionIsSet( "BUILD_PROCESSOR" ):
                self.reqmodules += [ "Marlin", "MarlinUtil" ]
                self.reqfiles.append(["lib/libArborPFAProcessor.so", "lib/libSDHCALArborProcessor.so"])
    
    def postCheckDeps(self):
        BaseILC.postCheckDeps(self)

        self.env["ARBORPFA"] = self.installPath
        self.envpath["LD_LIBRARY_PATH"].append( '$ARBORPFA/lib' )

