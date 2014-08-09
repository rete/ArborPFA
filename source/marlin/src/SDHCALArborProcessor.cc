/**
 *  @file   marlin/src/SDHCALArborProcessor.cc
 * 
 *  @brief  Implementation of the arbor pfa processor class for sdhcal.
 * 
 *  $Log: $
 */

// marlin
#include "marlin/Global.h"
#include "marlin/Exceptions.h"

// gear
#include "gear/BField.h"
#include "gear/CalorimeterParameters.h"
#include "gear/GEAR.h"
#include "gear/GearParameters.h"
#include "gear/CalorimeterParameters.h"
#include "gear/TPCParameters.h"
#include "gear/PadRowLayout2D.h"
#include "gear/LayerLayout.h"

// pandora sdk
#include "Api/PandoraApi.h"
#include "Objects/CartesianVector.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"
#include "Helpers/XmlHelper.h"

// pandora content
#include "FineGranularityContent.h"

// pandora moitoring
#include "TTreeWrapper.h"

#include "SDHCALArborProcessor.h"

// arbor
#include "arborpfa/arbor/Arbor.h"
#include "arborpfa/api/ArborApi.h"
#include "arborpfa/content/ArborHelper.h"
#include "arborpfa/content/CaloHitHelper.h"
#include "arborpfa/content/ArborPluginManager.h"

// lcio
#include "UTIL/CellIDDecoder.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/LCGenericObject.h"
#include "IMPL/ClusterImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "IMPL/LCRelationImpl.h"
#include "IMPL/ReconstructedParticleImpl.h"

// marlin utils
#include "CalorimeterHitType.h"
#include "ClusterShapes.h"

// std
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <limits>

// root
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"

//#ifdef __MAKECINT__
#pragma link C++ class std::vector<float> +;
//#endif

SDHCALArborProcessor sdhcalArborProcessor;

pandora::Pandora    *SDHCALArborProcessor::m_pPandora   = NULL;
arbor::Arbor        *SDHCALArborProcessor::m_pArbor     = NULL;
EVENT::LCEvent      *SDHCALArborProcessor::m_pLcioEvent = NULL;
SDHCALArborProcessor::FloatVector          SDHCALArborProcessor::m_sdhcalEnergyFactors = SDHCALArborProcessor::FloatVector();
SDHCALArborProcessor::FloatVector          SDHCALArborProcessor::m_sdhcalEnergyConstants = SDHCALArborProcessor::FloatVector();

float               SDHCALBFieldCalculator::m_BField = 0.0001f;
float               SDHCALPseudoLayerCalculator::m_hCalEndCapInnerZ = 0.f;
float               SDHCALPseudoLayerCalculator::m_hCalEndCapLayerThickness = 0.f;

pandora::PseudoLayer SDHCALPseudoLayerCalculator::GetPseudoLayer(const pandora::CartesianVector &positionVector) const
{
	return pandora::PseudoLayer( round((positionVector.GetZ() - m_hCalEndCapInnerZ - 22.0 ) / m_hCalEndCapLayerThickness)) + 1;
}

pandora::PseudoLayer SDHCALPseudoLayerCalculator::GetPseudoLayerAtIp() const
{
	return this->GetPseudoLayer(pandora::CartesianVector(0,0,0));
}

//------------------------------------------------------------------------------------------------------------------------------------------

SDHCALArborProcessor::SDHCALArborProcessor() :
    Processor("SDHCALArborProcessor"),
    m_nRun(0),
    m_nEvent(0)
{
    _description = "Arbor reconstructs clusters and particle flow objects in sdhcal";
    this->ProcessSteeringFile();

}

