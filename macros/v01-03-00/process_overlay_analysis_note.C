#include <TObject.h>
#include <TGraphErrors.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TH2.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TText.h>
#include <TMultiGraph.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TF1.h>
#include <TLatex.h>

#include <string>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include <OverlayPurityAnalysis.h>

//------------------------------------------------------

// particle type enum
enum ParticleType
{
	CHARGED,
	NEUTRAL
};

//------------------------------------------------------

// particle info struct
struct ParticleInfo
{
	ParticleType    m_type;
	int             m_energy;
};

//------------------------------------------------------

struct FitInputParameters
{
	FitInputParameters() :
		m_minFitValue(0),
		m_maxFitValue(0),
		m_pFitFunction(0),
	 m_shouldUsePredefinedFunction(false)
	{}

	~FitInputParameters()
	{
		if(m_pFitFunction != 0)
			delete m_pFitFunction;
	}

	float            m_minFitValue;
	float            m_maxFitValue;
	bool             m_shouldUsePredefinedFunction;
	TF1              *m_pFitFunction;
	std::string       m_predefinedFunctionName;
	std::string      m_fitOptions;
	std::string      m_drawOptions;
};

//------------------------------------------------------

struct FitResult
{
	float       m_chi2;
	bool        m_isSuccessful;
	float       m_mean;
	float       m_rms;
};

//------------------------------------------------------

// overlay file info struct
struct OverlayInfo
{
	~OverlayInfo()
	{
		if(m_pRootFile)
			delete m_pRootFile;

		if(m_pAnalysis)
			delete m_pAnalysis;
	}

	int                     m_separationDistance;
	std::string              m_fileName;
	ParticleInfo             m_particle1;
	ParticleInfo             m_particle2;
	OverlayPurityAnalysis   *m_pAnalysis;
	float                   m_meanNPfos;
	FitResult                m_recMeasNeutralFitResult;
	TFile                   *m_pRootFile;
};

//------------------------------------------------------

struct AlgorithmOverlayAnalysis
{
	typedef std::pair<TCanvas*,TMultiGraph*> CanvasMultiGraphPair;
	typedef std::map<std::string, CanvasMultiGraphPair> CanvasMultiGraphMap;
	typedef std::vector<OverlayInfo*> OverlayInfoList;

	~AlgorithmOverlayAnalysis()
	{
		for(CanvasMultiGraphMap::iterator iter = m_canvasMultiGraphMap.begin(), endIter = m_canvasMultiGraphMap.end() ; endIter != iter ; ++iter)
		{
			delete iter->second.first;
			delete iter->second.second;
		}

		for(OverlayInfoList::iterator iter = m_overlayInfoStorage.begin(), endIter = m_overlayInfoStorage.end() ; endIter != iter ; ++iter)
		{
			delete *iter;
		}

		m_separationDistanceList.clear();
		m_canvasMultiGraphMap.clear();
		m_overlayInfoStorage.clear();
	}

	OverlayInfoList                   m_overlayInfoStorage;
	CanvasMultiGraphMap               m_canvasMultiGraphMap;
	std::vector<int>                  m_separationDistanceList;

	std::string                       m_fileDirectory;
	std::string                       m_treeName;
	std::string                       m_physicsList;
	std::vector<AlgorithmType>        m_algorithmTypeList;
	std::vector<DataType>             m_dataTypeList;
};

//------------------------------------------------------

int getDefaultColor(AlgorithmType algorithmType)
{
	if(ARBOR_PFA == algorithmType)
		return kRed;
	else
		return kBlack;
}

//------------------------------------------------------

int getDefaultLineStyle(DataType dataType)
{
	if(TEST_BEAM == dataType)
		return 7;  // dashed line style
	else
		return 1;  // plain line style
}

//------------------------------------------------------

// fake function for root in order to compile the script
void process_overlay_analysis_note() {}

//------------------------------------------------------


void fitHistogramWithGaussian(const FitInputParameters &inputParameters, FitResult &fitResult, TH1 *pHistogram)
{
	if(pHistogram == 0)
		return;

	fitResult.m_isSuccessful = true;

	TFitResultPtr resultPtr = pHistogram->Fit("gaus",
			inputParameters.m_fitOptions.c_str(),
			inputParameters.m_drawOptions.c_str(),
			inputParameters.m_minFitValue,
			inputParameters.m_maxFitValue);

	if(resultPtr.fStatus != 0)
	{
		fitResult.m_isSuccessful = false;
		return;
	}

	fitResult.m_chi2 = resultPtr->Chi2();
	fitResult.m_mean = resultPtr->Parameter(1);
	fitResult.m_rms = resultPtr->Parameter(2);
}

