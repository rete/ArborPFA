/**
 *  @file   marlin/include/SDHCALArborProcessor.h
 * 
 *  @brief  Header file for the arbor pfa processor class for sdhcal.
 * 
 *  $Log: $
 */

#ifndef SDHCAL_ARBOR_PROCESSOR_H
#define SDHCAL_ARBOR_PROCESSOR_H 1

#include "marlin/Processor.h"

#include "Utilities/BFieldCalculator.h"
#include "Utilities/PseudoLayerCalculator.h"
//#include "PfoCreator.h"

// lcio
#include "EVENT/CalorimeterHit.h"

namespace pandora
{
 class Pandora;
 class Cluster;
 class CartesianVector;
}

namespace pandora_monitoring
{
 class TTreeWrapper;
}

namespace arborpfa
{
 class Arbor;
}

class TFile;
class TBranch;
class TTree;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  SimpleBFieldCalculator class
 */
class SDHCALBFieldCalculator : public pandora::BFieldCalculator
{
public:
    static float        m_BField;              ///< The bfield for SDHCAL

private:
    void InitializeGeometry(){}
    float GetBField(const pandora::CartesianVector &positionVector) const
    {
    	return m_BField;
    }
};


/**
 *  @brief  SimpleBFieldCalculator class
 */
class SDHCALPseudoLayerCalculator : public pandora::PseudoLayerCalculator
{
public:

	static float         m_hCalEndCapInnerZ;
	static float         m_hCalEndCapLayerThickness;

private:

    void InitializeGeometry() {}

    virtual pandora::PseudoLayer GetPseudoLayer(const pandora::CartesianVector &positionVector) const;

    virtual pandora::PseudoLayer GetPseudoLayerAtIp() const;
};


/**
 *  @brief  PandoraPFANewProcessor class
 */
class SDHCALArborProcessor : public marlin::Processor
{
public:
    typedef std::vector<std::string> StringVector;
    typedef std::vector<float> FloatVector;

    /**
     *  @brief  Settings class
     */
    class Settings
    {
    public:
        std::string     m_pandoraSettingsXmlFile;           ///< The pandora settings xml file
        int             m_nEventsToSkip;                    ///< Number of events to skip at start of reconstruction job
    };

    /**
     *  @brief  Default constructor
     */
    SDHCALArborProcessor();

    /**
     *  @brief  Create new processor
     */
    virtual Processor *newProcessor();

    /**
     *  @brief  Initialize, called at startup
     */
    virtual void init();

    /**
     *  @brief  Process run header
     *
     *  @param  pLCRunHeader the lc run header
     */
    virtual void processRunHeader(lcio::LCRunHeader *pLCRunHeader);

    /**
     *  @brief  Process event, main entry point
     *
     *  @param  pLCEvent the lc event
     */
    virtual void processEvent(EVENT::LCEvent *pLCEvent);

    /**
     *  @brief  Checks for event
     *
     *  @param  pLCEvent the lc event
     */
    virtual void check(EVENT::LCEvent *pLCEvent);

    /**
     *  @brief  End, called at shutdown
     */
    virtual void end();

    /**
     *  @brief  Get address of the pandora instance
     * 
     *  @return address of the pandora instance
     */
    static pandora::Pandora *GetPandora();

    /**
     *  @brief  Get address of the current lcio event
     * 
     *  @return address of the current lcio event
     */
    static EVENT::LCEvent *GetCurrentEvent();

private:
    /**
     *  @brief  Register user algorithm factories, energy correction functions and particle id functions,
     *          insert user code here
     */
    pandora::StatusCode RegisterUserComponents() const;

    /**
     *
     */
    pandora::StatusCode CreateGeometry() const;

    /**
     *
     */
    pandora::StatusCode CreateCaloHits(EVENT::LCEvent *pLCEvent);

    /**
     *
     */
    pandora::StatusCode CreateTracks(EVENT::LCEvent *pLCEvent) const;

    /**
     *
     */
    pandora::StatusCode CreateParticleFlowObjects(EVENT::LCEvent *pLCEvent);

    /**
     *
     */
    pandora::StatusCode FillRootTree();