//------------------------------------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::init()
{
    try
    {
        streamlog_out(MESSAGE) << "SDHCALArborProcessor - Init" << std::endl;
        this->FinaliseSteeringParameters();

        m_pArbor = new arbor::Arbor();
        m_pPandora = m_pArbor->GetPandora();

        PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->RegisterUserComponents());
        PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CreateGeometry());
        PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::ReadSettings(*m_pPandora, m_settings.m_pandoraSettingsXmlFile));
    }
    catch (pandora::StatusCodeException &statusCodeException)
    {
        streamlog_out(ERROR) << "Failed to initialize marlin pandora: " << statusCodeException.ToString() << std::endl;
        throw statusCodeException;
    }
    catch (gear::Exception &exception)
    {
        streamlog_out(ERROR) << "Failed to initialize marlin pandora: gear exception " << exception.what() << std::endl;
        throw exception;
    }
    catch (...)
    {
        streamlog_out(ERROR) << "Failed to initialize marlin pandora: unrecognized exception" << std::endl;
        throw;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::processRunHeader(LCRunHeader *pLCRunHeader)
{
    m_detectorName = pLCRunHeader->getDetectorName();
    streamlog_out(DEBUG5) << "Detector Name " << m_detectorName << ", Run " << ++m_nRun <<  std::endl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::processEvent(LCEvent *pLCEvent)
{
    static int eventCounter = 0;
    m_pLcioEvent = pLCEvent;

    if (eventCounter < m_settings.m_nEventsToSkip)
    {
        ++eventCounter;
        throw marlin::SkipEventException(this);
    }

    try
    {
        streamlog_out(MESSAGE) << "SDHCALArborProcessor, Run " << pLCEvent->getRunNumber() << ", Event " << pLCEvent->getEventNumber() << std::endl;
        ++m_nEvent;

        PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CreateCaloHits(pLCEvent));
        PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CreateTracks(pLCEvent));
        PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, arbor::ArborApi::PrepareEvent(*m_pArbor));
        PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::ProcessEvent(*m_pPandora));
        PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, arbor::ArborApi::Reset(*m_pArbor));

        PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::Reset(*m_pPandora));
        this->Reset();
    }
    catch (pandora::StatusCodeException &statusCodeException)
    {
        streamlog_out(ERROR) << "Marlin pandora failed to process event: " << statusCodeException.ToString() << std::endl;
        throw statusCodeException;
    }
    catch (gear::Exception &exception)
    {
        streamlog_out(ERROR) << "Marlin pandora failed to process event: gear exception " << exception.what() << std::endl;
        throw exception;
    }
    catch (EVENT::Exception &exception)
    {
        streamlog_out(ERROR) << "Marlin pandora failed to process event: lcio exception " << exception.what() << std::endl;
        throw exception;
    }
    catch (...)
    {
        streamlog_out(ERROR) << "Marlin pandora failed to process event: unrecognized exception" << std::endl;
        throw;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::check(LCEvent */*pLCEvent*/)
{
    streamlog_out(DEBUG) << "SDHCALArborProcessor - Check" << std::endl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::end()
{
 streamlog_out(MESSAGE) << "SDHCALArborProcessor - End" << std::endl;
 delete m_pArbor;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode SDHCALArborProcessor::RegisterUserComponents() const
{
    // Register content from external pandora libraries
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::SetPseudoLayerCalculator(*m_pPandora,
        new SDHCALPseudoLayerCalculator()));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::SetShowerProfileCalculator(*m_pPandora,
        new FineGranularityShowerProfileCalculator()));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, FineGranularityContent::RegisterAlgorithms(*m_pPandora));
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, FineGranularityContent::RegisterHelperFunctions(*m_pPandora));

    // Register local content
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::SetBFieldCalculator(*m_pPandora,
        new SDHCALBFieldCalculator()));

    // register arbor algorithms and plugins
				PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, arbor::ArborApi::RegisterArborAlgorithms(*m_pArbor));
				PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, arbor::ArborApi::RegisterArborPlugins(*m_pArbor));

				// energy correction for sdhcal
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterEnergyCorrectionFunction(*m_pPandora,
        "SDHCALEnergyCorrection", pandora::HADRONIC, &SDHCALArborProcessor::SDHCALEnergyCorrectionFunction));

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::ProcessSteeringFile()
{
    registerProcessorParameter("PandoraSettingsXmlFile",
                            "The pandora settings xml file",
                            m_settings.m_pandoraSettingsXmlFile,
                            std::string());

    // Input collections
    registerInputCollection(LCIO::CALORIMETERHIT,
                            "HCalCaloHitCollection",
                            "Name of the HCAL calo hit collection",
                            m_hCalCaloHitCollection,
                            std::string(""));

    registerInputCollection(LCIO::LCGENERICOBJECT,
                            "GenericObjectTrackInfoCollection",
                            "Name of the generic object collection for sdhcal track info",
                            m_genericObjectTrackInfoCollection,
                            std::string(""));

    // Absorber properties
    registerProcessorParameter("AbsorberRadiationLength",
                            "The absorber radation length",
                            m_absorberRadiationLength,
                            float(1.));

    registerProcessorParameter("AbsorberInteractionLength",
                            "The absorber interaction length",
                            m_absorberInteractionLength,
                            float(1.));

    // Name of PFO collection written by MarlinPandora
    registerOutputCollection( LCIO::CLUSTER,
                              "ClusterCollectionName",
                              "Cluster Collection Name",
                              m_clusterCollectionName,
                              std::string("ArborPFAClusterCollection"));

    registerOutputCollection( LCIO::RECONSTRUCTEDPARTICLE,
                              "PFOCollectionName",
                              "PFO Collection Name",
                              m_pfoCollectionName,
																													std::string("ArborPFAPfoCollection"));

				registerProcessorParameter("SDHCALBField",
																												"The bfield in the sdhcal prototype, units Tesla",
																												SDHCALBFieldCalculator::m_BField,
																												float(0.01f));

				registerProcessorParameter("SDHCALEnergyFactors",
																												"The SDHCAL energy factors for threshold 1, 2 and 3",
																												m_sdhcalEnergyFactors,
																												FloatVector());

				registerProcessorParameter("SDHCALEnergyConstants",
																												"The SDHCAL energy constant for computation alphas(1,2,3), betas(1,2,3) and gammas(1,2,3) ",
																												m_sdhcalEnergyConstants,
																												FloatVector());

    // Number of events to skip
    registerProcessorParameter("NEventsToSkip",
                            "Number of events to skip at start of reconstruction job",
                            m_settings.m_nEventsToSkip,
                            int(0));

    registerProcessorParameter("IsSingleParticleStudy",
                            "",
                            m_isSingleParticleStudy,
                            false);

    registerProcessorParameter("SingleParticleMomentum",
                            "",
                            m_singleParticleMomentum,
                            static_cast<float>(0.f) );

}

