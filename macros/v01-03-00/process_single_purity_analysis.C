#include <TObject.h>
#include <TGraphErrors.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TH2.h>
#include <TF1.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TText.h>
#include <TMultiGraph.h>
#include <TLatex.h>

#include <string>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility>

#include <SinglePurityAnalysis.h>

void process_single_purity_analysis()
{
	std::string fileDirectory = "/home/remi/git/ArborPFA/output/v01-03-00/SingleParticle/newCuts/";
	std::string physicsList = "FTFP_BERT_HP";

	std::map<AlgorithmType, std::string> algorithmToTreeNameMap;
	algorithmToTreeNameMap[PANDORA_PFA] = "PfoMonitoring";
	algorithmToTreeNameMap[ARBOR_PFA] = "PfoMonitoring";

	std::vector< std::pair<AlgorithmType, DataType> > algorithmAndDataToProcess;
//	algorithmAndDataToProcess.push_back( std::pair<AlgorithmType, DataType>(PANDORA_PFA, SIMULATION) );
	algorithmAndDataToProcess.push_back( std::pair<AlgorithmType, DataType>(PANDORA_PFA, TEST_BEAM) );
//	algorithmAndDataToProcess.push_back( std::pair<AlgorithmType, DataType>(ARBOR_PFA, SIMULATION) );
	algorithmAndDataToProcess.push_back( std::pair<AlgorithmType, DataType>(ARBOR_PFA, TEST_BEAM) );

	std::vector<int> energies;
	energies.push_back(10);
	energies.push_back(20);
	energies.push_back(30);
	energies.push_back(40);
	energies.push_back(50);
	energies.push_back(60);
	energies.push_back(70);
	energies.push_back(80);

	std::vector<SinglePurityAnalysis*> singlePurityAnalysisStorage;

	CaliceStyle();

	TCanvas *cc1 = new TCanvas("cc1","Mean Purity");
	TCanvas *cc2 = new TCanvas("cc2","Number of PFOs");
	TCanvas *cc3 = new TCanvas("cc3","Mean charged energy");
	TCanvas *cc4 = new TCanvas("cc4","Energy resolution");

	CanvasFormat(cc1);
	CanvasFormat(cc2);
	CanvasFormat(cc3);
	CanvasFormat(cc4);

	TMultiGraph *pMeanPurityChargedMultiGraph = new TMultiGraph("MeanPurityChargedMultiGraph", "Efficiency");
	TMultiGraph *pNPfosMultiGraph = new TMultiGraph("NPfosMultiGraph", "N Pfos");
	TMultiGraph *pMeanChargedEnergyMultiGraph = new TMultiGraph("MeanChargedEnergyMultiGraph", "Reconstructed Energy [GeV]");
	TMultiGraph *pEnergyResolutionMultiGraph = new TMultiGraph();

	for(unsigned int a=0 ; a<algorithmAndDataToProcess.size() ; a++)
	{
		TGraph *pMeanPurityChargedGraph = new TGraph((int)energies.size());
		TGraph *pNPfosGraph = new TGraph((int)energies.size());
		TGraphErrors *pMeanChargedEnergyGraph = new TGraphErrors((int)energies.size());
		TGraphErrors *pEnergyResolutionGraph = new TGraphErrors((int)energies.size());

		AlgorithmType algoritmType = algorithmAndDataToProcess.at(a).first;
		DataType dataType = algorithmAndDataToProcess.at(a).second;
		std::string treeName = algorithmToTreeNameMap[algoritmType];

		// start of loop
		for(unsigned int e=0 ; e<energies.size() ; e++)
		{
			int energy = energies.at(e);

			const std::string fileName = fileDirectory + BuildSingleParticleFileName(energy, algoritmType, dataType);
			TFile *pRootFile = TFile::Open(fileName.c_str());
			TTree *pTree     = (TTree*) pRootFile->Get(treeName.c_str());
			SinglePurityAnalysis *purityAnalysis = new SinglePurityAnalysis(pTree);
			purityAnalysis->Loop();

			pMeanPurityChargedGraph->SetPoint(e, energy, purityAnalysis->purityChargedDistribution.mean);
			pNPfosGraph->SetPoint(e, energy, purityAnalysis->nPfosDistribution.mean);

			int min = 0;
			int max = energy + 30;
			TH1 *pHistogram = createHistogram("histo", "histo", max-min, min, max, purityAnalysis->energyChargedDistribution);

			TF1 *pGaus = new TF1("gausFunc","gaus", min, max);
			pHistogram->Fit(pGaus, "NQ", "");
			pHistogram->Fit(pGaus, "NQ", "", pGaus->GetParameter(1)-1.5*pGaus->GetParameter(2), pGaus->GetParameter(1)+1.5*pGaus->GetParameter(2));

			pMeanChargedEnergyGraph->SetPoint(e, energy, pGaus->GetParameter(1));//purityAnalysis->energyChargedDistribution.mean);
			pMeanChargedEnergyGraph->SetPointError(e, 0, pGaus->GetParError(1));

			pEnergyResolutionGraph->SetPoint(e, energy, pGaus->GetParameter(2)/pGaus->GetParameter(1));
			double error = std::sqrt(std::pow(pGaus->GetParError(2)/pGaus->GetParameter(1), 2)
			                        + std::pow((pGaus->GetParameter(2)*pGaus->GetParError(1))/(pGaus->GetParameter(1)*pGaus->GetParameter(1)), 2));
			pEnergyResolutionGraph->SetPointError(e, 0, error);

			singlePurityAnalysisStorage.push_back(purityAnalysis);
		}

		std::stringstream ss;
		ss << AlgorithmName(algoritmType) << " - " << DataName(dataType);
		GraphInfo graphInfo(algoritmType, dataType);

		pMeanPurityChargedGraph->SetMarkerColor(graphInfo.m_markerColor);
		pMeanPurityChargedGraph->SetMarkerStyle(graphInfo.m_markerStyle);
		pMeanPurityChargedGraph->SetLineStyle(1);//graphInfo.m_lineStyle);
		pMeanPurityChargedGraph->SetLineWidth(graphInfo.m_lineWidth);
		pMeanPurityChargedGraph->SetLineColor(graphInfo.m_lineColor);
		pMeanPurityChargedGraph->SetFillColor(kWhite);
		pMeanPurityChargedGraph->SetTitle(ss.str().c_str());
		pMeanPurityChargedMultiGraph->Add(pMeanPurityChargedGraph);

		pNPfosGraph->SetMarkerColor(graphInfo.m_markerColor);
		pNPfosGraph->SetMarkerStyle(graphInfo.m_markerStyle);
		pNPfosGraph->SetLineStyle(graphInfo.m_lineStyle);
		pNPfosGraph->SetLineWidth(graphInfo.m_lineWidth);
		pNPfosGraph->SetLineColor(graphInfo.m_lineColor);
		pNPfosGraph->SetFillColor(kWhite);
		pNPfosGraph->SetTitle(ss.str().c_str());
		pNPfosMultiGraph->Add(pNPfosGraph);

		pMeanChargedEnergyGraph->SetMarkerColor(graphInfo.m_markerColor);
		pMeanChargedEnergyGraph->SetMarkerStyle(graphInfo.m_markerStyle);
		pMeanChargedEnergyGraph->SetLineStyle(1);//graphInfo.m_lineStyle);
		pMeanChargedEnergyGraph->SetLineWidth(graphInfo.m_lineWidth);
		pMeanChargedEnergyGraph->SetLineColor(graphInfo.m_lineColor);
		pMeanChargedEnergyGraph->SetFillColor(kWhite);
		pMeanChargedEnergyGraph->SetTitle(ss.str().c_str());
		pMeanChargedEnergyMultiGraph->Add(pMeanChargedEnergyGraph);

		pEnergyResolutionGraph->SetMarkerColor(graphInfo.m_markerColor);
		pEnergyResolutionGraph->SetMarkerStyle(graphInfo.m_markerStyle);
		pEnergyResolutionGraph->SetLineStyle(graphInfo.m_lineStyle);
		pEnergyResolutionGraph->SetLineWidth(graphInfo.m_lineWidth);
		pEnergyResolutionGraph->SetLineColor(graphInfo.m_lineColor);
		pEnergyResolutionGraph->SetFillColor(kWhite);
		pEnergyResolutionGraph->SetTitle(ss.str().c_str());
		pEnergyResolutionMultiGraph->Add(pEnergyResolutionGraph);
	}

	cc1->cd();
	pMeanPurityChargedMultiGraph->Draw("ap");
	pMeanPurityChargedMultiGraph->GetXaxis()->SetTitle("E_{beam} [GeV]");
	pMeanPurityChargedMultiGraph->GetXaxis()->SetRangeUser(0, energies.back()+10);
	pMeanPurityChargedMultiGraph->GetYaxis()->SetTitle("Efficiency");
	pMeanPurityChargedMultiGraph->GetYaxis()->SetRangeUser(0, 1);
	LegendFormat(cc1->BuildLegend());
	cc1->Update();

	cc2->cd();
	pNPfosMultiGraph->Draw("ap");
	pNPfosMultiGraph->GetXaxis()->SetTitle("E_{beam} [GeV]");
	pNPfosMultiGraph->GetXaxis()->SetRangeUser(0, energies.back()+10);
	pNPfosMultiGraph->GetYaxis()->SetTitle("N_{pfo}");
	LegendFormat(cc2->BuildLegend());
	cc2->Update();

	cc3->cd();
	pMeanChargedEnergyMultiGraph->Draw("ap");
	pMeanChargedEnergyMultiGraph->GetXaxis()->SetTitle("E_{beam} [GeV]");
	pMeanChargedEnergyMultiGraph->GetXaxis()->SetRangeUser(0, energies.back()+10);
	pMeanChargedEnergyMultiGraph->GetYaxis()->SetTitle("Reconstructed energy");
	pMeanChargedEnergyMultiGraph->GetYaxis()->SetRangeUser(0, energies.back()+10);
	LegendFormat(cc3->BuildLegend());
	TF1 *pLin = new TF1("lin", "x", 0, 90);
	pLin->SetLineColor(kGray);
	pLin->SetLineWidth(2);
	pLin->Draw("same");
	cc3->Update();

	cc4->cd();
	pEnergyResolutionMultiGraph->Draw("ap");
	pEnergyResolutionMultiGraph->GetXaxis()->SetTitle("E_{beam} [GeV]");
	pEnergyResolutionMultiGraph->GetXaxis()->SetRangeUser(0, energies.back()+10);
	pEnergyResolutionMultiGraph->GetYaxis()->SetTitle("#frac{#sigma_{E}}{E}");
	pEnergyResolutionMultiGraph->GetYaxis()->SetRangeUser(0, 0.4);
	LegendFormat(cc4->BuildLegend());
	cc4->Update();

	cc1->WaitPrimitive();

	for( unsigned int i=0 ; i<singlePurityAnalysisStorage.size() ; i++ )
		delete singlePurityAnalysisStorage.at(i);
	singlePurityAnalysisStorage.clear();
}

