

export MARLIN_DLL=/home/remi/git/ArborPFA/lib/libSDHCALArborProcessor.so

energy=$1
inputFile=$2

algorithm="ArborPFA"
#algorithm="PandoraPFA"

dataType="MC"
#dataType="DATA"

inputCollection="HCALBarrel"
#inputCollection="SDHCAL_HIT"

pandoraSettingsFile="/home/remi/git/ArborPFA/xml/SDHCALArborPandoraSettings.xml"
#pandoraSettingsFile="/home/remi/git/ArborPFA/xml/StandardReconstructionPandoraSettings.xml"

gearFileForReconstruction="/home/remi/git/ArborPFA/xml/SDHCALGearFile.xml"

specifier="${energy}_GeV_${algorithm}_${dataType}"
rootOutputFile="SingleParticle_${specifier}.root"

if [ "$#" -ne "2" ]
then
	echo "Incorrect number of argument. Expected 2 (energy, input file)"
	exit 1
fi


cat > MarlinXML_singleParticle_${specifier}.xml << EOF


<marlin>
<execute>
  <processor name="MyMarlinSDHCALArbor"/>
</execute>

<global>
  <parameter name="LCIOInputFiles">
  ${inputFile}
  </parameter>
  <parameter name="GearXMLFile" value="${gearFileForReconstruction}"/>
  <parameter name="MaxRecordNumber" value="0"/>
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
  <parameter name="IsSingleParticleStudy" type="bool"> true </parameter>
  <parameter name="SingleParticleMomentum" type="float"> ${energy} </parameter>
  <parameter name="RootOuputFileName" type ="string"> ${rootOutputFile} </parameter>
  <parameter name="RootTreeName" type ="string"> PFOAnalysis </parameter>
  <parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT"> MESSAGE DEBUG </parameter>
</processor>

</marlin>


EOF


Marlin MarlinXML_singleParticle_${specifier}.xml

rm MarlinXML_singleParticle_${specifier}.xml
