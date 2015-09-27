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
#include <fstream>
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

void processAlgorithmOverlayAnalysis(int chargedEnergy = 50)
{
	unsigned int startingDistance = 5;
	unsigned int endDistance = 30;
	unsigned int distanceStep = 5;

	float       minFitValue = -4;
	float       maxFitValue = 4;
	bool        drawDetailed = true;
	bool        saveDetailedPlots = false;
	std::string fitOpt = drawDetailed ? "Q" : "NQ";
	std::string controlPlotsDirectory = "ControlPlots/";

	ParticleInfo particle1;
	particle1.m_energy = 10;
	particle1.m_type = NEUTRAL;

	ParticleInfo particle2;
	particle2.m_energy = chargedEnergy;
	particle2.m_type = CHARGED;

	AlgorithmOverlayAnalysis *pAnalysis = new AlgorithmOverlayAnalysis();

	pAnalysis->m_fileDirectory = "/home/remi/git/ArborPFA/output/v01-04-00/Separation/newFiles/Separation/";
	pAnalysis->m_physicsList = "FTFP_BERT_HP";
	pAnalysis->m_treeName = "PfoMonitoring";
	pAnalysis->m_algorithmTypeList.push_back(ARBOR_PFA);
	pAnalysis->m_dataTypeList.push_back(TEST_BEAM);

	for(unsigned int d=startingDistance ; d<=endDistance; d+=distanceStep)
		pAnalysis->m_separationDistanceList.push_back(d);

	CaliceStyle();

	// mean number of pfos
	pAnalysis->m_canvasMultiGraphMap["N_PFOS"].first = new TCanvas("N_PFOS", "Mean number of pfos");
	pAnalysis->m_canvasMultiGraphMap["N_PFOS"].second = new TMultiGraph();

	// charged particle
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_MEAN"].first = new TCanvas("CHARGED_ENERGY_MEAN", "Mean charged ERec from GetMean()");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_MEAN"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_FIT"].first = new TCanvas("CHARGED_ENERGY_FIT", "Mean charged ERec from fit");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_FIT"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_MEAN"].first = new TCanvas("CHARGED_ENERGY_DIFFERENCE_MEAN", "Charged ERec - EMeas from GetMean()");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_MEAN"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_FIT"].first = new TCanvas("CHARGED_ENERGY_DIFFERENCE_FIT", "Charged ERec - EMeas from fit");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_FIT"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].first = new TCanvas("CHARGED_EFFICIENCY", "Mean charged hit efficiency");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].first = new TCanvas("CHARGED_PURITY", "Mean charged hit purity");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].second = new TMultiGraph();

	// neutral particle
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].first = new TCanvas("NEUTRAL_PURITY", "Mean neutral hit purity");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].first = new TCanvas("NEUTRAL_EFFICIENCY", "Mean neutral hit efficiency");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN"].first = new TCanvas("NEUTRAL_ENERGY_MEAN", "Mean neutral ERec from GetMean()");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN_EFFICIENT"].first = new TCanvas("NEUTRAL_ENERGY_MEAN_EFFICIENT", "Mean neutral ERec from GetMean() when at least one neutral has been reconstructed");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN_EFFICIENT"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_MEAN"].first = new TCanvas("NEUTRAL_ENERGY_DIFFERENCE_MEAN", "Neutral ERec - EMeas from GetMean()");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_MEAN"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_MEAN_EFFICIENT"].first = new TCanvas("NEUTRAL_ENERGY_DIFFERENCE_MEAN_EFFICIENT", "Neutral ERec - EMeas from GetMean() when at least one neutral has been reconstructed ");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_MEAN_EFFICIENT"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_1_SIGMA"].first = new TCanvas("NEUTRAL_PROBA_WITHIN_1_SIGMA", "Probability to recover the neutral hadron within 1 sigma of the real energy");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_1_SIGMA"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_2_SIGMA"].first = new TCanvas("NEUTRAL_PROBA_WITHIN_2_SIGMA", "Probability to recover the neutral hadron within 2 sigma of the real energy");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_2_SIGMA"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_1_SIGMA"].first = new TCanvas("NEUTRAL_DIFFERENCE_PROBA_WITHIN_1_SIGMA", "Probability to recover the neutral hadron within 1 sigma of the reconstructed energy");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_1_SIGMA"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_2_SIGMA"].first = new TCanvas("NEUTRAL_DIFFERENCE_PROBA_WITHIN_2_SIGMA", "Probability to recover the neutral hadron within 2 sigma of the reconstructed energy");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_2_SIGMA"].second = new TMultiGraph();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_FIT"].first = new TCanvas("NEUTRAL_ENERGY_FIT", "Mean neutral ERec from fit");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_FIT"].second = new TMultiGraph();
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_FIT"].first = new TCanvas("NEUTRAL_ENERGY_DIFFERENCE_FIT", "Neutral ERec - EMeas from fit");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_FIT"].second = new TMultiGraph();

	// format the canvases
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["N_PFOS"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_MEAN"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_MEAN_EFFICIENT"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_FIT"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_MEAN"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_FIT"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_FIT"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_MEAN"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_FIT"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_1_SIGMA"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_2_SIGMA"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_1_SIGMA"].first);
	CanvasFormat(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_2_SIGMA"].first);

	// for fits in the loop
	gStyle->SetOptFit(1);

	TF1 *pGaus = new TF1("gausFunc", "gaus", -120, 120);

	// loop over data types
	for(unsigned int dt=0 ; dt<pAnalysis->m_dataTypeList.size() ; dt++)
	{
		DataType dataType = pAnalysis->m_dataTypeList.at(dt);

		// loop over algorithms
		for(unsigned int at=0 ; at<pAnalysis->m_algorithmTypeList.size() ; at++)
		{
			AlgorithmType algorithmType = pAnalysis->m_algorithmTypeList.at(at);

			// global graphs
			TGraph *pNPfosGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));

			// charged particle graph
			TGraph *pChargedPurityGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pChargedEfficiencyGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pChargedEnergyMeanGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pChargedEnergyFitGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pChargedEnergyDifferenceMeanGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pChargedEnergyDifferenceFitGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));

			// neutral particle graph
			TGraph *pNeutralPurityGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNeutralEfficiencyGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNeutralEnergyMeanGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNeutralEnergyMeanEfficientGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNeutralEnergyFitGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNeutralEnergyDifferenceMeanGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNeutralEnergyDifferenceFitGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNeutralProbaRecover1SigmaGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNeutralProbaRecover2SigmaGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNeutralDifferenceProbaRecover1SigmaGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));
			TGraph *pNeutralDifferenceProbaRecover2SigmaGraph = new TGraph(int(pAnalysis->m_separationDistanceList.size()));

			std::stringstream fileName;

			std::ofstream f_npfos;
			fileName << "data_npfos_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_npfos.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralEfficiency;
			fileName << "data_neutralEfficiency_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralEfficiency.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralPurity;
			fileName << "data_neutralPurity_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralPurity.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralERec;
			fileName << "data_neutralERec_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralERec.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralERecNeutralEfficient;
			fileName << "data_neutralERecNeutralEfficient_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralERecNeutralEfficient.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralERecDifference;
			fileName << "data_neutralERecDifference_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralERecDifference.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralERecDifferenceNeutralEfficient;
			fileName << "data_neutralERecDifferenceNeutralEfficient_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralERecDifferenceNeutralEfficient.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralProba1Sigma;
			fileName << "data_neutralProba1Sigma_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralProba1Sigma.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralProba2Sigma;
			fileName << "data_neutralProba2Sigma_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralProba2Sigma.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralDifferenceProba1Sigma;
			fileName << "data_neutralDifferenceProba1Sigma_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralDifferenceProba1Sigma.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralDifferenceProba2Sigma;
			fileName << "data_neutralDifferenceProba2Sigma_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralDifferenceProba2Sigma.open(fileName.str().c_str());
			fileName.str("");

			std::ofstream f_neutralPercentage;
			fileName << "data_neutralPercentage_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << particle2.m_energy << "Gev.txt";
			f_neutralPercentage.open(fileName.str().c_str());
			fileName.str("");


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
				pOverlayInfo->m_pAnalysis->Loop();

				std::stringstream ss;
				TCanvas *pCanvas;

				if(drawDetailed)
				{
					pCanvas = new TCanvas();
					pCanvas->cd();
				}


				ss << "histo_neutral_mcenergy_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << pOverlayInfo->m_separationDistance << "cm";
				TH1 *pNeutralMCEnergyHistogram = createHistogram(ss.str().c_str(), ss.str().c_str(), particle1.m_energy*6 + 1,
						particle1.m_energy-3*particle1.m_energy, particle1.m_energy+3*particle1.m_energy,
						pOverlayInfo->m_pAnalysis->neutralMCEnergyDistribution);

				pNeutralMCEnergyHistogram->Fit(pGaus, fitOpt.c_str(), "", particle1.m_energy-particle1.m_energy*0.5, particle1.m_energy+particle1.m_energy*0.5);

				float neutralMCEnergyMeanFit = pGaus->GetParameter(1);
				float neutralMCEnergyLowerBound1Sigma = pGaus->GetParameter(1) + -1*pGaus->GetParameter(2);
				float neutralMCEnergyUpperBound1Sigma = pGaus->GetParameter(1) +  1*pGaus->GetParameter(2);
				float neutralMCEnergyLowerBound2Sigma = pGaus->GetParameter(1) + -2*pGaus->GetParameter(2);
				float neutralMCEnergyUpperBound2Sigma = pGaus->GetParameter(1) +  2*pGaus->GetParameter(2);

				float neutralMCEnergy1SigmaCount = count(pOverlayInfo->m_pAnalysis->neutralMCEnergyDistribution, neutralMCEnergyLowerBound1Sigma, neutralMCEnergyUpperBound1Sigma);
				float neutralRecEnergy1SigmaCount = count(pOverlayInfo->m_pAnalysis->neutralEnergyDistribution, neutralMCEnergyLowerBound1Sigma, neutralMCEnergyUpperBound1Sigma);
				float neutralProbaRecover1Sigma = neutralRecEnergy1SigmaCount/neutralMCEnergy1SigmaCount;

				float neutralMCEnergy2SigmaCount = count(pOverlayInfo->m_pAnalysis->neutralMCEnergyDistribution, neutralMCEnergyLowerBound2Sigma, neutralMCEnergyUpperBound2Sigma);
				float neutralRecEnergy2SigmaCount = count(pOverlayInfo->m_pAnalysis->neutralEnergyDistribution, neutralMCEnergyLowerBound2Sigma, neutralMCEnergyUpperBound2Sigma);
				float neutralProbaRecover2Sigma = neutralRecEnergy2SigmaCount/neutralMCEnergy2SigmaCount;

				// extract neutral energy from fit around the expected mean energy
				ss.str("");
				ss << "histo_neutral_energy_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << pOverlayInfo->m_separationDistance << "cm";
				TH1 *pNeutralEnergyHistogram = createHistogram(ss.str().c_str(), ss.str().c_str(), particle1.m_energy*6 + 1,
						particle1.m_energy-3*particle1.m_energy, particle1.m_energy+3*particle1.m_energy,
						pOverlayInfo->m_pAnalysis->neutralEnergyDistribution);

				pNeutralEnergyHistogram->Fit(pGaus, fitOpt.c_str(), "", particle1.m_energy-particle1.m_energy*0.5, particle1.m_energy+particle1.m_energy*0.5);
				double neutralEnergyFit = pGaus->GetParameter(1);

				if(drawDetailed)
				{
					pNeutralMCEnergyHistogram->DrawCopy();

					if(saveDetailedPlots)
					{
						std::string fileName = controlPlotsDirectory + ss.str() + ".pdf";
						pCanvas->SaveAs(fileName.c_str());
					}
				}

				delete pNeutralEnergyHistogram;

				if(drawDetailed)
				{
					pCanvas = new TCanvas();
					pCanvas->cd();
				}

				// extract charged energy from fit around the expected mean energy
				ss.str("");
				ss << "histo_charged_energy_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << pOverlayInfo->m_separationDistance << "cm";
				TH1 *pChargedEnergyHistogram = createHistogram(ss.str().c_str(), ss.str().c_str(), particle2.m_energy*6 + 1,
						particle2.m_energy-3*particle2.m_energy, particle2.m_energy+3*particle2.m_energy,
						pOverlayInfo->m_pAnalysis->chargedEnergyDistribution);

				pChargedEnergyHistogram->Fit(pGaus, "Q", "", particle2.m_energy-particle2.m_energy*0.5, particle2.m_energy+particle2.m_energy*0.5);
				double chargedEnergyFit = pGaus->GetParameter(1);

				if(drawDetailed)
				{
					pChargedEnergyHistogram->DrawCopy();

					if(saveDetailedPlots)
					{
						std::string fileName = controlPlotsDirectory + ss.str() + ".pdf";
						pCanvas->SaveAs(fileName.c_str());
					}
				}

				delete pChargedEnergyHistogram;




				if(drawDetailed)
				{
					pCanvas = new TCanvas();
					pCanvas->cd();
				}

				float neutralEnergyDifference1Sigma = integral(pOverlayInfo->m_pAnalysis->neutralEnergyDifferenceDistribution, neutralMCEnergyLowerBound1Sigma - neutralMCEnergyMeanFit, neutralMCEnergyUpperBound1Sigma - neutralMCEnergyMeanFit);
				float neutralEnergyDifference2Sigma = integral(pOverlayInfo->m_pAnalysis->neutralEnergyDifferenceDistribution, neutralMCEnergyLowerBound2Sigma - neutralMCEnergyMeanFit, neutralMCEnergyUpperBound2Sigma - neutralMCEnergyMeanFit);

				// extract neutral rec-meas from fit around 0
				ss.str("");
				ss << "histo_neutral_energy_difference_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << pOverlayInfo->m_separationDistance << "cm";
				TH1 *pNeutralEnergyDifferenceHistogram = createHistogram(ss.str().c_str(), ss.str().c_str(), 121, -60, 60,
						pOverlayInfo->m_pAnalysis->neutralEnergyDifferenceDistribution);

				pNeutralEnergyDifferenceHistogram->Fit(pGaus, fitOpt.c_str(), "", -4, 4);
				double neutralEnergyDifferenceFit = pGaus->GetParameter(1);

				if(drawDetailed)
				{
					pNeutralEnergyDifferenceHistogram->DrawCopy();

					if(saveDetailedPlots)
					{
						std::string fileName = controlPlotsDirectory + ss.str() + ".pdf";
						pCanvas->SaveAs(fileName.c_str());
					}
				}

				delete pNeutralEnergyDifferenceHistogram;

				if(drawDetailed)
				{
					pCanvas = new TCanvas();
					pCanvas->cd();
				}

				// extract charged rec-meas from fit around 0
				ss.str("");
				ss << "histo_charged_energy_difference_" << AlgorithmName(algorithmType) << "_" << DataName(dataType) << "_" << pOverlayInfo->m_separationDistance << "cm";
				TH1 *pChargedEnergyDifferenceHistogram = createHistogram(ss.str().c_str(), ss.str().c_str(), 121, -60, 60,
						pOverlayInfo->m_pAnalysis->chargedEnergyDifferenceDistribution);

				pChargedEnergyDifferenceHistogram->Fit(pGaus, "Q", "", -4, 4);
				double chargedEnergyDifferenceFit = pGaus->GetParameter(1);

				if(drawDetailed)
				{
					pChargedEnergyDifferenceHistogram->DrawCopy();

					if(saveDetailedPlots)
					{
						std::string fileName = controlPlotsDirectory + ss.str() + ".pdf";
						pCanvas->SaveAs(fileName.c_str());
					}
				}

				delete pChargedEnergyDifferenceHistogram;

				/*****************/
				//  FILL GRAPHS  //
				/*****************/

				// global graphs
				pNPfosGraph->SetPoint(             dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->nPfosDistribution.mean);
				f_npfos << pOverlayInfo->m_pAnalysis->nPfosDistribution.mean << " ";

				// charged particle graphs
				pChargedPurityGraph->SetPoint(    dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->chargedPurityDistribution.mean);
				pChargedEfficiencyGraph->SetPoint(dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->chargedEfficiencyDistribution.mean);

				pChargedEnergyMeanGraph->SetPoint(dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->chargedEnergyDistribution.mean);
				pChargedEnergyFitGraph->SetPoint( dis, pOverlayInfo->m_separationDistance, chargedEnergyFit);

				pChargedEnergyDifferenceMeanGraph->SetPoint(dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->chargedEnergyDifferenceDistribution.mean);
				pChargedEnergyDifferenceFitGraph->SetPoint( dis, pOverlayInfo->m_separationDistance, chargedEnergyDifferenceFit);

				// neutral particle graphs
				pNeutralPurityGraph->SetPoint(     dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->neutralPurityDistribution.mean);
				f_neutralPurity << pOverlayInfo->m_pAnalysis->neutralPurityDistribution.mean << " ";
				pNeutralEfficiencyGraph->SetPoint( dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->neutralEfficiencyDistribution.mean);
				f_neutralEfficiency << pOverlayInfo->m_pAnalysis->neutralEfficiencyDistribution.mean << " ";

				pNeutralEnergyMeanGraph->SetPoint(dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->neutralEnergyDistribution.mean);
				f_neutralERec << pOverlayInfo->m_pAnalysis->neutralEnergyDistribution.mean << " ";
				pNeutralEnergyMeanEfficientGraph->SetPoint(dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->neutralEnergyEfficientDistribution.mean);
				f_neutralERecNeutralEfficient << pOverlayInfo->m_pAnalysis->neutralEnergyEfficientDistribution.mean << " ";
				pNeutralEnergyFitGraph->SetPoint( dis, pOverlayInfo->m_separationDistance, neutralEnergyFit);

				pNeutralProbaRecover1SigmaGraph->SetPoint(dis, pOverlayInfo->m_separationDistance, neutralProbaRecover1Sigma);
				f_neutralProba1Sigma << neutralProbaRecover1Sigma << " ";
				pNeutralProbaRecover2SigmaGraph->SetPoint(dis, pOverlayInfo->m_separationDistance, neutralProbaRecover2Sigma);
				f_neutralProba2Sigma << neutralProbaRecover2Sigma << " ";
				pNeutralDifferenceProbaRecover1SigmaGraph->SetPoint(dis, pOverlayInfo->m_separationDistance, neutralEnergyDifference1Sigma);
				f_neutralDifferenceProba1Sigma << neutralEnergyDifference1Sigma << " ";
				pNeutralDifferenceProbaRecover2SigmaGraph->SetPoint(dis, pOverlayInfo->m_separationDistance, neutralEnergyDifference2Sigma);
				f_neutralDifferenceProba2Sigma << neutralEnergyDifference2Sigma << " ";

				pNeutralEnergyDifferenceMeanGraph->SetPoint(dis, pOverlayInfo->m_separationDistance, pOverlayInfo->m_pAnalysis->neutralEnergyDifferenceDistribution.mean);
				f_neutralERecDifference << pOverlayInfo->m_pAnalysis->neutralEnergyDifferenceDistribution.mean << " ";
				pNeutralEnergyDifferenceFitGraph->SetPoint( dis, pOverlayInfo->m_separationDistance, neutralEnergyDifferenceFit);
				f_neutralERecDifferenceNeutralEfficient << pOverlayInfo->m_pAnalysis->neutralEnergyDifferenceEfficientDistribution.mean << " ";

				f_neutralPercentage << pOverlayInfo->m_pAnalysis->neutralRecPercentage << " ";

				pAnalysis->m_overlayInfoStorage.push_back(pOverlayInfo);
			}

			f_npfos.close();
			f_neutralEfficiency.close();
			f_neutralPurity.close();
			f_neutralERec.close();
			f_neutralERecDifference.close();
			f_neutralERecDifferenceNeutralEfficient.close();
			f_neutralERecNeutralEfficient.close();
			f_neutralProba1Sigma.close();
			f_neutralProba2Sigma.close();
			f_neutralDifferenceProba1Sigma.close();
			f_neutralDifferenceProba1Sigma.close();
			f_neutralPercentage.close();

			std::stringstream titleStream;
			std::string particleType1Str = particle1.m_type == NEUTRAL ? "neutral" : "charged";
			std::string particleType2Str = particle2.m_type == NEUTRAL ? "neutral" : "charged";
			titleStream << particle1.m_energy << " GeV "
					<< particleType1Str << " , " << particle2.m_energy << " GeV "
					<< particleType2Str;


			DrawAttributeMapping attributeMapping(algorithmType, dataType);


			configureGraph(pNPfosGraph, attributeMapping);
			pNPfosGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["N_PFOS"].second->Add(pNPfosGraph);

			// charged particle configuration
			configureGraph(pChargedPurityGraph, attributeMapping);
			pChargedPurityGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].second->Add(pChargedPurityGraph);

			configureGraph(pChargedEfficiencyGraph, attributeMapping);
			pChargedEfficiencyGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].second->Add(pChargedEfficiencyGraph);

			configureGraph(pChargedEnergyMeanGraph, attributeMapping);
			pChargedEnergyMeanGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_MEAN"].second->Add(pChargedEnergyMeanGraph);

			configureGraph(pChargedEnergyFitGraph, attributeMapping);
			pChargedEnergyFitGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_FIT"].second->Add(pChargedEnergyFitGraph);

			configureGraph(pChargedEnergyDifferenceMeanGraph, attributeMapping);
			pChargedEnergyDifferenceMeanGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_MEAN"].second->Add(pChargedEnergyDifferenceMeanGraph);

			configureGraph(pChargedEnergyDifferenceFitGraph, attributeMapping);
			pChargedEnergyDifferenceFitGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_FIT"].second->Add(pChargedEnergyDifferenceFitGraph);


			// neutral particle configuration
			configureGraph(pNeutralPurityGraph, attributeMapping);
			pNeutralPurityGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].second->Add(pNeutralPurityGraph);

			configureGraph(pNeutralEfficiencyGraph, attributeMapping);
			pNeutralEfficiencyGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].second->Add(pNeutralEfficiencyGraph);

			configureGraph(pNeutralEnergyMeanGraph, attributeMapping);
			pNeutralEnergyMeanGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN"].second->Add(pNeutralEnergyMeanGraph);

			configureGraph(pNeutralEnergyMeanEfficientGraph, attributeMapping);
			pNeutralEnergyMeanEfficientGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN_EFFICIENT"].second->Add(pNeutralEnergyMeanEfficientGraph);

			configureGraph(pNeutralEnergyFitGraph, attributeMapping);
			pNeutralEnergyFitGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_FIT"].second->Add(pNeutralEnergyFitGraph);

			configureGraph(pNeutralEnergyDifferenceMeanGraph, attributeMapping);
			pNeutralEnergyDifferenceMeanGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_MEAN"].second->Add(pNeutralEnergyDifferenceMeanGraph);

			configureGraph(pNeutralEnergyDifferenceFitGraph, attributeMapping);
			pNeutralEnergyDifferenceFitGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_FIT"].second->Add(pNeutralEnergyDifferenceFitGraph);

			configureGraph(pNeutralProbaRecover1SigmaGraph, attributeMapping);
			pNeutralProbaRecover1SigmaGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_1_SIGMA"].second->Add(pNeutralProbaRecover1SigmaGraph);

			configureGraph(pNeutralProbaRecover2SigmaGraph, attributeMapping);
			pNeutralProbaRecover2SigmaGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_2_SIGMA"].second->Add(pNeutralProbaRecover2SigmaGraph);

			configureGraph(pNeutralDifferenceProbaRecover1SigmaGraph, attributeMapping);
			pNeutralDifferenceProbaRecover1SigmaGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_1_SIGMA"].second->Add(pNeutralDifferenceProbaRecover1SigmaGraph);

			configureGraph(pNeutralDifferenceProbaRecover2SigmaGraph, attributeMapping);
			pNeutralDifferenceProbaRecover2SigmaGraph->SetTitle(titleStream.str().c_str());
			pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_2_SIGMA"].second->Add(pNeutralDifferenceProbaRecover2SigmaGraph);

		} // algorithm loop
	} // data loop

	gStyle->SetOptFit(0);

	/*********************/
	//  PLOT EVERYTHING  //
	/*********************/

	// global plots
	pAnalysis->m_canvasMultiGraphMap["N_PFOS"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["N_PFOS"].second, "#N_{pfo}");
	pAnalysis->m_canvasMultiGraphMap["N_PFOS"].second->GetYaxis()->SetRangeUser(0, 3);
	pAnalysis->m_canvasMultiGraphMap["N_PFOS"].first->Update();


	// charged particle plots
	pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].second, "Charged particle purity");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].second->GetYaxis()->SetRangeUser(0, 1);
	pAnalysis->m_canvasMultiGraphMap["CHARGED_PURITY"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].second, "Charged particle efficiency");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].second->GetYaxis()->SetRangeUser(0, 1);
	pAnalysis->m_canvasMultiGraphMap["CHARGED_EFFICIENCY"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_MEAN"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_MEAN"].second, "[MEAN] Mean reconstructed charged energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_MEAN"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_FIT"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_FIT"].second, "[FIT] Mean reconstructed charged energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_FIT"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_MEAN"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_MEAN"].second, "[MEAN] Mean rec-meas charged energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_MEAN"].first->Update();

 	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_FIT"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_FIT"].second, "[FIT] Mean rec-meas charged energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["CHARGED_ENERGY_DIFFERENCE_FIT"].first->Update();


	// neutral particle plots
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].second, "Neutral particle purity");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].second->GetYaxis()->SetRangeUser(0, 1);
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PURITY"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].second, "Neutral particle efficiency");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].second->GetYaxis()->SetRangeUser(0, 1);
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_EFFICIENCY"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN"].second, "[MEAN] Mean reconstructed neutral energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN_EFFICIENT"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN_EFFICIENT"].second, "[MEAN] Mean reconstructed neutral energy when efficient [GeV]");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_MEAN_EFFICIENT"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_FIT"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_FIT"].second, "[FIT] Mean reconstructed neutral energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_FIT"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_MEAN"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_MEAN"].second, "[MEAN] Mean rec-meas neutral energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_MEAN"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_FIT"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_FIT"].second, "[FIT] Mean rec-meas neutral energy [GeV]");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_ENERGY_DIFFERENCE_FIT"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_1_SIGMA"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_1_SIGMA"].second, "Event fraction of neutral energy within 1 Sigma of the real energy");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_1_SIGMA"].second->GetYaxis()->SetRangeUser(0, 1);
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_1_SIGMA"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_2_SIGMA"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_2_SIGMA"].second, "Event fraction of neutral energy within 2 Sigma of the real energy");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_2_SIGMA"].second->GetYaxis()->SetRangeUser(0, 1);
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_PROBA_WITHIN_2_SIGMA"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_1_SIGMA"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_1_SIGMA"].second, "Event fraction of neutral energy within 1 Sigma of the reconstructed energy");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_1_SIGMA"].second->GetYaxis()->SetRangeUser(0, 1);
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_1_SIGMA"].first->Update();

	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_2_SIGMA"].first->cd();
	drawOverlayMultiGraph(pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_2_SIGMA"].second, "Event fraction of neutral energy within 2 Sigma of the reconstructed");
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_2_SIGMA"].second->GetYaxis()->SetRangeUser(0, 1);
	pAnalysis->m_canvasMultiGraphMap["NEUTRAL_DIFFERENCE_PROBA_WITHIN_2_SIGMA"].first->Update();
}