    /**
     *
     */
    pandora::StatusCode FindShowerEnteringPoint(float &enteringPointX, float &enteringPointY) const;

    /**
     *  @brief  Process steering file parameters, insert user code here
     */
    void ProcessSteeringFile();

    /**
     *  @brief  Copy some steering parameters between settings objects
     */
    void FinaliseSteeringParameters();

    /**
     *  @brief  Reset the pandora pfa new processor
     */
    void Reset();

    /**
     *
     */
    static void SDHCALEnergyCorrectionFunction(const pandora::Cluster *const pCluster, float &energyCorrection);

    /**
     *
     */
    static void SDHCALEnergyCorrectionFunctionCaloHitList(const pandora::CaloHitList &caloHitList, float &energyCorrection);

    /**
     *
     */
    static void SDHCALEnergyCorrectionFunctionLCCaloHitVec(const std::vector<EVENT::CalorimeterHit*> &caloHitVec, float &energyCorrection);

    static pandora::Pandora        *m_pPandora;                     ///< Address of the pandora instance
    static arborpfa::Arbor         *m_pArbor;                       ///< Address of the arbor instance
    static EVENT::LCEvent           *m_pLcioEvent;                   ///< Address of the current lcio event

    // global settings
    Settings                        m_settings;                     ///< The settings for the pandora pfa new processor
    unsigned int                    m_nRun;                         ///< The run number
    unsigned int                    m_nEvent;                       ///< The event number
    static FloatVector             m_sdhcalEnergyFactors;
    static FloatVector             m_sdhcalEnergyConstants;
    bool                           m_isSingleParticleStudy;
    float                          m_singleParticleMomentum;


    /*
     * analysis members
     */
    // names
    std::string                     m_rootFileName;
    std::string                     m_rootTreeName;

    // file and tree
    TFile                          *m_rootFile;
//    TTree                          *m_pPFOTuple;
    pandora_monitoring::TTreeWrapper *m_pTTreeWrapper;

    // branch variables
    int                            m_nPfos;
    int                            m_nChargedParticles;
    int                            m_nNeutralParticles;
    float                          m_chargedEnergy;
    float                          m_neutralEnergy;
    int                            m_algorithmEfficiency;
    std::vector<int>               m_isCharged;
    std::vector<int>               m_nHitType1;
    std::vector<int>               m_nHitType2;
    std::vector<int>               m_nHitType3;
    std::vector<float>            m_chi2TrackClusterComparison;
    std::vector<float>            m_pfoEnergies;
    std::vector<float>            m_centerOfGravityPfoX;
    std::vector<float>            m_centerOfGravityPfoY;
    std::vector<float>            m_centerOfGravityPfoZ;

    // event members
    std::string                     m_hCalCaloHitCollection;
    std::string                     m_genericObjectTrackInfoCollection;
    std::string                     m_clusterCollectionName;
    std::string                     m_pfoCollectionName;
    std::string                     m_detectorName;                 ///< The detector name

    std::vector<EVENT::CalorimeterHit *>   m_caloHitVec;
    std::vector<EVENT::CalorimeterHit *>   m_caloHitTypeParticle1;
    std::vector<EVENT::CalorimeterHit *>   m_caloHitTypeParticle2;

    // geometry parameters
    float                           m_hCalEndCapInnerR;
    float                           m_hCalEndCapInnerZ;
    float                           m_hCalEndCapLayerThickness;
    float                           m_absorberRadiationLength;
    float                           m_absorberInteractionLength;
    int                             m_NCells0;
    int                             m_NCells1;

};

//------------------------------------------------------------------------------------------------------------------------------------------

inline marlin::Processor *SDHCALArborProcessor::newProcessor()
{
    return new SDHCALArborProcessor();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Pandora *SDHCALArborProcessor::GetPandora()
{
    if (NULL == m_pPandora)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_INITIALIZED);

    return m_pPandora;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline EVENT::LCEvent *SDHCALArborProcessor::GetCurrentEvent()
{
    if (NULL == m_pLcioEvent)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_INITIALIZED);

    return m_pLcioEvent;
}

#endif // #ifndef SDHCAL_ARBOR_PROCESSOR_H
