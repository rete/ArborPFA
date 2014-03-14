

export MARLIN_DLL=/home/remi/git/ArborPFA/lib/libSDHCALArborProcessor.so

energy1=$1
energy2=$2
separationDistance=$3

particleType1="neutral"
particleType2="charged"

#algorithm="ArborPFA"
algorithm="PandoraPFA"

dataType="MC"
dataType="DATA"

inputCollection="HCALBarrel"
#inputCollection="SDHCAL_HIT"

#pandoraSettingsFile="/home/remi/git/ArborPFA/xml/SDHCALArborPandoraSettings.xml"
pandoraSettingsFile="/home/remi/git/ArborPFA/xml/StandardReconstructionPandoraSettings.xml"

gearFileForOverlay="/home/remi/ilcsoft/SDHCAL/MarlinBaboon/marlinXml/SDHCALgeom.xml"
gearFileForReconstruction="/home/remi/git/ArborPFA/xml/SDHCALGearFile.xml"

isSingleParticle="true"


overlayInputFile1="/home/remi/ilcsoft/SDHCAL/data/simulation/pi-/single_calorimeterhit_pi-_${energy1}GeV_new_I0.slcio"
overlayInputFile2="/home/remi/ilcsoft/SDHCAL/data/simulation/pi-/single_calorimeterhit_pi-_${energy2}GeV_new_I1.slcio"

#overlayInputFile1="/home/remi/ilcsoft/SDHCAL/data/testBeam/2012/TDHCAL_715693_cut_I0.slcio"
#overlayInputFile2="/home/remi/ilcsoft/SDHCAL/data/testBeam/2012/TDHCAL_715693_cut_I1.slcio"

specifier="${energy1}_GeV_${particleType1}_${energy2}_GeV_${particleType2}_${separationDistance}_cm_${algorithm}_${dataType}"
rootOutputFile="Separation_${specifier}.root"

if [ "$#" -ne "3" ]
then
	echo "Incorrect number of argument. Expected 3 (energy 1, energy 2, separation distance)"
	exit 1
fi


cat > Algorithm_OverlayEvent_${specifier}.cfg << EOF

[OverlayEventAlgorithm]

generatesLCTracks = false
useTrackInfo = true
separationDistance = ${separationDistance}
particleType1 = ${particleType1}
particleType2 = ${particleType2}
inputEnergy1 = ${energy1}
inputEnergy2 = ${energy2}

[TrackFinderAlgorithm]

clusterSizeLimit = 4
minimumTrackSize = 5
lookupDistanceX = 4
lookupDistanceY = 4
lookupDistanceZ = 2
maximumConnectorsAngle = 0.45
drawConnectors = false

EOF

cat > OverlayEvent_${specifier}.cfg << EOF

[general]

gearFile = ${gearFileForOverlay}
algorithmConfigFile = Algorithm_OverlayEvent_${specifier}.cfg

[input1]

slciofile = ${overlayInputFile1}
collectionName = ${inputCollection}
codingPattern = M:3,S-1:3,I:9,J:9,K-1:6

[input2]

slciofile = ${overlayInputFile2}
collectionName = ${inputCollection}
codingPattern = M:3,S-1:3,I:9,J:9,K-1:6

[output]

slcioFile = /home/remi/git/ArborPFA/output/Separation/OverlayEvent_${specifier}.slcio
collectionName = ${inputCollection}
nbOfEventsToOverlay = 100000
separationDistance = ${separationDistance}   # in number of pads
codingPattern = M:3,S-1:3,I:9,J:9,K-1:6


EOF



cat > MarlinXML_${specifier}.xml << EOF


<marlin>
<execute>
  <processor name="MyMarlinSDHCALArbor"/>
  <!-- <processor name="MyLCIOOutputProcessor"/> -->
</execute>

<global>
  <parameter name="LCIOInputFiles">
  /home/remi/git/ArborPFA/output/Separation/OverlayEvent_${specifier}.slcio
  </parameter>
  <parameter name="GearXMLFile" value="${gearFileForReconstruction}"/>
  <parameter name="MaxRecordNumber" value="1000"/>
  <parameter name="SkipNEvents" value="0"/>
  <parameter name="SupressCheck" value="false"/>
  <parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT"> MESSAGE </parameter>
  <parameter name="RandomSeed" value="1234567890" />
</global>

<processor name="MyMarlinSDHCALArbor" type="SDHCALArborProcessor">

  <parameter name="PandoraSettingsXmlFile" type="String"> ${pandoraSettingsFile} </parameter>
  
  <!-- Collection names -->
  <parameter name="HCalCaloHitCollection" type="String"> ${inputCollection} </parameter>
  <parameter name="GenericObjectTrackInfoCollection" type="String"> SDHCALTrackInfo </parameter>
  <parameter name="ClusterCollectionName" type="String"> ArborPFAClusterCollection </parameter>
  <parameter name="PFOCollectionName" type="String"> ArborPFAPfoCollection </parameter>
  
  <parameter name="AbsorberRadiationLength" type="float">0.2857</parameter>
  
  <parameter name="SDHCALEnergyFactors" type="FloatVec"> 
    1.0 
    2.0 
    3.0 
  </parameter>
  
  <parameter name="SDHCALEnergyConstants" type="FloatVec"> 
    0.0227148
    4.77839e-05
    -4.05221e-08
    0.112287
    6.24094e-06
    2.9642e-08
    0.156503
    0.00010774
    4.97735e-08
  </parameter>
  
  <parameter name="RootOuputFileName" type ="string"> ${rootOutputFile} </parameter>
  <parameter name="RootTreeName" type ="string"> PFOAnalysis </parameter>
  <parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT"> MESSAGE DEBUG </parameter>
</processor>


 <processor name="MyLCIOOutputProcessor" type="LCIOOutputProcessor">
  <!--   standard output: full reconstruction keep all collections -->
  <parameter name="LCIOOutputFile" type="string" >
  /home/remi/git/ArborPFA/output/Separation/Separation_${specifier}.slcio
  </parameter>
  <parameter name="LCIOWriteMode" type="string" value="WRITE_NEW"/>
  <!--parameter name="SplitFileSizekB" type="int" value="1992294"/-->
 </processor>

</marlin>


EOF


OverlayEvent -f OverlayEvent_${specifier}.cfg -g

if [ "$?" -eq "0" ]
then
        Marlin MarlinXML_${specifier}.xml
else
        echo "----------- OverlayEvent -f OverlayEvent_${specifier}.cfg -g failed -----------"
        exit 1
fi

rm OverlayEvent_${specifier}.cfg Algorithm_OverlayEvent_${specifier}.cfg
rm MarlinXML_${specifier}.xml