//------------------------------------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::FinaliseSteeringParameters()
{
	   const gear::CalorimeterParameters &hCalEndcapParameters = marlin::Global::GEAR->getHcalEndcapParameters();
	   const gear::LayerLayout &layerLayout = hCalEndcapParameters.getLayerLayout();
				m_NCells0 = hCalEndcapParameters.getIntVal("NCells0");
				m_NCells1 = hCalEndcapParameters.getIntVal("NCells1");

				m_hCalEndCapInnerR = hCalEndcapParameters.getExtent()[0];
				m_hCalEndCapInnerZ = hCalEndcapParameters.getExtent()[2];
				m_hCalEndCapLayerThickness = layerLayout.getThickness(0);

				SDHCALPseudoLayerCalculator::m_hCalEndCapInnerZ = m_hCalEndCapInnerZ;
				SDHCALPseudoLayerCalculator::m_hCalEndCapLayerThickness = m_hCalEndCapLayerThickness;

				if(3 != m_sdhcalEnergyFactors.size() || 9 != m_sdhcalEnergyConstants.size())
					throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode SDHCALArborProcessor::CreateGeometry() const
{

 try
 {
     PandoraApi::Geometry::Parameters geometryParameters;

     const gear::CalorimeterParameters &hCalEndcapParameters = marlin::Global::GEAR->getHcalEndcapParameters();

     const gear::LayerLayout &layerLayout = hCalEndcapParameters.getLayerLayout();

     geometryParameters.m_hCalEndCapParameters.m_innerRCoordinate    = hCalEndcapParameters.getExtent()[0];
     geometryParameters.m_hCalEndCapParameters.m_innerZCoordinate    = hCalEndcapParameters.getExtent()[2];
     geometryParameters.m_hCalEndCapParameters.m_innerPhiCoordinate  = hCalEndcapParameters.getPhi0();
     geometryParameters.m_hCalEndCapParameters.m_innerSymmetryOrder  = hCalEndcapParameters.getSymmetryOrder();
     geometryParameters.m_hCalEndCapParameters.m_outerRCoordinate    = hCalEndcapParameters.getExtent()[1];
     geometryParameters.m_hCalEndCapParameters.m_outerZCoordinate    = hCalEndcapParameters.getExtent()[3];
     geometryParameters.m_hCalEndCapParameters.m_outerPhiCoordinate  = hCalEndcapParameters.getPhi0();
     geometryParameters.m_hCalEndCapParameters.m_outerSymmetryOrder  = hCalEndcapParameters.getSymmetryOrder();
     geometryParameters.m_hCalEndCapParameters.m_isMirroredInZ       = false;
     geometryParameters.m_hCalEndCapParameters.m_nLayers             = layerLayout.getNLayers();

     for (int i = 0; i < layerLayout.getNLayers(); ++i)
     {
         PandoraApi::Geometry::Parameters::LayerParameters layerParameters;
         layerParameters.m_closestDistanceToIp   = layerLayout.getDistance(i) + (0.5 * (layerLayout.getThickness(i) + layerLayout.getAbsorberThickness(i)));
         layerParameters.m_nRadiationLengths     = m_absorberRadiationLength * layerLayout.getAbsorberThickness(i);
         layerParameters.m_nInteractionLengths   = m_absorberInteractionLength * layerLayout.getAbsorberThickness(i);
         geometryParameters.m_hCalEndCapParameters.m_layerParametersList.push_back(layerParameters);
     }

     PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::Create(*m_pPandora, geometryParameters));
 }
 catch (gear::Exception &exception)
 {
     streamlog_out(ERROR) << "Failure in marlin pandora geometry creator, gear exception: " << exception.what() << std::endl;
     throw exception;
 }

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode SDHCALArborProcessor::CreateCaloHits(EVENT::LCEvent *pLCEvent)
{

		try
		{
						const EVENT::LCCollection *pCaloHitCollection = pLCEvent->getCollection(m_hCalCaloHitCollection);
						const int nElements(pCaloHitCollection->getNumberOfElements());

						if (0 == nElements)
										return pandora::STATUS_CODE_FAILURE;

						const gear::LayerLayout &endcapLayerLayout(marlin::Global::GEAR->getHcalEndcapParameters().getLayerLayout());

						UTIL::CellIDDecoder<CalorimeterHit> cellIdDecoder(pCaloHitCollection);
						const std::string layerCodingString(pCaloHitCollection->getParameters().getStringVal(LCIO::CellIDEncoding));
						const std::string layerCoding((layerCodingString.find("K-1") == std::string::npos) ? "K" : "K-1");
						const std::string cellIDCodingI("I");
						const std::string cellIDCodingJ("J");

						for (int i = 0; i < nElements; ++i)
						{
										try
										{
														EVENT::CalorimeterHit *pCaloHit = dynamic_cast<CalorimeterHit*>(pCaloHitCollection->getElementAt(i));

														if (NULL == pCaloHit)
																		throw EVENT::Exception("Collection type mismatch");

														PandoraApi::CaloHit::Parameters caloHitParameters;
														caloHitParameters.m_hitType = pandora::HCAL;
														caloHitParameters.m_isDigital = false;
														caloHitParameters.m_layer = cellIdDecoder(pCaloHit)[layerCoding.c_str()];
														caloHitParameters.m_isInOuterSamplingLayer = (caloHitParameters.m_layer.Get() <= 2 || caloHitParameters.m_layer.Get() >= endcapLayerLayout.getNLayers()-3);//(this->GetNLayersFromEdge(pCaloHit) <= m_settings.m_nOuterSamplingLayers);

										    caloHitParameters.m_pParentAddress = (void*)pCaloHit;
										    caloHitParameters.m_inputEnergy = pCaloHit->getEnergy();
										    caloHitParameters.m_time = pCaloHit->getTime();

										    unsigned int I = cellIdDecoder(pCaloHit)[cellIDCodingI.c_str()];
										    unsigned int J = cellIdDecoder(pCaloHit)[cellIDCodingJ.c_str()];
										    unsigned int K = caloHitParameters.m_layer.Get();

										    float cellSize0 = endcapLayerLayout.getCellSize0(K);
										    float cellSize1 = endcapLayerLayout.getCellSize1(K);

										   	float x = - m_NCells0 / 2.0 * cellSize0 + I * cellSize0;
										   	float y = - m_NCells1 / 2.0 * cellSize1 + J * cellSize1;
										   	float z = m_hCalEndCapInnerZ + 22.0 + K * m_hCalEndCapLayerThickness;

										    const float *pCaloHitPosition(pCaloHit->getPosition());
										    const pandora::CartesianVector positionVector(x, y, z);

										   	caloHitParameters.m_positionVector = positionVector;
										   	caloHitParameters.m_expectedDirection = positionVector.GetUnitVector();

														float absorberCorrection(1.);
														caloHitParameters.m_detectorRegion = pandora::ENDCAP;

										    caloHitParameters.m_cellSizeU = endcapLayerLayout.getCellSize0(K);
										    caloHitParameters.m_cellSizeV = endcapLayerLayout.getCellSize1(K);
										    caloHitParameters.m_cellThickness = endcapLayerLayout.getThickness(K);

										    const float layerAbsorberThickness(endcapLayerLayout.getAbsorberThickness(K));
										    caloHitParameters.m_nCellRadiationLengths = m_absorberRadiationLength * layerAbsorberThickness;
										    caloHitParameters.m_nCellInteractionLengths = m_absorberInteractionLength * layerAbsorberThickness;
										    caloHitParameters.m_cellNormalVector = pandora::CartesianVector(0, 0, 1);

										    for (unsigned int i = 0, iMax = endcapLayerLayout.getNLayers(); i < iMax; ++i)
										    {
										        const float absorberThickness(endcapLayerLayout.getAbsorberThickness(i));

										        if (absorberThickness <= 0.)
										            continue;

										        if (layerAbsorberThickness > 0.)
										            absorberCorrection = absorberThickness / layerAbsorberThickness;

										        break;
										    }

										    if(pCaloHit->getEnergy() - 1.f < std::numeric_limits<float>::epsilon())
										     caloHitParameters.m_inputEnergy = 0.0406459;
										    else if(pCaloHit->getEnergy() - 2.f < std::numeric_limits<float>::epsilon())
										    	caloHitParameters.m_inputEnergy = 0.101952;
										    else if(pCaloHit->getEnergy() - 3.f < std::numeric_limits<float>::epsilon())
										    	caloHitParameters.m_inputEnergy = 0.258078;
										    else
										    	throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);

														caloHitParameters.m_mipEquivalentEnergy = 0.0406459;
														caloHitParameters.m_hadronicEnergy = caloHitParameters.m_inputEnergy.Get();
														caloHitParameters.m_electromagneticEnergy = caloHitParameters.m_inputEnergy.Get();

														if(pCaloHit->getType() == 1 || pCaloHit->getType() == 3)
														{
															m_caloHitTypeParticle1.push_back(pCaloHit);
														}
														else if(pCaloHit->getType() == 2 || pCaloHit->getType() == 3)
														{
															m_caloHitTypeParticle2.push_back(pCaloHit);
														}

														m_caloHitVec.push_back(pCaloHit);

														PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::CaloHit::Create(*m_pPandora, caloHitParameters));
										}
										catch (pandora::StatusCodeException &statusCodeException)
										{
														streamlog_out(ERROR) << "Failed to extract hcal calo hit: " << statusCodeException.ToString() << std::endl;
										}
										catch (EVENT::Exception &exception)
										{
														streamlog_out(WARNING) << "Failed to extract hcal calo hit: " << exception.what() << std::endl;
										}
						}
		}
		catch (EVENT::Exception &exception)
		{
						streamlog_out(MESSAGE) << "Failed to extract hcal calo hit collection: " << m_hCalCaloHitCollection << ", " << exception.what() << std::endl;
		}


	 return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode SDHCALArborProcessor::CreateTracks(EVENT::LCEvent *pLCEvent) const
{

	if(m_isSingleParticleStudy)
	{

		float enteringPointX = 0.f;
		float enteringPointY = 0.f;
		PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->FindShowerEnteringPoint(enteringPointX, enteringPointY));

		try
		{
   float enteringPointZ = m_hCalEndCapInnerZ;

   pandora::CartesianVector trackPositionAtEnd(enteringPointX, enteringPointY, enteringPointZ);

   pandora::CartesianVector trackMomemtumAtEnd(0.0001, 0.0001, m_singleParticleMomentum);
   float trackMomentum = trackMomemtumAtEnd.GetMagnitude();

   PandoraApi::Track::Parameters trackParameters;

   trackParameters.m_pParentAddress = 0;
   trackParameters.m_particleId = pandora::PI_MINUS;
   trackParameters.m_mass = pandora::PdgTable::GetParticleMass(pandora::PI_PLUS);
   trackParameters.m_charge = -1;
   trackParameters.m_z0 = 0.0;  // fake z0 must far from the entry point (in mm)
   trackParameters.m_d0 = std::fabs(trackParameters.m_z0.Get()) * std::sqrt(trackPositionAtEnd.GetX()*trackPositionAtEnd.GetX()+trackPositionAtEnd.GetY()*trackPositionAtEnd.GetY()) / (std::fabs(trackParameters.m_z0.Get()) + m_hCalEndCapInnerZ);
   trackParameters.m_momentumAtDca = trackMomemtumAtEnd;

   // track states
   trackParameters.m_trackStateAtStart = pandora::TrackState(pandora::CartesianVector(0., 0., trackParameters.m_z0.Get()), trackMomemtumAtEnd);
   trackParameters.m_trackStateAtEnd = pandora::TrackState(trackPositionAtEnd, trackMomemtumAtEnd);
   trackParameters.m_trackStateAtCalorimeter = trackParameters.m_trackStateAtEnd.Get();

   std::cout << "track state at calo (processor) : " << trackParameters.m_trackStateAtCalorimeter.Get() << std::endl;

   trackParameters.m_timeAtCalorimeter = 0.f;     // don't care ...
   trackParameters.m_isProjectedToEndCap = true; // SDHCAL is a fake end cap
   trackParameters.m_reachesCalorimeter = true;  // No ecal here, the track reaches the Hcal end cap

   trackParameters.m_canFormPfo = true;              // force it to produce a pfo
   trackParameters.m_canFormClusterlessPfo = false;  // force it to produce a pfo, so that there must be a an associated cluster

   PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::Track::Create(*m_pPandora, trackParameters));

		}
  catch (pandora::StatusCodeException &statusCodeException)
  {
      streamlog_out(ERROR) << "Failed to extract a track: " << statusCodeException.ToString() << std::endl;
  }
  catch (EVENT::Exception &exception)
  {
      streamlog_out(WARNING) << "Failed to extract a vertex: " << exception.what() << std::endl;
  }

		return pandora::STATUS_CODE_SUCCESS;
	}

	try
	{

		const EVENT::LCCollection *pTrackInfoCollection = pLCEvent->getCollection(m_genericObjectTrackInfoCollection);
		const int nElements(pTrackInfoCollection->getNumberOfElements());

		if(0 == nElements)
			return pandora::STATUS_CODE_FAILURE;

		const gear::LayerLayout &endcapLayerLayout(marlin::Global::GEAR->getHcalEndcapParameters().getLayerLayout());
		float cellSize0 = endcapLayerLayout.getCellSize0(0);
		float cellSize1 = endcapLayerLayout.getCellSize1(0);

		for(int i = 0; i < nElements; ++i)
		{

			try
			{

				EVENT::LCGenericObject *pTrackInfoGO = dynamic_cast<LCGenericObject*>(pTrackInfoCollection->getElementAt(i));

    if (NULL == pTrackInfoGO)
        throw EVENT::Exception("Collection type mismatch");

    // grab the track parameters from generic object
    unsigned int entryPointCell0 = static_cast<unsigned int>(pTrackInfoGO->getFloatVal(0));
    unsigned int entryPointCell1 = static_cast<unsigned int>(pTrackInfoGO->getFloatVal(1));

    float trackAtEndX = - m_NCells0 / 2.0 * cellSize0 + entryPointCell0 * cellSize0;
    float trackAtEndY = - m_NCells1 / 2.0 * cellSize1 + entryPointCell1 * cellSize1;
    float trackAtEndZ = m_hCalEndCapInnerZ;

    float momentumX = pTrackInfoGO->getFloatVal(3);
    float momentumY = pTrackInfoGO->getFloatVal(4);
    float momentumZ = pTrackInfoGO->getFloatVal(5);

    pandora::CartesianVector trackPositionAtEnd(trackAtEndX, trackAtEndY, trackAtEndZ);
    pandora::CartesianVector trackMomemtumAtEnd(0.0001, 0.0001, momentumZ);
    float trackMomentum = trackMomemtumAtEnd.GetMagnitude();

    PandoraApi::Track::Parameters trackParameters;

    trackParameters.m_pParentAddress = (void *)pTrackInfoGO;
    trackParameters.m_particleId = pandora::PI_MINUS;
    trackParameters.m_mass = pandora::PdgTable::GetParticleMass(pandora::PI_PLUS);
    trackParameters.m_charge = -1;
    trackParameters.m_z0 = 0.0;  // fake z0 must far from the entry point (in mm)
    trackParameters.m_d0 = std::fabs(trackParameters.m_z0.Get()) * std::sqrt(trackPositionAtEnd.GetX()*trackPositionAtEnd.GetX()+trackPositionAtEnd.GetY()*trackPositionAtEnd.GetY()) / (std::fabs(trackParameters.m_z0.Get()) + m_hCalEndCapInnerZ);
    trackParameters.m_momentumAtDca = trackMomemtumAtEnd;

    // track states
    trackParameters.m_trackStateAtStart = pandora::TrackState(pandora::CartesianVector(0., 0., trackParameters.m_z0.Get()), trackMomemtumAtEnd);
    trackParameters.m_trackStateAtEnd = pandora::TrackState(trackPositionAtEnd, trackMomemtumAtEnd);
    trackParameters.m_trackStateAtCalorimeter = trackParameters.m_trackStateAtEnd.Get();

    trackParameters.m_timeAtCalorimeter = 0.f;     // don't care ...
    trackParameters.m_isProjectedToEndCap = true; // SDHCAL is a fake end cap
    trackParameters.m_reachesCalorimeter = true;  // No ecal here, the track reaches the Hcal end cap

    trackParameters.m_canFormPfo = true;              // force it to produce a pfo
    trackParameters.m_canFormClusterlessPfo = false;  // force it to produce a pfo, so that there must be a an associated cluster

    PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::Track::Create(*m_pPandora, trackParameters));

			}
   catch (pandora::StatusCodeException &statusCodeException)
   {
       streamlog_out(ERROR) << "Failed to extract a track: " << statusCodeException.ToString() << std::endl;
   }
   catch (EVENT::Exception &exception)
   {
       streamlog_out(WARNING) << "Failed to extract a vertex: " << exception.what() << std::endl;
   }

		}

	}
	catch (EVENT::Exception &exception)
	{
					streamlog_out(MESSAGE) << "Failed to extract tracks: " << m_hCalCaloHitCollection << ", " << exception.what() << std::endl;
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode SDHCALArborProcessor::CreateParticleFlowObjects(EVENT::LCEvent *pLCEvent)
{

 const pandora::PfoList *pPfoList = NULL;
 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::GetCurrentPfoList(*m_pPandora, pPfoList));

 IMPL::LCCollectionVec *pClusterCollection = new IMPL::LCCollectionVec(LCIO::CLUSTER);
 IMPL::LCCollectionVec *pReconstructedParticleCollection = new IMPL::LCCollectionVec(LCIO::RECONSTRUCTEDPARTICLE);

 IMPL::LCFlagImpl lcFlagImpl(pClusterCollection->getFlag());
 lcFlagImpl.setBit(LCIO::CLBIT_HITS);
 pClusterCollection->setFlag(lcFlagImpl.getFlag());

 std::vector<std::string> subDetectorNames ;
 subDetectorNames.push_back("ecal") ; const unsigned int ecal_Index(0) ;
 subDetectorNames.push_back("hcal") ; const unsigned int hcal_Index(1) ;
 subDetectorNames.push_back("yoke") ; const unsigned int yoke_Index(2) ;
 subDetectorNames.push_back("lcal") ; const unsigned int lcal_Index(3) ;
 subDetectorNames.push_back("lhcal"); const unsigned int lhcal_Index(4);
 subDetectorNames.push_back("bcal") ; const unsigned int bcal_Index(5) ;

 pClusterCollection->parameters().setValues("ClusterSubdetectorNames", subDetectorNames);

 // Create lcio "reconstructed particles" from the pandora "particle flow objects"
 for (pandora::PfoList::const_iterator itPFO = pPfoList->begin(), itPFOEnd = pPfoList->end(); itPFO != itPFOEnd; ++itPFO)
 {
     IMPL::ReconstructedParticleImpl *pReconstructedParticle = new ReconstructedParticleImpl();

     const pandora::ClusterAddressList clusterAddressList((*itPFO)->GetClusterAddressList());
     const pandora::TrackAddressList trackAddressList((*itPFO)->GetTrackAddressList());

     // Create lcio clusters
     for (pandora::ClusterAddressList::const_iterator itCluster = clusterAddressList.begin(), itClusterEnd = clusterAddressList.end();
         itCluster != itClusterEnd; ++itCluster)
     {
         IMPL::ClusterImpl *pCluster = new ClusterImpl();

         const unsigned int nHitsInCluster((*itCluster).size());

         float clusterEnergy(0.);
         float *pHitE = new float[nHitsInCluster];
         float *pHitX = new float[nHitsInCluster];
         float *pHitY = new float[nHitsInCluster];
         float *pHitZ = new float[nHitsInCluster];

         for (unsigned int iHit = 0; iHit < nHitsInCluster; ++iHit)
         {
             EVENT::CalorimeterHit *pCalorimeterHit = (CalorimeterHit*)((*itCluster)[iHit]);
             pCluster->addHit(pCalorimeterHit, 1.0);

             const float caloHitEnergy(pCalorimeterHit->getEnergy());
             clusterEnergy += caloHitEnergy;

             pHitE[iHit] = caloHitEnergy;
             pHitX[iHit] = pCalorimeterHit->getPosition()[0];
             pHitY[iHit] = pCalorimeterHit->getPosition()[1];
             pHitZ[iHit] = pCalorimeterHit->getPosition()[2];

             std::vector<float> &subDetectorEnergies = pCluster->subdetectorEnergies();
             subDetectorEnergies.resize(subDetectorNames.size());

             switch (CHT(pCalorimeterHit->getType()).caloID())
             {
                 case CHT::ecal:  subDetectorEnergies[ecal_Index ] += caloHitEnergy; break;
                 case CHT::hcal:  subDetectorEnergies[hcal_Index ] += caloHitEnergy; break;
                 case CHT::yoke:  subDetectorEnergies[yoke_Index ] += caloHitEnergy; break;
                 case CHT::lcal:  subDetectorEnergies[lcal_Index ] += caloHitEnergy; break;
                 case CHT::lhcal: subDetectorEnergies[lhcal_Index] += caloHitEnergy; break;
                 case CHT::bcal:  subDetectorEnergies[bcal_Index ] += caloHitEnergy; break;
                 default: streamlog_out(DEBUG) << " no subdetector found for hit with type: " << pCalorimeterHit->getType() << std::endl;
             }
         }

         pCluster->setEnergy(clusterEnergy);

         ClusterShapes *pClusterShapes = new ClusterShapes(nHitsInCluster, pHitE, pHitX, pHitY, pHitZ);
         pCluster->setPosition(pClusterShapes->getCentreOfGravity());
         pCluster->setIPhi(std::atan2(pClusterShapes->getEigenVecInertia()[1], pClusterShapes->getEigenVecInertia()[0]));
         pCluster->setITheta(std::acos(pClusterShapes->getEigenVecInertia()[2]));

         pClusterCollection->addElement(pCluster);
         pReconstructedParticle->addCluster(pCluster);

         delete pClusterShapes;
         delete[] pHitE; delete[] pHitX; delete[] pHitY; delete[] pHitZ;
     }

     // Add tracks to the lcio reconstructed particles
     for (pandora::TrackAddressList::const_iterator itTrack = trackAddressList.begin(), itTrackEnd = trackAddressList.end();
         itTrack != itTrackEnd; ++itTrack)
     {
         pReconstructedParticle->addTrack((Track*)(*itTrack));
     }

     float momentum[3] = {(*itPFO)->GetMomentum().GetX(), (*itPFO)->GetMomentum().GetY(), (*itPFO)->GetMomentum().GetZ()};
     pReconstructedParticle->setMomentum(momentum);
     pReconstructedParticle->setEnergy((*itPFO)->GetEnergy());
     pReconstructedParticle->setMass((*itPFO)->GetMass());
     pReconstructedParticle->setCharge((*itPFO)->GetCharge());
     pReconstructedParticle->setType((*itPFO)->GetParticleId());

     pReconstructedParticleCollection->addElement(pReconstructedParticle);
 }

 pLCEvent->addCollection(pClusterCollection, m_clusterCollectionName.c_str());
 pLCEvent->addCollection(pReconstructedParticleCollection, m_pfoCollectionName.c_str());

	return pandora::STATUS_CODE_SUCCESS;
}