//------------------------------------------------------

void processAlgorithmOverlayAnalysis()
{
	unsigned int startingDistance = 5;
	unsigned int endDistance = 30;
	unsigned int distanceStep = 5;

	float       minFitValue = -4;
	float       maxFitValue = 4;
	float       purityCut = 0.9;

	ParticleInfo particle1;
	particle1.m_energy = 10;
	particle1.m_type = NEUTRAL;

	ParticleInfo particle2;
	particle2.m_energy = 30;
	particle2.m_type = CHARGED;

	AlgorithmOverlayAnalysis *pAnalysis = new AlgorithmOverlayAnalysis();

	pAnalysis->m_fileDirectory = "/home/remi/git/ArborPFA/output/v01-03-00/Separation/";
	pAnalysis->m_physicsList = "FTFP_BERT_HP";
	pAnalysis->m_treeName = "PfoMonitoring";
	pAnalysis->m_algorithmTypeList.push_back(ARBOR_PFA);
	pAnalysis->m_algorithmTypeList.push_back(PANDORA_PFA);
	pAnalysis->m_dataTypeList.push_back(SIMULATION);
	pAnalysis->m_dataTypeList.push_back(TEST_BEAM);

		for(unsigned int d=startingDistance ; d<=endDistance; d+=distanceStep)
			pAnalysis->m_separationDistanceList.push_back(d);

	CaliceStyle();

	// purity and efficiency for the charged particle
	pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].first = new TCanvas("CHARGED_PURITY", "Mean charged hit purity");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].second = new TMultiGraph();
//	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].first = new TCanvas("NEUTRAL_PURITY", "Mean neutral hit purity");
//	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].second = new TMultiGraph();

	// purity and efficiency for the neutral particle
	pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].first = new TCanvas("CHARGED_EFFICIENCY", "Mean charged hit efficiency");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].second = new TMultiGraph();
