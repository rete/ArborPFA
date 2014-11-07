#include <TObject.h>
#include <TGraphErrors.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TH2.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TText.h>
#include <TMultiGraph.h>
#include <TLatex.h>

#include <string>
#include <iostream>
#include <map>
#include <string>

#include <OverlayPurityAnalysis.h>

void process_overlay_purity_analysis()
{
	std::string fileDirectory = "/home/remi/git/ArborPFA/output/v01-03-00/Separation/";
	std::string treeName = "PfoMonitoring";
	std::string physicsList = "FTFP_BERT_HP";
	AlgorithmType algoritmType = ARBOR_PFA; // PANDORA_PFA or ARBOR_PFA
	DataType dataType = SIMULATION;   // SIMULATION or TEST_BEAM

	std::vector<int> neutralEnergies;
	std::vector<int> chargedEnergies;

	neutralEnergies.push_back(10);
	chargedEnergies.push_back(10);
	chargedEnergies.push_back(30);

	bool doEfficiencyPurityHistogram = neutralEnergies.size() == 1 && chargedEnergies.size() == 1;
//	bool doRecoveredMeasuredHistogram = chargedEnergies.size() == 1;

	std::vector<OverlayPurityAnalysis*> overlayPurityAnalysisStorage;

	std::vector<int> separationDistance;

	for(unsigned int d=5 ; d<31; d+=5)
		separationDistance.push_back(d);

	CaliceStyle();

	TCanvas *cc1 = new TCanvas("cc1","Mean Purity");
	TCanvas *cc3 = new TCanvas("cc3","Mean Efficiency");
	TCanvas *cc6 = new TCanvas("cc6","Number of PFOs");
	TCanvas *cc7 = NULL;
	TCanvas *cc8 = new TCanvas("cc8","Mean charged energy");
	TCanvas *cc5 = NULL;

	if(doEfficiencyPurityHistogram)
	 cc5 = new TCanvas("cc5","Efficiency / Purity");

//	if(doRecoveredMeasuredHistogram)
//		cc7 = new TCanvas("cc7","Recovered - measured energy");

	CanvasFormat(cc1);
	CanvasFormat(cc3);
	CanvasFormat(cc6);
	CanvasFormat(cc8);

	if(doEfficiencyPurityHistogram)
	 CanvasFormat(cc5);

//	if(doRecoveredMeasuredHistogram)
//			CanvasFormat(cc7);

	TMultiGraph *pMeanPurityChargedMultiGraph = new TMultiGraph();
	TMultiGraph *pMeanEfficiencyChargedMultiGraph = new TMultiGraph();
	TMultiGraph *pNPfosMultiGraph = new TMultiGraph();
	TMultiGraph *pMeanChargedEnergyMultiGraph = new TMultiGraph();

	TGraph *pMeanPurityChargedGraph = NULL;
	TGraph *pMeanEfficiencyChargedGraph = NULL;
	TGraph *pNPfosGraph = NULL;
	TGraph *pMeanChargedEnergyGraph = NULL;
	TH2F *pEfficiencyPurityHistogram = NULL;
//	TH1F *pRecoveredMeasuredEnergyHistogram = NULL;

	int currentColor = 0;

	// start of loop
	for(unsigned int e1=0 ; e1<neutralEnergies.size() ; e1++)
	{
		for(unsigned int e2=0 ; e2<chargedEnergies.size() ; e2++)
		{
			int energy1 = neutralEnergies.at(e1);
			int energy2 = chargedEnergies.at(e2);

			pMeanPurityChargedGraph = new TGraph((int)separationDistance.size());
			pMeanEfficiencyChargedGraph = new TGraph((int)separationDistance.size());
			pNPfosGraph = new TGraph((int)separationDistance.size());
			pMeanChargedEnergyGraph = new TGraph((int)separationDistance.size());

//			if(doRecoveredMeasuredHistogram)
//				pRecoveredMeasuredEnergyHistogram = new TH1F("recoveredMeasuredEnergy", "Recovered - measured energy [GeV]", 101, -50, 50);

			if(doEfficiencyPurityHistogram)
				pEfficiencyPurityHistogram = new TH2F("EfficiencyPurityHistogram", "", 101, 0, 1, 101, 0, 1);

			for(unsigned int d=0 ; d<separationDistance.size() ; d++)
			{
				int distance = separationDistance.at(d);

				const std::string fileName = fileDirectory + BuildOverlayEventFileName(energy1, energy2, distance, algoritmType, dataType);
				TFile *pRootFile = TFile::Open(fileName.c_str());
				TTree *pTree     = (TTree*) pRootFile->Get(treeName.c_str());
				OverlayPurityAnalysis *purityAnalysis = new OverlayPurityAnalysis(pTree);
				purityAnalysis->Loop();

				pMeanPurityChargedGraph->SetPoint(d, distance, purityAnalysis->purityChargedDistribution.mean);
				pMeanEfficiencyChargedGraph->SetPoint(d, distance, purityAnalysis->efficiencyChargedDistribution.mean);
				pNPfosGraph->SetPoint(d, distance, purityAnalysis->nPfosDistribution.mean);
				pMeanChargedEnergyGraph->SetPoint(d, distance, purityAnalysis->energyChargedDistribution.mean);

				if(doEfficiencyPurityHistogram)
				 pEfficiencyPurityHistogram->Fill(purityAnalysis->efficiencyChargedDistribution.mean, purityAnalysis->purityChargedDistribution.mean);

				overlayPurityAnalysisStorage.push_back(purityAnalysis);
			}

			std::stringstream titleStream;
			titleStream << energy1 << " GeV neutral , " << energy2 << " GeV charged";

			pMeanPurityChargedGraph->SetMarkerColor(GraphSettings::commonMarkerColor + currentColor);
			pMeanPurityChargedGraph->SetMarkerStyle(GraphSettings::commonMarkerStyle);
			pMeanPurityChargedGraph->SetLineStyle(GraphSettings::commonLineStyle);
			pMeanPurityChargedGraph->SetLineWidth(GraphSettings::commonLineWidth);
			pMeanPurityChargedGraph->SetLineColor(GraphSettings::commonLineColor + currentColor);
			pMeanPurityChargedGraph->SetFillColor(kWhite);
			pMeanPurityChargedGraph->SetTitle(titleStream.str().c_str());
			pMeanPurityChargedMultiGraph->Add(pMeanPurityChargedGraph);

			pMeanEfficiencyChargedGraph->SetMarkerColor(GraphSettings::commonMarkerColor + currentColor);
			pMeanEfficiencyChargedGraph->SetMarkerStyle(GraphSettings::commonMarkerStyle);
			pMeanEfficiencyChargedGraph->SetLineStyle(GraphSettings::commonLineStyle);
			pMeanEfficiencyChargedGraph->SetLineWidth(GraphSettings::commonLineWidth);
			pMeanEfficiencyChargedGraph->SetLineColor(GraphSettings::commonLineColor + currentColor);
			pMeanEfficiencyChargedGraph->SetFillColor(kWhite);
			pMeanEfficiencyChargedGraph->SetTitle(titleStream.str().c_str());
			pMeanEfficiencyChargedMultiGraph->Add(pMeanEfficiencyChargedGraph);


			if(doEfficiencyPurityHistogram)
			{
				cc5->cd();
				pEfficiencyPurityHistogram->SetTitle("");
				pEfficiencyPurityHistogram->GetXaxis()->SetTitle("Efficiency");
				pEfficiencyPurityHistogram->GetXaxis()->SetRangeUser(0, 1);
				pEfficiencyPurityHistogram->GetYaxis()->SetTitle("Purity");
				pEfficiencyPurityHistogram->GetYaxis()->SetRangeUser(0, 1);
				pEfficiencyPurityHistogram->Draw("colz");
		 }

			pNPfosGraph->SetMarkerColor(GraphSettings::commonMarkerColor + currentColor);
			pNPfosGraph->SetMarkerStyle(GraphSettings::commonMarkerStyle);
			pNPfosGraph->SetLineStyle(GraphSettings::commonLineStyle);
			pNPfosGraph->SetLineWidth(GraphSettings::commonLineWidth);
			pNPfosGraph->SetLineColor(GraphSettings::commonLineColor + currentColor);
			pNPfosGraph->SetFillColor(kWhite);
			pNPfosGraph->SetTitle(titleStream.str().c_str());
			pNPfosMultiGraph->Add(pNPfosGraph);


//			pMeanContaminationGraph->SetMarkerColor(GraphSettings::commonMarkerColor + currentColor);
//			pMeanContaminationGraph->SetMarkerStyle(GraphSettings::commonMarkerStyle);
//			pMeanContaminationGraph->SetLineStyle(GraphSettings::commonLineStyle);
//			pMeanContaminationGraph->SetLineWidth(GraphSettings::commonLineWidth);
//			pMeanContaminationGraph->SetLineColor(GraphSettings::commonLineColor + currentColor);
//			pMeanContaminationGraph->SetFillColor(kWhite);
//			pMeanContaminationGraph->SetTitle(titleStream.str().c_str());
//			pMeanContaminationMultiGraph->Add(pMeanContaminationGraph);


			pMeanChargedEnergyGraph->SetMarkerColor(GraphSettings::commonMarkerColor + currentColor);
			pMeanChargedEnergyGraph->SetMarkerStyle(GraphSettings::commonMarkerStyle);
			pMeanChargedEnergyGraph->SetLineStyle(GraphSettings::commonLineStyle);
			pMeanChargedEnergyGraph->SetLineWidth(GraphSettings::commonLineWidth);
			pMeanChargedEnergyGraph->SetLineColor(GraphSettings::commonLineColor + currentColor);
			pMeanChargedEnergyGraph->SetFillColor(kWhite);
			pMeanChargedEnergyGraph->SetTitle(titleStream.str().c_str());
			pMeanChargedEnergyMultiGraph->Add(pMeanChargedEnergyGraph);

			currentColor++;
		}
	}

	cc1->cd();
	pMeanPurityChargedMultiGraph->Draw("alp");
	pMeanPurityChargedMultiGraph->GetXaxis()->SetTitle("Distance between showers [cm]");
	pMeanPurityChargedMultiGraph->GetXaxis()->SetRangeUser(-1, 31);
	pMeanPurityChargedMultiGraph->GetYaxis()->SetTitle("Purity");
	pMeanPurityChargedMultiGraph->GetYaxis()->SetRangeUser(0, 1);
	pMeanPurityChargedMultiGraph->SetTitle("");
	TLegend *pMeanPurityLegend = cc1->BuildLegend(0.5, 0.6, 0.8, 0.8);
	pMeanPurityLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	pMeanPurityLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	cc1->Update();


	cc3->cd();
	pMeanEfficiencyChargedMultiGraph->Draw("alp");
	pMeanEfficiencyChargedMultiGraph->GetXaxis()->SetTitle("Distance between showers [cm]");
	pMeanEfficiencyChargedMultiGraph->GetXaxis()->SetRangeUser(-1, 31);
	pMeanEfficiencyChargedMultiGraph->GetYaxis()->SetTitle("Efficiency");
	pMeanEfficiencyChargedMultiGraph->GetYaxis()->SetRangeUser(0, 1);
	pMeanEfficiencyChargedMultiGraph->SetTitle("");
	TLegend *pMeanEfficiencyLegend = cc3->BuildLegend(0.5, 0.6, 0.8, 0.8);
	pMeanEfficiencyLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	pMeanEfficiencyLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	cc3->Update();


	cc6->cd();
	pNPfosMultiGraph->Draw("alp");
	pNPfosMultiGraph->GetXaxis()->SetTitle("Distance between showers [cm]");
	pNPfosMultiGraph->GetXaxis()->SetRangeUser(-1, 31);
	pNPfosMultiGraph->GetYaxis()->SetTitle("#N_{pfo}");
	pNPfosMultiGraph->SetMinimum(0);
	pNPfosMultiGraph->SetTitle("");
	TLegend *pNPfoLegend = cc6->BuildLegend(0.5, 0.6, 0.8, 0.8);
	pNPfoLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	pNPfoLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	cc6->Update();


//	cc7->cd();
//	pMeanContaminationMultiGraph->Draw("alp");
//	pMeanContaminationMultiGraph->GetXaxis()->SetTitle("Distance between showers [cm]");
//	pMeanContaminationMultiGraph->GetXaxis()->SetRangeUser(-1, 31);
//	pMeanContaminationMultiGraph->GetYaxis()->SetTitle("Contamination");
//	pMeanContaminationMultiGraph->SetTitle("");
//	TLegend *pMeanContaminationLegend = cc7->BuildLegend(0.5, 0.6, 0.8, 0.8);
//	pMeanContaminationLegend->SetLineColor(GraphSettings::commonLegendLineColor);
//	pMeanContaminationLegend->SetFillColor(GraphSettings::commonLegendFillColor);
//	cc7->Update();


	cc8->cd();
	pMeanChargedEnergyMultiGraph->Draw("alp");
	pMeanChargedEnergyMultiGraph->GetXaxis()->SetTitle("Distance between showers [cm]");
	pMeanChargedEnergyMultiGraph->GetXaxis()->SetRangeUser(-1, 31);
	pMeanChargedEnergyMultiGraph->GetYaxis()->SetTitle("Charged energy");
	pMeanChargedEnergyMultiGraph->SetTitle("");
	TLegend *pMeanChargedEnergyLegend = cc8->BuildLegend(0.5, 0.6, 0.8, 0.8);
	LegendFormat(pMeanChargedEnergyLegend);
	cc8->Update();

	cc6->WaitPrimitive();

	for( unsigned int i=0 ; i<overlayPurityAnalysisStorage.size() ; i++ )
		delete overlayPurityAnalysisStorage.at(i);
	overlayPurityAnalysisStorage.clear();
}