pandora::StatusCode SDHCALArborProcessor::FindShowerEnteringPoint(float &enteringPointX, float &enteringPointY) const
{
	const gear::LayerLayout &endcapLayerLayout(marlin::Global::GEAR->getHcalEndcapParameters().getLayerLayout());

	UTIL::CellIDDecoder<EVENT::CalorimeterHit> cellIdDecoder("M:3,S-1:3,I:9,J:9,K-1:6");
	const std::string layerCoding("K-1");
	float cellSize0 = endcapLayerLayout.getCellSize0(0);
	float cellSize1 = endcapLayerLayout.getCellSize1(0);
	float xyCut = 4.0;
	unsigned int layerCut = 4;
	float barycenterX = 0.f;
	float barycenterY = 0.f;

	for(unsigned int c=0 ; c<m_caloHitVec.size() ; c++)
	{
		EVENT::CalorimeterHit *pCaloHit = m_caloHitVec.at(c);

		barycenterX += cellIdDecoder(pCaloHit)["I"];
		barycenterY += cellIdDecoder(pCaloHit)["J"];
	}

	barycenterX /= m_caloHitVec.size();
	barycenterY /= m_caloHitVec.size();

	std::vector<CalorimeterHit *> caloHitVecForEnteringPoint;

	for(unsigned int c=0 ; c<m_caloHitVec.size() ; c++)
	{
		EVENT::CalorimeterHit *pCaloHit = m_caloHitVec.at(c);

		if(fabs(cellIdDecoder(pCaloHit)["I"]-barycenterX) < xyCut
				&& fabs(cellIdDecoder(pCaloHit)["J"]-barycenterY) < xyCut
				&& cellIdDecoder(pCaloHit)["K-1"] < layerCut)
		{
			caloHitVecForEnteringPoint.push_back(pCaloHit);
		}
	}

	enteringPointX = 0.f;
	enteringPointY = 0.f;

	for(unsigned int c=0 ; c<caloHitVecForEnteringPoint.size() ; c++)
	{
		EVENT::CalorimeterHit *pCaloHit = caloHitVecForEnteringPoint.at(c);

		enteringPointX += cellIdDecoder(pCaloHit)["I"];
		enteringPointY += cellIdDecoder(pCaloHit)["J"];
	}

	enteringPointX /= caloHitVecForEnteringPoint.size();
	enteringPointY /= caloHitVecForEnteringPoint.size();

	enteringPointX = - m_NCells0 / 2.0 * cellSize0 + enteringPointX * cellSize0;
	enteringPointY = - m_NCells1 / 2.0 * cellSize1 + enteringPointY * cellSize1;

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode SDHCALArborProcessor::FillRootTree()
{
 const pandora::PfoList *pPfoList = NULL;
 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::GetCurrentPfoList(*m_pPandora, pPfoList));
	int pfoID = 0;

 for (pandora::PfoList::const_iterator itPFO = pPfoList->begin(), itPFOEnd = pPfoList->end(); itPFO != itPFOEnd; ++itPFO)
 {
 	pandora::ParticleFlowObject *pPfo = *itPFO;
 	float pfoEnergy = 0.f;

 	// compute the pfo energy
 	pandora::CaloHitList allPfoCaloHitList;
 	const pandora::ClusterList &pfoClusterList = pPfo->GetClusterList();

  float pandoraPFOEnergy = pPfo->GetEnergy();

 	for(pandora::ClusterList::const_iterator clusterIter = pfoClusterList.begin() , clusterEndIter = pfoClusterList.end() ; clusterEndIter != clusterIter ; ++clusterIter)
 	{
 		const pandora::Cluster *pCluster = *clusterIter;
 		pandora::CaloHitList clusterCaloHitList;
 		pCluster->GetOrderedCaloHitList().GetCaloHitList(clusterCaloHitList);
 		allPfoCaloHitList.insert(clusterCaloHitList.begin(), clusterCaloHitList.end());
 	}

 	if(allPfoCaloHitList.empty())
 		continue;

 	SDHCALArborProcessor::SDHCALEnergyCorrectionFunctionCaloHitList(allPfoCaloHitList, pfoEnergy);
 	m_pfoEnergies.push_back(pfoEnergy);

 	const pandora::TrackList &trackList = pPfo->GetTrackList();
 	pandora::Track *pTrack = NULL;

 	if(!trackList.empty())
 	{
 		pTrack = *trackList.begin();
 		float chiComparison = pTrack->GetTrackStateAtCalorimeter().GetMomentum().GetMagnitude() - pfoEnergy;
 		m_chi2TrackClusterComparison.push_back(chiComparison*chiComparison);
 	}
 	else
 	{
 		m_chi2TrackClusterComparison.push_back(0.f);
 	}

 	if(trackList.empty())
 	{
 		m_isCharged.push_back(0);
 		m_neutralEnergy += pfoEnergy;
 		m_nNeutralParticles++;
 	}
 	else
 	{
 		m_isCharged.push_back(1);
 		m_chargedEnergy += pfoEnergy;
 		m_nChargedParticles++;
 	}

 	// identify nHit of each type (overlay study)
 	const pandora::ClusterAddressList clusterAddressList((*itPFO)->GetClusterAddressList());
 	m_nHitType1.push_back(0);
 	m_nHitType2.push_back(0);
 	m_nHitType3.push_back(0);

  for (pandora::ClusterAddressList::const_iterator itCluster = clusterAddressList.begin(), itClusterEnd = clusterAddressList.end();
      itCluster != itClusterEnd; ++itCluster)
  {
  	unsigned int nHitsInCluster((*itCluster).size());

   for (unsigned int iHit = 0; iHit < nHitsInCluster; ++iHit)
   {
   	EVENT::CalorimeterHit *pCaloHit = (EVENT::CalorimeterHit *)((*itCluster).at(iHit));

   	if(1 == pCaloHit->getType())
   		m_nHitType1.at(pfoID)++;
   	else if(2 == pCaloHit->getType())
   		m_nHitType2.at(pfoID)++;
   	else if(3 == pCaloHit->getType())
   		m_nHitType3.at(pfoID)++;
   }
  }

  pfoID++;
 }
 m_nPfos = pPfoList->size();

 float totalEventEnergy = 0.f;
 SDHCALArborProcessor::SDHCALEnergyCorrectionFunctionLCCaloHitVec(m_caloHitVec, totalEventEnergy);

 float energyMC1 = 0.f;
 float energyMC2 = 0.f;
 SDHCALArborProcessor::SDHCALEnergyCorrectionFunctionLCCaloHitVec(m_caloHitTypeParticle1, energyMC1);
 SDHCALArborProcessor::SDHCALEnergyCorrectionFunctionLCCaloHitVec(m_caloHitTypeParticle2, energyMC2);

 m_rootFile->cd();

 m_pTTreeWrapper->Set(m_rootTreeName, "nPfos", m_nPfos);
 m_pTTreeWrapper->Set(m_rootTreeName, "totalEventEnergy", totalEventEnergy);
 m_pTTreeWrapper->Set(m_rootTreeName, "energyMC1", energyMC1);
 m_pTTreeWrapper->Set(m_rootTreeName, "energyMC2", energyMC2);

 m_pTTreeWrapper->Set(m_rootTreeName, "nChargedParticles", m_nChargedParticles);
 m_pTTreeWrapper->Set(m_rootTreeName, "nNeutralParticles", m_nNeutralParticles);
 m_pTTreeWrapper->Set(m_rootTreeName, "chargedEnergy", m_chargedEnergy);
	m_pTTreeWrapper->Set(m_rootTreeName, "neutralEnergy", m_neutralEnergy);
	m_pTTreeWrapper->Set(m_rootTreeName, "isCharged", &m_isCharged);
	m_pTTreeWrapper->Set(m_rootTreeName, "nHitType1", &m_nHitType1);
	m_pTTreeWrapper->Set(m_rootTreeName, "nHitType2", &m_nHitType2);
	m_pTTreeWrapper->Set(m_rootTreeName, "nHitType3", &m_nHitType3);
	m_pTTreeWrapper->Set(m_rootTreeName, "chi2TrackClusterComparison", &m_chi2TrackClusterComparison);
	m_pTTreeWrapper->Set(m_rootTreeName, "pfoEnergies", &m_pfoEnergies);

	m_pTTreeWrapper->Fill(m_rootTreeName);

 pPfoList = NULL;

	return pandora::STATUS_CODE_SUCCESS;
}