//	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].first = new TCanvas("NEUTRAL_EFFICIENCY", "Mean neutral hit efficiency");
//	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].second = new TMultiGraph();

	// mean number of pfos
	pAnalysis->m_canvasMultiGraphMap["N_PFOS"].first = new TCanvas("N_PFOS", "Mean number of pfos");
	pAnalysis->m_canvasMultiGraphMap["N_PFOS"].second = new TMultiGraph();

	// mean recovered - measured energy for the neutral particle
	pAnalysis->m_canvasMultiGraphMap["REC_MEAS_NEUTRAL_ENERGY"].first = new TCanvas("REC_MEAS_NEUTRAL_ENERGY", "Mean recovered - measured neutral energy");
	pAnalysis->m_canvasMultiGraphMap["REC_MEAS_NEUTRAL_ENERGY"].second = new TMultiGraph();

	// pure reconstructed neutral energy mean
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_MEAN"].first = new TCanvas("PURE_REC_NEUTRAL_ENERGY_MEAN", "Pure reconstructed neutral energy");
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_MEAN"].second = new TMultiGraph();

	// pure reconstructed charged energy mean
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_MEAN"].first = new TCanvas("PURE_REC_CHARGED_ENERGY_MEAN", "Pure reconstructed charged energy");
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_MEAN"].second = new TMultiGraph();

	// pure reconstructed neutral energy rms
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_RESOL"].first = new TCanvas("PURE_REC_NEUTRAL_ENERGY_RESOL", "Pure reconstructed neutral energy");
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_RESOL"].second = new TMultiGraph();

	// pure reconstructed charged energy rms
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_RESOL"].first = new TCanvas("PURE_REC_CHARGED_ENERGY_RESOL", "Pure reconstructed charged energy");
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_RESOL"].second = new TMultiGraph();

	// format the canvases
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["N_PFOS"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["REC_MEAS_NEUTRAL_ENERGY"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_MEAN"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_MEAN"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_RESOL"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_RESOL"].first);

	// loop over SIM and TB
	for(unsigned int dt=0 ; dt<pAnalysis->m_dataTypeList.size() ; dt++)
	{
		DataType dataType = pAnalysis->m_dataTypeList.at(dt);

		// loop over ARBOR_PFA and PANDORA_PFA
		for(unsigned int at=0 ; at<pAnalysis->m_algorithmTypeList.size() ; at++)
		{
			AlgorithmType algorithmType = pAnalysis->m_algorithmTypeList.at(at);

			// single graph declaration
			TGraph *pChargedPurityGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pChargedEfficiencyGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNPfosGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pRecMeasNeutralEnergyGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pPureRecNeutralEnergyMeanGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pPureRecChargedEnergyMeanGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pPureRecNeutralEnergyResolGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pPureRecChargedEnergyResolGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			int currentPointId = 0;
			int currentPointId2 = 0;
			int currentPointId3 = 0;

			// loop over separation distance
			for(unsigned int dis=0 ; dis<pAnalysis->m_separationDistanceList.size() ; dis++)
			{
				OverlayInfo *pOverlayInfo = new OverlayInfo();

				pOverlayInfo->m_separationDistance = pAnalysis->m_separationDistanceList.at(dis);
				pOverlayInfo->m_fileName = pAnalysis->m_fileDirectory
						+ BuildOverlayEventFileName(particle1.m_energy, particle2.m_energy, pOverlayInfo->m_separationDistance, algorithmType, dataType);
				pOverlayInfo->m_particle1 = particle1;
				pOverlayInfo->m_particle2 = particle2;

				pOverlayInfo->m_pRootFile = TFile::Open(pOverlayInfo->m_fileName.c_str());
				TTree *pTree = (TTree*) pOverlayInfo->m_pRootFile->Get(pAnalysis->m_treeName.c_str());
				pOverlayInfo->m_pAnalysis = new OverlayPurityAnalysis(pTree);
				pOverlayInfo->m_pAnalysis->SetPurityCut(purityCut);
				pOverlayInfo->m_pAnalysis->Loop();

				pChargedPurityGraph->SetPoint(     dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->purityChargedDistribution.mean);
				pChargedEfficiencyGraph->SetPoint( dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->efficiencyChargedDistribution.mean);
				pNPfosGraph->SetPoint(             dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->nPfosDistribution.mean);

				TH1 *pNeutralEnergyDifferenceHistogram = createHistogram("tmpHisto", "tmpHisto", 201, -50, 50, pOverlayInfo->m_pAnalysis->energyDifferenceNeutralDistribution);
				FitInputParameters inputParams;
				FitResult fitResult;
				inputParams.m_minFitValue = minFitValue;
				inputParams.m_maxFitValue = maxFitValue;
				inputParams.m_fitOptions = "S";

				fitHistogramWithGaussian(inputParams, fitResult, pNeutralEnergyDifferenceHistogram);

				if(fitResult.m_isSuccessful)
				{
					pRecMeasNeutralEnergyGraph->SetPoint(currentPointId, pOverlayInfo->m_separationDistance, fitResult.m_mean);
					currentPointId++;
				}
				else
				{
					std::cout << "*** WARNING : non successful fit (neutral difference)! ***" << std::endl;
				}

				delete pNeutralEnergyDifferenceHistogram;

				std::stringstream ss;
				ss << "histo_neutral_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << pOverlayInfo->m_separationDistance << "cm";

				TH1 *pPureNeutralEnergyHistogram = createHistogram(ss.str().c_str(), ss.str().c_str(), particle1.m_energy*6 + 1,
						particle1.m_energy-3*particle1.m_energy, particle1.m_energy+3*particle1.m_energy,
						pOverlayInfo->m_pAnalysis->pureNeutralEnergyDistribution);

				FitInputParameters inputParams2;
				FitResult fitResult2;
				inputParams2.m_minFitValue = particle1.m_energy-2*particle1.m_energy;
				inputParams2.m_maxFitValue = particle1.m_energy+2*particle1.m_energy;
				inputParams2.m_fitOptions = "S";
				fitHistogramWithGaussian(inputParams2, fitResult2, pPureNeutralEnergyHistogram);

				if(fitResult2.m_isSuccessful)
				{
					pPureRecNeutralEnergyMeanGraph->SetPoint(currentPointId2, pOverlayInfo->m_separationDistance, fitResult2.m_mean);
					pPureRecNeutralEnergyResolGraph->SetPoint(currentPointId2, pOverlayInfo->m_separationDistance, fitResult2.m_rms/particle1.m_energy);
					currentPointId2++;
				}
				else
				{
					std::cout << "*** WARNING : non successful fit (pure neutral) ! ***" << std::endl;
				}

				delete pPureNeutralEnergyHistogram;

				ss.str("");
				ss << "histo_charged_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << pOverlayInfo->m_separationDistance << "cm";

				TH1 *pPureChargedEnergyHistogram = createHistogram(ss.str().c_str(), ss.str().c_str(), particle2.m_energy*6 + 1,
						particle2.m_energy-3*particle2.m_energy, particle2.m_energy+3*particle2.m_energy,
						pOverlayInfo->m_pAnalysis->pureChargedEnergyDistribution);

				FitInputParameters inputParams3;
				FitResult fitResult3;
				inputParams3.m_minFitValue = particle2.m_energy-2*particle2.m_energy;
				inputParams3.m_maxFitValue = particle2.m_energy+2*particle2.m_energy;
				inputParams3.m_fitOptions = "S";
				fitHistogramWithGaussian(inputParams3, fitResult3, pPureChargedEnergyHistogram);

				if(fitResult3.m_isSuccessful)
				{
					pPureRecChargedEnergyMeanGraph->SetPoint(currentPointId3, pOverlayInfo->m_separationDistance, fitResult3.m_mean);
					pPureRecChargedEnergyResolGraph->SetPoint(currentPointId3, pOverlayInfo->m_separationDistance, fitResult3.m_rms/particle2.m_energy);
					currentPointId3++;
				}
				else
				{
					std::cout << "*** WARNING : non successful fit (pure charged) ! ***" << std::endl;
				}

				delete pPureChargedEnergyHistogram;

				pAnalysis->m_overlayInfoStorage.push_back(pOverlayInfo);
			}

			std::stringstream titleStream;
			std::string particleType1Str = particle1.m_type == NEUTRAL ? "neutral" : "charged";
			std::string particleType2Str = particle2.m_type == NEUTRAL ? "neutral" : "charged";
			titleStream << particle1.m_energy << " GeV "
					<< particleType1Str << " , " << particle2.m_energy << " GeV "
					<< particleType2Str;

			DrawAttributeMapping attributeMapping(algorithmType, dataType);

			configureGraph(pChargedPurityGraph, attributeMapping);
			pChargedPurityGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].second->Add(pChargedPurityGraph);

			configureGraph(pChargedEfficiencyGraph, attributeMapping);
			pChargedEfficiencyGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].second->Add(pChargedEfficiencyGraph);

			configureGraph(pNPfosGraph, attributeMapping);
			pNPfosGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["N_PFOS"].second->Add(pNPfosGraph);

			configureGraph(pRecMeasNeutralEnergyGraph, attributeMapping);
			pRecMeasNeutralEnergyGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["REC_MEAS_NEUTRAL_ENERGY"].second->Add(pRecMeasNeutralEnergyGraph);

			configureGraph(pPureRecNeutralEnergyMeanGraph, attributeMapping);
			pPureRecNeutralEnergyMeanGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_MEAN"].second->Add(pPureRecNeutralEnergyMeanGraph);

			configureGraph(pPureRecNeutralEnergyResolGraph, attributeMapping);
			pPureRecNeutralEnergyResolGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_RESOL"].second->Add(pPureRecNeutralEnergyResolGraph);

			configureGraph(pPureRecChargedEnergyMeanGraph, attributeMapping);
			pPureRecChargedEnergyMeanGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_MEAN"].second->Add(pPureRecChargedEnergyMeanGraph);

			configureGraph(pPureRecChargedEnergyResolGraph, attributeMapping);
			pPureRecChargedEnergyResolGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_RESOL"].second->Add(pPureRecChargedEnergyResolGraph);
		} // algorithm loop
	} // data loop

	// plots everything
	pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].second, "Charged particle purity");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].second->GetYaxis()->SetRangeUser(0, 1);
	pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].second, "Charged particle efficiency");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].second->GetYaxis()->SetRangeUser(0, 1);
	pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["N_PFOS"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["N_PFOS"].second, "#N_{pfo}");
	pAnalysis->m_canvasMultiGraphMap["N_PFOS"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["REC_MEAS_NEUTRAL_ENERGY"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["REC_MEAS_NEUTRAL_ENERGY"].second, "Mean of Recovered - measured neutral energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["REC_MEAS_NEUTRAL_ENERGY"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_MEAN"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_MEAN"].second, "Mean of pure reconstructed neutral energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_MEAN"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_RESOL"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_RESOL"].second, "#sigma_{E}/E of pure reconstructed neutral energy");
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_NEUTRAL_ENERGY_RESOL"].first->Update();

 pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_MEAN"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_MEAN"].second, "Mean of pure reconstructed charged energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_MEAN"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_RESOL"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_RESOL"].second, "#sigma_{E}/E of pure reconstructed charged energy");
	pAnalysis->m_canvasMultiGraphMap["PURE_REC_CHARGED_ENERGY_RESOL"].first->Update();

	// and see the result
	pAnalysis->m_canvasMultiGraphMap["N_PFOS"].first->WaitPrimitive();

	delete pAnalysis;
}