//-----------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::Reset()
{
 m_nPfos = 0;
 m_nChargedParticles = 0;
 m_nNeutralParticles = 0;
 m_chargedEnergy = 0.f;
 m_neutralEnergy = 0.f;
 m_algorithmEfficiency = 0;
 m_isCharged.clear();
 m_nHitType1.clear();
 m_nHitType2.clear();
 m_nHitType3.clear();
 m_chi2TrackClusterComparison.clear();
 m_pfoEnergies.clear();
 m_centerOfGravityPfoX.clear();
 m_centerOfGravityPfoY.clear();
 m_centerOfGravityPfoZ.clear();

 m_caloHitVec.clear();
 m_caloHitTypeParticle1.clear();
 m_caloHitTypeParticle2.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::SDHCALEnergyCorrectionFunction(const pandora::Cluster *const pCluster, float &energyCorrection)
{
	if(NULL == pCluster)
		throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);

	if(0 == pCluster->GetNCaloHits())
	{
		energyCorrection = 0.f;
		return;
	}

	pandora::CaloHitList clusterCaloHitList;
	pCluster->GetOrderedCaloHitList().GetCaloHitList(clusterCaloHitList);

	SDHCALArborProcessor::SDHCALEnergyCorrectionFunctionCaloHitList(clusterCaloHitList, energyCorrection);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::SDHCALEnergyCorrectionFunctionCaloHitList(const pandora::CaloHitList &caloHitList, float &energyCorrection)
{

	unsigned int N = caloHitList.size();
	unsigned int N1 = 0;
	unsigned int N2 = 0;
	unsigned int N3 = 0;

	float alpha = m_sdhcalEnergyConstants.at(0) + m_sdhcalEnergyConstants.at(1)*N + m_sdhcalEnergyConstants.at(2) *N*N;
	float beta  = m_sdhcalEnergyConstants.at(3) + m_sdhcalEnergyConstants.at(4)*N + m_sdhcalEnergyConstants.at(5) *N*N;
	float gamma = m_sdhcalEnergyConstants.at(6) + m_sdhcalEnergyConstants.at(7)*N + m_sdhcalEnergyConstants.at(8) *N*N;

	for(pandora::CaloHitList::const_iterator iter = caloHitList.begin() , endIter = caloHitList.end() ; endIter != iter ; ++iter)
	{
		const pandora::CaloHit *pCaloHit = *iter;

		if( (pCaloHit->GetInputEnergy() - m_sdhcalEnergyFactors.at(0)) < std::numeric_limits<float>::epsilon() )
			N1++;
		else if((pCaloHit->GetInputEnergy() - m_sdhcalEnergyFactors.at(1)) < std::numeric_limits<float>::epsilon())
			N2++;
		else if((pCaloHit->GetInputEnergy() - m_sdhcalEnergyFactors.at(2)) < std::numeric_limits<float>::epsilon())
			N3++;
		else
			throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);
	}

	energyCorrection = alpha*N1 + beta*N2 + gamma*N3;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SDHCALArborProcessor::SDHCALEnergyCorrectionFunctionLCCaloHitVec(const std::vector<CalorimeterHit*> &caloHitVec, float &energyCorrection)
{
	unsigned int N = caloHitVec.size();
	unsigned int N1 = 0;
	unsigned int N2 = 0;
	unsigned int N3 = 0;

	energyCorrection = 0.f;

	if(0 == N)
		return;

	float alpha = m_sdhcalEnergyConstants.at(0) + m_sdhcalEnergyConstants.at(1)*N + m_sdhcalEnergyConstants.at(2) *N*N;
	float beta  = m_sdhcalEnergyConstants.at(3) + m_sdhcalEnergyConstants.at(4)*N + m_sdhcalEnergyConstants.at(5) *N*N;
	float gamma = m_sdhcalEnergyConstants.at(6) + m_sdhcalEnergyConstants.at(7)*N + m_sdhcalEnergyConstants.at(8) *N*N;

	for(std::vector<CalorimeterHit*>::const_iterator iter = caloHitVec.begin() , endIter = caloHitVec.end() ; endIter != iter ; ++iter)
	{
		const CalorimeterHit *pCaloHit = *iter;
		if( (pCaloHit->getEnergy() - 1.f) < std::numeric_limits<float>::epsilon() )
			N1++;
		else if((pCaloHit->getEnergy() - 2.f) < std::numeric_limits<float>::epsilon())
			N2++;
		else if((pCaloHit->getEnergy() - 3.f) < std::numeric_limits<float>::epsilon())
			N3++;
		else
			throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);
	}

	energyCorrection = alpha*N1 + beta*N2 + gamma*N3;
}
