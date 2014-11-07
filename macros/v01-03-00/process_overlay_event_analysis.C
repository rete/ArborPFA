#include <TObject.h>
#include <TGraphErrors.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TH2.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TText.h>

#include <string>
#include <iostream>
#include <map>
#include <string>

#include <OverlayEventPFOAnalysis.h>

void process_overlay_event_analysis(int energy1 = 10, int energy2 = 10)
{
	std::string fileDirectory = "/home/remi/git/ArborPFA/output/OverlayEvent/";

	std::string treeName = "PFOAnalysis";
	std::string physicsList = "FTFP_BERT_HP";

	std::vector<OverlayEventPFOAnalysis*> overlayEventPFOAnalysisStorage;
	std::vector<TObject*> rootObjectStorage;

	std::vector<int> separationDistance;
	separationDistance.push_back(5);
	separationDistance.push_back(10);
	separationDistance.push_back(15);
	separationDistance.push_back(20);
	separationDistance.push_back(25);
	separationDistance.push_back(30);

	CaliceStyle();

	TCanvas *cc1 = new TCanvas("cc1","Algorithm Efficiency (1 Ch + 1 Neu)");
	TCanvas *cc2 = new TCanvas("cc2","Algorithm Efficiency (1 Ch + n Neu)");
	TCanvas *cc3 = new TCanvas("cc3","Estimated - Reconstructed charged energy");
	TCanvas *cc4 = new TCanvas("cc4","Estimated - Reconstructed neutral energy");
	TCanvas *cc5 = new TCanvas("cc5","Number of PFOs");
	CanvasFormat(cc1);
	CanvasFormat(cc2);
	CanvasFormat(cc3);
	CanvasFormat(cc4);
	CanvasFormat(cc5);

	TGraph *arborSimGraphEfficiencyChargedNeutral = new TGraph((int)separationDistance.size());
	TGraph *arborTBGraphEfficiencyChargedNeutral = new TGraph((int)separationDistance.size());
	TGraph *pandoraSimGraphEfficiencyChargedNeutral = new TGraph((int)separationDistance.size());
	TGraph *pandoraTBGraphEfficiencyChargedNeutral = new TGraph((int)separationDistance.size());

	TGraph *arborSimGraphEfficiencyChargedNeutralAndAdditional = new TGraph((int)separationDistance.size());
	TGraph *arborTBGraphEfficiencyChargedNeutralAndAdditional = new TGraph((int)separationDistance.size());
	TGraph *pandoraSimGraphEfficiencyChargedNeutralAndAdditional = new TGraph((int)separationDistance.size());
	TGraph *pandoraTBGraphEfficiencyChargedNeutralAndAdditional = new TGraph((int)separationDistance.size());

	TGraph *arborSimGraphChargedEnergyDifference = new TGraph((int)separationDistance.size());
	TGraph *arborTBGraphChargedEnergyDifference = new TGraph((int)separationDistance.size());
	TGraph *pandoraSimGraphChargedEnergyDifference = new TGraph((int)separationDistance.size());
	TGraph *pandoraTBGraphChargedEnergyDifference = new TGraph((int)separationDistance.size());

	TGraph *arborSimGraphNeutralEnergyDifference = new TGraph((int)separationDistance.size());
	TGraph *arborTBGraphNeutralEnergyDifference = new TGraph((int)separationDistance.size());
	TGraph *pandoraSimGraphNeutralEnergyDifference = new TGraph((int)separationDistance.size());
	TGraph *pandoraTBGraphNeutralEnergyDifference = new TGraph((int)separationDistance.size());

	TGraph *arborSimGraphNPfos = new TGraph((int)separationDistance.size());
	TGraph *arborTBGraphNPfos = new TGraph((int)separationDistance.size());
	TGraph *pandoraSimGraphNPfos = new TGraph((int)separationDistance.size());
	TGraph *pandoraTBGraphNPfos = new TGraph((int)separationDistance.size());

	rootObjectStorage.push_back(cc1);
	rootObjectStorage.push_back(cc2);

	rootObjectStorage.push_back(arborSimGraphEfficiencyChargedNeutral);
	rootObjectStorage.push_back(arborTBGraphEfficiencyChargedNeutral);
	rootObjectStorage.push_back(pandoraSimGraphEfficiencyChargedNeutral);
	rootObjectStorage.push_back(pandoraTBGraphEfficiencyChargedNeutral);

	rootObjectStorage.push_back(arborSimGraphEfficiencyChargedNeutralAndAdditional);
	rootObjectStorage.push_back(arborTBGraphEfficiencyChargedNeutralAndAdditional);
	rootObjectStorage.push_back(pandoraSimGraphEfficiencyChargedNeutralAndAdditional);
	rootObjectStorage.push_back(pandoraTBGraphEfficiencyChargedNeutralAndAdditional);

	rootObjectStorage.push_back(arborSimGraphChargedEnergyDifference);
	rootObjectStorage.push_back(arborTBGraphChargedEnergyDifference);
	rootObjectStorage.push_back(pandoraSimGraphChargedEnergyDifference);
	rootObjectStorage.push_back(pandoraTBGraphChargedEnergyDifference);

	rootObjectStorage.push_back(arborSimGraphNeutralEnergyDifference);
	rootObjectStorage.push_back(arborTBGraphNeutralEnergyDifference);
	rootObjectStorage.push_back(pandoraSimGraphNeutralEnergyDifference);
	rootObjectStorage.push_back(pandoraTBGraphNeutralEnergyDifference);

	rootObjectStorage.push_back(arborSimGraphNPfos);
	rootObjectStorage.push_back(arborTBGraphNPfos);
	rootObjectStorage.push_back(pandoraSimGraphNPfos);
	rootObjectStorage.push_back(pandoraTBGraphNPfos);

	for(unsigned int d=0 ; d<separationDistance.size() ; d++) {

		int distance = separationDistance.at(d);

		std::string arborSimFileName   = fileDirectory + BuildOverlayEventFileName(energy1, energy2, distance, ARBOR_PFA, SIMULATION);
		TFile *arborSimRootFile = TFile::Open(arborSimFileName.c_str());
		TTree *arborSimTree     = (TTree*) arborSimRootFile->Get(treeName.c_str());
		OverlayEventPFOAnalysis *arborSimAnalysis = new OverlayEventPFOAnalysis(arborSimTree);
		arborSimAnalysis->Loop();

		std::string arborTBFileName    = fileDirectory + BuildOverlayEventFileName(energy1, energy2, distance, ARBOR_PFA, TEST_BEAM);
		TFile *arborTBRootFile = TFile::Open(arborTBFileName.c_str());
		TTree *arborTBTree     = (TTree*) arborTBRootFile->Get(treeName.c_str());
		OverlayEventPFOAnalysis *arborTBAnalysis = new OverlayEventPFOAnalysis(arborTBTree);
		arborTBAnalysis->Loop();

		std::string pandoraSimFileName = fileDirectory + BuildOverlayEventFileName(energy1, energy2, distance, PANDORA_PFA, SIMULATION);
		TFile *pandoraSimRootFile = TFile::Open(pandoraSimFileName.c_str());
		TTree *pandoraSimTree     = (TTree*) pandoraSimRootFile->Get(treeName.c_str());
		OverlayEventPFOAnalysis *pandoraSimAnalysis = new OverlayEventPFOAnalysis(pandoraSimTree);
		pandoraSimAnalysis->Loop();

		std::string pandoraTBFileName  = fileDirectory + BuildOverlayEventFileName(energy1, energy2, distance, PANDORA_PFA, TEST_BEAM);
		TFile *pandoraTBRootFile = TFile::Open(pandoraTBFileName.c_str());
		TTree *pandoraTBTree     = (TTree*) pandoraTBRootFile->Get(treeName.c_str());
		OverlayEventPFOAnalysis *pandoraTBAnalysis = new OverlayEventPFOAnalysis(pandoraTBTree);
		pandoraTBAnalysis->Loop();

		arborSimGraphEfficiencyChargedNeutral->SetPoint(d, distance, arborSimAnalysis->efficiencyChargedNeutral.mean);
		arborSimGraphEfficiencyChargedNeutralAndAdditional->SetPoint(d, distance, arborSimAnalysis->efficiencyChargedNeutralAndAdditional.mean);
		arborSimGraphChargedEnergyDifference->SetPoint(d, distance, arborSimAnalysis->totalMinusChargedParticleEnergyDistribution.mean);
		arborSimGraphNeutralEnergyDifference->SetPoint(d, distance, arborSimAnalysis->totalMinusNeutralParticleEnergyDistribution.mean);
		arborSimGraphNPfos->SetPoint(d, distance, arborSimAnalysis->nPfosDistribution.mean);

		arborTBGraphEfficiencyChargedNeutral->SetPoint(d, distance, arborTBAnalysis->efficiencyChargedNeutral.mean);
		arborTBGraphEfficiencyChargedNeutralAndAdditional->SetPoint(d, distance, arborTBAnalysis->efficiencyChargedNeutralAndAdditional.mean);
		arborTBGraphChargedEnergyDifference->SetPoint(d, distance, arborTBAnalysis->totalMinusChargedParticleEnergyDistribution.mean);
		arborTBGraphNeutralEnergyDifference->SetPoint(d, distance, arborTBAnalysis->totalMinusNeutralParticleEnergyDistribution.mean);
		arborTBGraphNPfos->SetPoint(d, distance, arborTBAnalysis->nPfosDistribution.mean);

		pandoraSimGraphEfficiencyChargedNeutral->SetPoint(d, distance, pandoraSimAnalysis->efficiencyChargedNeutral.mean);
		pandoraSimGraphEfficiencyChargedNeutralAndAdditional->SetPoint(d, distance, pandoraSimAnalysis->efficiencyChargedNeutralAndAdditional.mean);
		pandoraSimGraphChargedEnergyDifference->SetPoint(d, distance, pandoraSimAnalysis->totalMinusChargedParticleEnergyDistribution.mean);
		pandoraSimGraphNeutralEnergyDifference->SetPoint(d, distance, pandoraSimAnalysis->totalMinusNeutralParticleEnergyDistribution.mean);
		pandoraSimGraphNPfos->SetPoint(d, distance, pandoraSimAnalysis->nPfosDistribution.mean);

		pandoraTBGraphEfficiencyChargedNeutral->SetPoint(d, distance, pandoraTBAnalysis->efficiencyChargedNeutral.mean);
		pandoraTBGraphEfficiencyChargedNeutralAndAdditional->SetPoint(d, distance, pandoraTBAnalysis->efficiencyChargedNeutralAndAdditional.mean);
		pandoraTBGraphChargedEnergyDifference->SetPoint(d, distance, pandoraTBAnalysis->totalMinusChargedParticleEnergyDistribution.mean);
		pandoraTBGraphNeutralEnergyDifference->SetPoint(d, distance, pandoraTBAnalysis->totalMinusNeutralParticleEnergyDistribution.mean);
		pandoraTBGraphNPfos->SetPoint(d, distance, pandoraTBAnalysis->nPfosDistribution.mean);

		overlayEventPFOAnalysisStorage.push_back(arborSimAnalysis);
		overlayEventPFOAnalysisStorage.push_back(arborTBAnalysis);
		overlayEventPFOAnalysisStorage.push_back(pandoraSimAnalysis);
		overlayEventPFOAnalysisStorage.push_back(pandoraTBAnalysis);
	}

	// code couleur :
	//    ArborPFA et sim   : rouge continue
	//    ArborPFA et TB    : rouge discontinue
	//    PandoraPFA et sim : bleu continue
	//    PandoraPFA et TB  : bleu discontinue

	// common to all plots
	std::stringstream textStrStream;
	textStrStream << energy1 << " GeV neutral + " << energy2 << " Gev charged";
	TPaveText *energiesText = CreateText(textStrStream.str());
	energiesText->SetTextSize(0.025);
	energiesText->SetTextColor(kBlack);

	cc1->cd();
	cc1->SetGrid();
	GraphFormat(arborSimGraphEfficiencyChargedNeutral);
	GraphFormat(arborTBGraphEfficiencyChargedNeutral);
	GraphFormat(pandoraSimGraphEfficiencyChargedNeutral);
	GraphFormat(pandoraTBGraphEfficiencyChargedNeutral);
	float rangeXMinEfficiencyChargedNeutral = 0.f;
	float rangeXMaxEfficiencyChargedNeutral = 35.f;
	float rangeYMinEfficiencyChargedNeutral = 0.f;
	float rangeYMaxEfficiencyChargedNeutral = 1.f;
	TLegend *efficiencyChargedNeutralLegend = new TLegend(0.2,0.2,0.4,0.3);
	efficiencyChargedNeutralLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	efficiencyChargedNeutralLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	rootObjectStorage.push_back(efficiencyChargedNeutralLegend);
	TPaveText *preliminaryEfficiencyChargedNeutral = CreateText("VERY PRELIMINARY");

	arborSimGraphEfficiencyChargedNeutral->SetMarkerColor(GraphSettings::arborSimMarkerColor);
	arborSimGraphEfficiencyChargedNeutral->SetMarkerStyle(GraphSettings::arborSimMarkerStyle);
	arborSimGraphEfficiencyChargedNeutral->SetTitle("");
	arborSimGraphEfficiencyChargedNeutral->SetLineStyle(GraphSettings::arborSimLineStyle);
	arborSimGraphEfficiencyChargedNeutral->SetLineWidth(GraphSettings::commonLineWidth);
	arborSimGraphEfficiencyChargedNeutral->SetLineColor(GraphSettings::arborSimLineColor);
	arborSimGraphEfficiencyChargedNeutral->GetXaxis()->SetTitle("Distance between showers [cm]");
	arborSimGraphEfficiencyChargedNeutral->GetXaxis()->SetRangeUser(rangeXMinEfficiencyChargedNeutral, rangeXMaxEfficiencyChargedNeutral);
	arborSimGraphEfficiencyChargedNeutral->GetYaxis()->SetTitle("Efficiency (N_{ch} = 1 && N_{n} = 1)");
	arborSimGraphEfficiencyChargedNeutral->GetYaxis()->SetRangeUser(rangeYMinEfficiencyChargedNeutral, rangeYMaxEfficiencyChargedNeutral);
	efficiencyChargedNeutralLegend->AddEntry(arborSimGraphEfficiencyChargedNeutral, (std::string("ArborPFA ")+physicsList).c_str(), "lp");
	arborSimGraphEfficiencyChargedNeutral->Draw("alp");

	arborTBGraphEfficiencyChargedNeutral->SetMarkerColor(GraphSettings::arborTBMarkerColor);
	arborTBGraphEfficiencyChargedNeutral->SetMarkerStyle(GraphSettings::arborTBMarkerStyle);
	arborTBGraphEfficiencyChargedNeutral->SetTitle("");
	arborTBGraphEfficiencyChargedNeutral->SetLineStyle(GraphSettings::arborTBLineStyle);
	arborTBGraphEfficiencyChargedNeutral->SetLineWidth(GraphSettings::commonLineWidth);
	arborTBGraphEfficiencyChargedNeutral->SetLineColor(GraphSettings::arborTBLineColor);
	arborTBGraphEfficiencyChargedNeutral->GetXaxis()->SetTitle("");
	arborTBGraphEfficiencyChargedNeutral->GetYaxis()->SetRangeUser(rangeXMinEfficiencyChargedNeutral, rangeXMaxEfficiencyChargedNeutral);
	arborTBGraphEfficiencyChargedNeutral->GetYaxis()->SetTitle("");
	arborTBGraphEfficiencyChargedNeutral->GetYaxis()->SetRangeUser(rangeYMinEfficiencyChargedNeutral, rangeYMaxEfficiencyChargedNeutral);
	efficiencyChargedNeutralLegend->AddEntry(arborTBGraphEfficiencyChargedNeutral, std::string("ArborPFA SDHCAL data").c_str(), "lp");
	arborTBGraphEfficiencyChargedNeutral->Draw("lp same");

	pandoraSimGraphEfficiencyChargedNeutral->SetMarkerColor(GraphSettings::pandoraSimMarkerColor);
	pandoraSimGraphEfficiencyChargedNeutral->SetMarkerStyle(GraphSettings::pandoraSimMarkerStyle);
	pandoraSimGraphEfficiencyChargedNeutral->SetTitle("");
	pandoraSimGraphEfficiencyChargedNeutral->SetLineStyle(GraphSettings::pandoraSimLineStyle);
	pandoraSimGraphEfficiencyChargedNeutral->SetLineColor(GraphSettings::pandoraSimLineColor);
	pandoraSimGraphEfficiencyChargedNeutral->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraSimGraphEfficiencyChargedNeutral->GetXaxis()->SetTitle("");
	pandoraSimGraphEfficiencyChargedNeutral->GetXaxis()->SetRangeUser(rangeXMinEfficiencyChargedNeutral, rangeXMaxEfficiencyChargedNeutral);
	pandoraSimGraphEfficiencyChargedNeutral->GetYaxis()->SetTitle("");
	pandoraSimGraphEfficiencyChargedNeutral->GetYaxis()->SetRangeUser(rangeYMinEfficiencyChargedNeutral, rangeYMaxEfficiencyChargedNeutral);
	efficiencyChargedNeutralLegend->AddEntry(pandoraSimGraphEfficiencyChargedNeutral, (std::string("PandoraPFA ")+physicsList).c_str(), "lp");
	pandoraSimGraphEfficiencyChargedNeutral->Draw("lp same");

	pandoraTBGraphEfficiencyChargedNeutral->SetMarkerColor(GraphSettings::pandoraTBMarkerColor);
	pandoraTBGraphEfficiencyChargedNeutral->SetMarkerStyle(GraphSettings::pandoraTBMarkerStyle);
	pandoraTBGraphEfficiencyChargedNeutral->SetTitle("");
	pandoraTBGraphEfficiencyChargedNeutral->SetLineStyle(GraphSettings::pandoraTBLineStyle);
	pandoraTBGraphEfficiencyChargedNeutral->SetLineColor(GraphSettings::pandoraTBLineColor);
	pandoraTBGraphEfficiencyChargedNeutral->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraTBGraphEfficiencyChargedNeutral->GetXaxis()->SetTitle("");
	pandoraTBGraphEfficiencyChargedNeutral->GetXaxis()->SetRangeUser(rangeXMinEfficiencyChargedNeutral, rangeXMaxEfficiencyChargedNeutral);
	pandoraTBGraphEfficiencyChargedNeutral->GetYaxis()->SetTitle("");
	pandoraTBGraphEfficiencyChargedNeutral->GetYaxis()->SetRangeUser(rangeYMinEfficiencyChargedNeutral, rangeYMaxEfficiencyChargedNeutral);
	efficiencyChargedNeutralLegend->AddEntry(pandoraTBGraphEfficiencyChargedNeutral, std::string("PandoraPFA SDHCAL data").c_str(), "lp");
	pandoraTBGraphEfficiencyChargedNeutral->Draw("lp same");

	efficiencyChargedNeutralLegend->Draw();
	preliminaryEfficiencyChargedNeutral->Draw();
	energiesText->Draw();


	cc2->cd();
	cc2->SetGrid();
	GraphFormat(arborSimGraphEfficiencyChargedNeutralAndAdditional);
	GraphFormat(arborTBGraphEfficiencyChargedNeutralAndAdditional);
	GraphFormat(pandoraSimGraphEfficiencyChargedNeutralAndAdditional);
	GraphFormat(pandoraTBGraphEfficiencyChargedNeutralAndAdditional);
	float rangeXMinEfficiencyChargedNeutralAndAdditional = 0.f;
	float rangeXMaxEfficiencyChargedNeutralAndAdditional = 35.f;
	float rangeYMinEfficiencyChargedNeutralAndAdditional = 0.f;
	float rangeYMaxEfficiencyChargedNeutralAndAdditional = 1.f;
	TLegend *efficiencyChargedNeutralAndAdditionalLegend = new TLegend(0.2,0.2,0.4,0.3);
	efficiencyChargedNeutralAndAdditionalLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	efficiencyChargedNeutralAndAdditionalLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	rootObjectStorage.push_back(efficiencyChargedNeutralLegend);
	TPaveText *preliminaryEfficiencyChargedNeutralAndAdditional = CreateText("VERY PRELIMINARY");

	arborSimGraphEfficiencyChargedNeutralAndAdditional->SetMarkerColor(GraphSettings::arborSimMarkerColor);
	arborSimGraphEfficiencyChargedNeutralAndAdditional->SetMarkerStyle(GraphSettings::arborSimMarkerStyle);
	arborSimGraphEfficiencyChargedNeutralAndAdditional->SetTitle("");
	arborSimGraphEfficiencyChargedNeutralAndAdditional->SetLineStyle(GraphSettings::arborSimLineStyle);
	arborSimGraphEfficiencyChargedNeutralAndAdditional->SetLineWidth(GraphSettings::commonLineWidth);
	arborSimGraphEfficiencyChargedNeutralAndAdditional->SetLineColor(GraphSettings::arborSimLineColor);
	arborSimGraphEfficiencyChargedNeutralAndAdditional->GetXaxis()->SetTitle("Distance between showers [cm]");
	arborSimGraphEfficiencyChargedNeutralAndAdditional->GetXaxis()->SetRangeUser(rangeXMinEfficiencyChargedNeutralAndAdditional, rangeXMaxEfficiencyChargedNeutralAndAdditional);
	arborSimGraphEfficiencyChargedNeutralAndAdditional->GetYaxis()->SetTitle("Efficiency (N_{ch} = 1 && N_{n} #geq 1)");
	arborSimGraphEfficiencyChargedNeutralAndAdditional->GetYaxis()->SetRangeUser(rangeYMinEfficiencyChargedNeutralAndAdditional, rangeYMaxEfficiencyChargedNeutralAndAdditional);
	efficiencyChargedNeutralAndAdditionalLegend->AddEntry(arborSimGraphEfficiencyChargedNeutralAndAdditional, (std::string("ArborPFA ")+physicsList).c_str(), "lp");
	arborSimGraphEfficiencyChargedNeutralAndAdditional->Draw("alp");

	arborTBGraphEfficiencyChargedNeutralAndAdditional->SetMarkerColor(GraphSettings::arborTBMarkerColor);
	arborTBGraphEfficiencyChargedNeutralAndAdditional->SetMarkerStyle(GraphSettings::arborTBMarkerStyle);
	arborTBGraphEfficiencyChargedNeutralAndAdditional->SetTitle("");
	arborTBGraphEfficiencyChargedNeutralAndAdditional->SetLineStyle(GraphSettings::arborTBLineStyle);
	arborTBGraphEfficiencyChargedNeutralAndAdditional->SetLineWidth(GraphSettings::commonLineWidth);
	arborTBGraphEfficiencyChargedNeutralAndAdditional->SetLineColor(GraphSettings::arborTBLineColor);
	arborTBGraphEfficiencyChargedNeutralAndAdditional->GetXaxis()->SetTitle("Distance between showers [cm]");
	arborTBGraphEfficiencyChargedNeutralAndAdditional->GetYaxis()->SetRangeUser(rangeXMinEfficiencyChargedNeutralAndAdditional, rangeXMaxEfficiencyChargedNeutralAndAdditional);
	arborTBGraphEfficiencyChargedNeutralAndAdditional->GetYaxis()->SetTitle("Efficiency (N_{ch} = 1 && N_{n} #geq 1)");
	arborTBGraphEfficiencyChargedNeutralAndAdditional->GetYaxis()->SetRangeUser(rangeYMinEfficiencyChargedNeutralAndAdditional, rangeYMaxEfficiencyChargedNeutralAndAdditional);
	efficiencyChargedNeutralAndAdditionalLegend->AddEntry(arborTBGraphEfficiencyChargedNeutralAndAdditional, std::string("ArborPFA SDHCAL data").c_str(), "lp");
	arborTBGraphEfficiencyChargedNeutralAndAdditional->Draw("lp same");

	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->SetMarkerColor(GraphSettings::pandoraSimMarkerColor);
	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->SetMarkerStyle(GraphSettings::pandoraSimMarkerStyle);
	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->SetTitle("");
	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->SetLineStyle(GraphSettings::pandoraSimLineStyle);
	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->SetLineColor(GraphSettings::pandoraSimLineColor);
	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->GetXaxis()->SetTitle("Distance between showers [cm]");
	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->GetXaxis()->SetRangeUser(rangeXMinEfficiencyChargedNeutralAndAdditional, rangeXMaxEfficiencyChargedNeutralAndAdditional);
	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->GetYaxis()->SetTitle("Efficiency (N_{ch} = 1 && N_{n} #geq 1)");
	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->GetYaxis()->SetRangeUser(rangeYMinEfficiencyChargedNeutralAndAdditional, rangeYMaxEfficiencyChargedNeutralAndAdditional);
	efficiencyChargedNeutralAndAdditionalLegend->AddEntry(pandoraSimGraphEfficiencyChargedNeutralAndAdditional, (std::string("PandoraPFA ")+physicsList).c_str(), "lp");
	pandoraSimGraphEfficiencyChargedNeutralAndAdditional->Draw("lp same");

	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->SetMarkerColor(GraphSettings::pandoraTBMarkerColor);
	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->SetMarkerStyle(GraphSettings::pandoraTBMarkerStyle);
	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->SetTitle("");
	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->SetLineStyle(GraphSettings::pandoraTBLineStyle);
	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->SetLineColor(GraphSettings::pandoraTBLineColor);
	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->GetXaxis()->SetTitle("Distance between showers [cm]");
	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->GetXaxis()->SetRangeUser(rangeXMinEfficiencyChargedNeutralAndAdditional, rangeXMaxEfficiencyChargedNeutralAndAdditional);
	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->GetYaxis()->SetTitle("Efficiency (N_{ch} = 1 && N_{n} #geq 1)");
	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->GetYaxis()->SetRangeUser(rangeYMinEfficiencyChargedNeutralAndAdditional, rangeYMaxEfficiencyChargedNeutralAndAdditional);
	efficiencyChargedNeutralAndAdditionalLegend->AddEntry(pandoraTBGraphEfficiencyChargedNeutralAndAdditional, std::string("PandoraPFA SDHCAL data").c_str(), "lp");
	pandoraTBGraphEfficiencyChargedNeutralAndAdditional->Draw("lp same");

	efficiencyChargedNeutralAndAdditionalLegend->Draw();
	preliminaryEfficiencyChargedNeutralAndAdditional->Draw();
	energiesText->Draw();

	cc3->cd();
	cc3->SetGrid();
	GraphFormat(arborSimGraphChargedEnergyDifference);
	GraphFormat(arborTBGraphChargedEnergyDifference);
	GraphFormat(pandoraSimGraphChargedEnergyDifference);
	GraphFormat(pandoraTBGraphChargedEnergyDifference);
	float rangeXMinChargedEnergy = 0.f;
	float rangeXMaxChargedEnergy = 35.f;
	float rangeYMinChargedEnergy = -20.f;
	float rangeYMaxChargedEnergy = 20.f;
	TLegend *chargedEnergyDifferenceLegend = new TLegend(0.2,0.2,0.4,0.3);
	chargedEnergyDifferenceLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	chargedEnergyDifferenceLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	rootObjectStorage.push_back(chargedEnergyDifferenceLegend);
	TPaveText *preliminaryChargedEnergyDifference = CreateText("VERY PRELIMINARY");

	arborSimGraphChargedEnergyDifference->SetMarkerColor(GraphSettings::arborSimMarkerColor);
	arborSimGraphChargedEnergyDifference->SetMarkerStyle(GraphSettings::arborSimMarkerStyle);
	arborSimGraphChargedEnergyDifference->SetTitle("");
	arborSimGraphChargedEnergyDifference->SetLineStyle(GraphSettings::arborSimLineStyle);
	arborSimGraphChargedEnergyDifference->SetLineWidth(GraphSettings::commonLineWidth);
	arborSimGraphChargedEnergyDifference->SetLineColor(GraphSettings::arborSimLineColor);
	arborSimGraphChargedEnergyDifference->GetXaxis()->SetTitle("Distance between showers [cm]");
	arborSimGraphChargedEnergyDifference->GetXaxis()->SetRangeUser(rangeXMinChargedEnergy, rangeXMaxChargedEnergy);
	arborSimGraphChargedEnergyDifference->GetYaxis()->SetTitle("Mean estimated - reconstructed energy (charged) [GeV]");
	arborSimGraphChargedEnergyDifference->GetYaxis()->SetRangeUser(rangeYMinChargedEnergy, rangeYMaxChargedEnergy);
	chargedEnergyDifferenceLegend->AddEntry(arborSimGraphChargedEnergyDifference, (std::string("ArborPFA ")+physicsList).c_str(), "lp");
	arborSimGraphChargedEnergyDifference->Draw("alp");

	arborTBGraphChargedEnergyDifference->SetMarkerColor(GraphSettings::arborTBMarkerColor);
	arborTBGraphChargedEnergyDifference->SetMarkerStyle(GraphSettings::arborTBMarkerStyle);
	arborTBGraphChargedEnergyDifference->SetTitle("");
	arborTBGraphChargedEnergyDifference->SetLineStyle(GraphSettings::arborTBLineStyle);
	arborTBGraphChargedEnergyDifference->SetLineWidth(GraphSettings::commonLineWidth);
	arborTBGraphChargedEnergyDifference->SetLineColor(GraphSettings::arborTBLineColor);
	arborTBGraphChargedEnergyDifference->GetXaxis()->SetTitle("");
	arborTBGraphChargedEnergyDifference->GetYaxis()->SetRangeUser(rangeXMinChargedEnergy, rangeXMaxChargedEnergy);
	arborTBGraphChargedEnergyDifference->GetYaxis()->SetTitle("");
	arborTBGraphChargedEnergyDifference->GetYaxis()->SetRangeUser(rangeYMinChargedEnergy, rangeYMaxChargedEnergy);
	chargedEnergyDifferenceLegend->AddEntry(arborTBGraphChargedEnergyDifference, std::string("ArborPFA SDHCAL data").c_str(), "lp");
	arborTBGraphChargedEnergyDifference->Draw("lp same");

	pandoraSimGraphChargedEnergyDifference->SetMarkerColor(GraphSettings::pandoraSimMarkerColor);
	pandoraSimGraphChargedEnergyDifference->SetMarkerStyle(GraphSettings::pandoraSimMarkerStyle);
	pandoraSimGraphChargedEnergyDifference->SetTitle("");
	pandoraSimGraphChargedEnergyDifference->SetLineStyle(GraphSettings::pandoraSimLineStyle);
	pandoraSimGraphChargedEnergyDifference->SetLineColor(GraphSettings::pandoraSimLineColor);
	pandoraSimGraphChargedEnergyDifference->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraSimGraphChargedEnergyDifference->GetXaxis()->SetTitle("");
	pandoraSimGraphChargedEnergyDifference->GetXaxis()->SetRangeUser(rangeXMinChargedEnergy, rangeXMaxChargedEnergy);
	pandoraSimGraphChargedEnergyDifference->GetYaxis()->SetTitle("");
	pandoraSimGraphChargedEnergyDifference->GetYaxis()->SetRangeUser(rangeYMinChargedEnergy, rangeYMaxChargedEnergy);
	chargedEnergyDifferenceLegend->AddEntry(pandoraSimGraphChargedEnergyDifference, (std::string("PandoraPFA ")+physicsList).c_str(), "lp");
	pandoraSimGraphChargedEnergyDifference->Draw("lp same");

	pandoraTBGraphChargedEnergyDifference->SetMarkerColor(GraphSettings::pandoraTBMarkerColor);
	pandoraTBGraphChargedEnergyDifference->SetMarkerStyle(GraphSettings::pandoraTBMarkerStyle);
	pandoraTBGraphChargedEnergyDifference->SetTitle("");
	pandoraTBGraphChargedEnergyDifference->SetLineStyle(GraphSettings::pandoraTBLineStyle);
	pandoraTBGraphChargedEnergyDifference->SetLineColor(GraphSettings::pandoraTBLineColor);
	pandoraTBGraphChargedEnergyDifference->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraTBGraphChargedEnergyDifference->GetXaxis()->SetTitle("");
	pandoraTBGraphChargedEnergyDifference->GetXaxis()->SetRangeUser(rangeXMinChargedEnergy, rangeXMaxChargedEnergy);
	pandoraTBGraphChargedEnergyDifference->GetYaxis()->SetTitle("");
	pandoraTBGraphChargedEnergyDifference->GetYaxis()->SetRangeUser(rangeYMinChargedEnergy, rangeYMaxChargedEnergy);
	chargedEnergyDifferenceLegend->AddEntry(pandoraTBGraphChargedEnergyDifference, std::string("PandoraPFA SDHCAL data").c_str(), "lp");
	pandoraTBGraphChargedEnergyDifference->Draw("lp same");

	chargedEnergyDifferenceLegend->Draw();
	preliminaryChargedEnergyDifference->Draw();
	energiesText->Draw();

	cc4->cd();
	cc4->SetGrid();
	GraphFormat(arborSimGraphNeutralEnergyDifference);
	GraphFormat(arborTBGraphNeutralEnergyDifference);
	GraphFormat(pandoraSimGraphNeutralEnergyDifference);
	GraphFormat(pandoraTBGraphNeutralEnergyDifference);
	float rangeXMinTotalMinusNeutral = 0.f;
	float rangeXMaxTotalMinusNeutral = 35.f;
	float rangeYMinTotalMinusNeutral = -20.f;
	float rangeYMaxTotalMinusNeutral = 20.f;
	TLegend *neutralEnergyDifferenceLegend = new TLegend(0.2,0.2,0.4,0.3);
	neutralEnergyDifferenceLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	neutralEnergyDifferenceLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	rootObjectStorage.push_back(neutralEnergyDifferenceLegend);
	TPaveText *preliminaryNeutralEnergyDifference = CreateText("VERY PRELIMINARY");

	arborSimGraphNeutralEnergyDifference->SetMarkerColor(GraphSettings::arborSimMarkerColor);
	arborSimGraphNeutralEnergyDifference->SetMarkerStyle(GraphSettings::arborSimMarkerStyle);
	arborSimGraphNeutralEnergyDifference->SetTitle("");
	arborSimGraphNeutralEnergyDifference->SetLineStyle(GraphSettings::arborSimLineStyle);
	arborSimGraphNeutralEnergyDifference->SetLineWidth(GraphSettings::commonLineWidth);
	arborSimGraphNeutralEnergyDifference->SetLineColor(GraphSettings::arborSimLineColor);
	arborSimGraphNeutralEnergyDifference->GetXaxis()->SetTitle("Distance between showers [cm]");
	arborSimGraphNeutralEnergyDifference->GetXaxis()->SetRangeUser(rangeXMinTotalMinusNeutral, rangeXMaxTotalMinusNeutral);
	arborSimGraphNeutralEnergyDifference->GetYaxis()->SetTitle("Mean estimated - reconstructed energy (neutral) [GeV]");
	arborSimGraphNeutralEnergyDifference->GetYaxis()->SetRangeUser(rangeYMinTotalMinusNeutral, rangeYMaxTotalMinusNeutral);
	neutralEnergyDifferenceLegend->AddEntry(arborSimGraphNeutralEnergyDifference, (std::string("ArborPFA ")+physicsList).c_str(), "lp");
	arborSimGraphNeutralEnergyDifference->Draw("alp");

	arborTBGraphNeutralEnergyDifference->SetMarkerColor(GraphSettings::arborTBMarkerColor);
	arborTBGraphNeutralEnergyDifference->SetMarkerStyle(GraphSettings::arborTBMarkerStyle);
	arborTBGraphNeutralEnergyDifference->SetTitle("");
	arborTBGraphNeutralEnergyDifference->SetLineStyle(GraphSettings::arborTBLineStyle);
	arborTBGraphNeutralEnergyDifference->SetLineWidth(GraphSettings::commonLineWidth);
	arborTBGraphNeutralEnergyDifference->SetLineColor(GraphSettings::arborTBLineColor);
	arborTBGraphNeutralEnergyDifference->GetXaxis()->SetTitle("");
	arborTBGraphNeutralEnergyDifference->GetYaxis()->SetRangeUser(rangeXMinTotalMinusNeutral, rangeXMaxTotalMinusNeutral);
	arborTBGraphNeutralEnergyDifference->GetYaxis()->SetTitle("");
	arborTBGraphNeutralEnergyDifference->GetYaxis()->SetRangeUser(rangeYMinTotalMinusNeutral, rangeYMaxTotalMinusNeutral);
	neutralEnergyDifferenceLegend->AddEntry(arborTBGraphNeutralEnergyDifference, std::string("ArborPFA SDHCAL data").c_str(), "lp");
	arborTBGraphNeutralEnergyDifference->Draw("lp same");

	pandoraSimGraphNeutralEnergyDifference->SetMarkerColor(GraphSettings::pandoraSimMarkerColor);
	pandoraSimGraphNeutralEnergyDifference->SetMarkerStyle(GraphSettings::pandoraSimMarkerStyle);
	pandoraSimGraphNeutralEnergyDifference->SetTitle("");
	pandoraSimGraphNeutralEnergyDifference->SetLineStyle(GraphSettings::pandoraSimLineStyle);
	pandoraSimGraphNeutralEnergyDifference->SetLineColor(GraphSettings::pandoraSimLineColor);
	pandoraSimGraphNeutralEnergyDifference->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraSimGraphNeutralEnergyDifference->GetXaxis()->SetTitle("");
	pandoraSimGraphNeutralEnergyDifference->GetXaxis()->SetRangeUser(rangeXMinTotalMinusNeutral, rangeXMaxTotalMinusNeutral);
	pandoraSimGraphNeutralEnergyDifference->GetYaxis()->SetTitle("");
	pandoraSimGraphNeutralEnergyDifference->GetYaxis()->SetRangeUser(rangeYMinTotalMinusNeutral, rangeYMaxTotalMinusNeutral);
	neutralEnergyDifferenceLegend->AddEntry(pandoraSimGraphNeutralEnergyDifference, (std::string("PandoraPFA ")+physicsList).c_str(), "lp");
	pandoraSimGraphNeutralEnergyDifference->Draw("lp same");

	pandoraTBGraphNeutralEnergyDifference->SetMarkerColor(GraphSettings::pandoraTBMarkerColor);
	pandoraTBGraphNeutralEnergyDifference->SetMarkerStyle(GraphSettings::pandoraTBMarkerStyle);
	pandoraTBGraphNeutralEnergyDifference->SetTitle("");
	pandoraTBGraphNeutralEnergyDifference->SetLineStyle(GraphSettings::pandoraTBLineStyle);
	pandoraTBGraphNeutralEnergyDifference->SetLineColor(GraphSettings::pandoraTBLineColor);
	pandoraTBGraphNeutralEnergyDifference->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraTBGraphNeutralEnergyDifference->GetXaxis()->SetTitle("");
	pandoraTBGraphNeutralEnergyDifference->GetXaxis()->SetRangeUser(rangeXMinTotalMinusNeutral, rangeXMaxTotalMinusNeutral);
	pandoraTBGraphNeutralEnergyDifference->GetYaxis()->SetTitle("");
	pandoraTBGraphNeutralEnergyDifference->GetYaxis()->SetRangeUser(rangeYMinTotalMinusNeutral, rangeYMaxTotalMinusNeutral);
	neutralEnergyDifferenceLegend->AddEntry(pandoraTBGraphNeutralEnergyDifference, std::string("PandoraPFA SDHCAL data").c_str(), "lp");
	pandoraTBGraphNeutralEnergyDifference->Draw("lp same");

	neutralEnergyDifferenceLegend->Draw();
	preliminaryNeutralEnergyDifference->Draw();
	energiesText->Draw();


	cc5->cd();
	cc5->SetGrid();
	GraphFormat(arborSimGraphNPfos);
	GraphFormat(arborTBGraphNPfos);
	GraphFormat(pandoraSimGraphNPfos);
	GraphFormat(pandoraTBGraphNPfos);
	float rangeXMinNPfos = 0.f;
	float rangeXMaxNPfos = 35.f;
	float rangeYMinNPfos = 0.f;
	float rangeYMaxNPfos = 5.f;
	TLegend *nPfosLegend = new TLegend(0.2,0.2,0.4,0.3);
	nPfosLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	nPfosLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	rootObjectStorage.push_back(nPfosLegend);
	TPaveText *preliminaryNPfos = CreateText("VERY PRELIMINARY");
	rootObjectStorage.push_back(preliminaryNPfos);

	arborSimGraphNPfos->SetMarkerColor(GraphSettings::arborSimMarkerColor);
	arborSimGraphNPfos->SetMarkerStyle(GraphSettings::arborSimMarkerStyle);
	arborSimGraphNPfos->SetTitle("");
	arborSimGraphNPfos->SetLineStyle(GraphSettings::arborSimLineStyle);
	arborSimGraphNPfos->SetLineWidth(GraphSettings::commonLineWidth);
	arborSimGraphNPfos->SetLineColor(GraphSettings::arborSimLineColor);
	arborSimGraphNPfos->GetXaxis()->SetTitle("Distance between showers [cm]");
	arborSimGraphNPfos->GetXaxis()->SetRangeUser(rangeXMinNPfos, rangeXMaxNPfos);
	arborSimGraphNPfos->GetYaxis()->SetTitle("Mean N_{pfo}");
	arborSimGraphNPfos->GetYaxis()->SetRangeUser(rangeYMinNPfos, rangeYMaxNPfos);
	nPfosLegend->AddEntry(arborSimGraphNPfos, (std::string("ArborPFA ")+physicsList).c_str(), "lp");
	arborSimGraphNPfos->Draw("alp");

	arborTBGraphNPfos->SetMarkerColor(GraphSettings::arborTBMarkerColor);
	arborTBGraphNPfos->SetMarkerStyle(GraphSettings::arborTBMarkerStyle);
	arborTBGraphNPfos->SetTitle("");
	arborTBGraphNPfos->SetLineStyle(GraphSettings::arborTBLineStyle);
	arborTBGraphNPfos->SetLineWidth(GraphSettings::commonLineWidth);
	arborTBGraphNPfos->SetLineColor(GraphSettings::arborTBLineColor);
	arborTBGraphNPfos->GetXaxis()->SetTitle("Energy [GeV]");
	arborTBGraphNPfos->GetYaxis()->SetRangeUser(rangeXMinNPfos, rangeXMaxNPfos);
	arborTBGraphNPfos->GetYaxis()->SetTitle("");
	arborTBGraphNPfos->GetYaxis()->SetRangeUser(rangeYMinNPfos, rangeYMaxNPfos);
	nPfosLegend->AddEntry(arborTBGraphNPfos, std::string("ArborPFA SDHCAL data").c_str(), "lp");
	arborTBGraphNPfos->Draw("lp same");

	pandoraSimGraphNPfos->SetMarkerColor(GraphSettings::pandoraSimMarkerColor);
	pandoraSimGraphNPfos->SetMarkerStyle(GraphSettings::pandoraSimMarkerStyle);
	pandoraSimGraphNPfos->SetTitle("");
	pandoraSimGraphNPfos->SetLineStyle(GraphSettings::pandoraSimLineStyle);
	pandoraSimGraphNPfos->SetLineColor(GraphSettings::pandoraSimLineColor);
	pandoraSimGraphNPfos->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraSimGraphNPfos->GetXaxis()->SetTitle("Energy [GeV]");
	pandoraSimGraphNPfos->GetXaxis()->SetRangeUser(rangeXMinNPfos, rangeXMaxNPfos);
	pandoraSimGraphNPfos->GetYaxis()->SetTitle("");
	pandoraSimGraphNPfos->GetYaxis()->SetRangeUser(rangeYMinNPfos, rangeYMaxNPfos);
	nPfosLegend->AddEntry(pandoraSimGraphNPfos, (std::string("PandoraPFA ")+physicsList).c_str(), "lp");
	pandoraSimGraphNPfos->Draw("lp same");

	pandoraTBGraphNPfos->SetMarkerColor(GraphSettings::pandoraTBMarkerColor);
	pandoraTBGraphNPfos->SetMarkerStyle(GraphSettings::pandoraTBMarkerStyle);
	pandoraTBGraphNPfos->SetTitle("");
	pandoraTBGraphNPfos->SetLineStyle(GraphSettings::pandoraTBLineStyle);
	pandoraTBGraphNPfos->SetLineColor(GraphSettings::pandoraTBLineColor);
	pandoraTBGraphNPfos->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraTBGraphNPfos->GetXaxis()->SetTitle("Energy [GeV]");
	pandoraTBGraphNPfos->GetXaxis()->SetRangeUser(rangeXMinNPfos, rangeXMaxNPfos);
	pandoraTBGraphNPfos->GetYaxis()->SetTitle("");
	pandoraTBGraphNPfos->GetYaxis()->SetRangeUser(rangeYMinNPfos, rangeYMaxNPfos);
	nPfosLegend->AddEntry(pandoraTBGraphNPfos, std::string("PandoraPFA SDHCAL data").c_str(), "lp");
	pandoraTBGraphNPfos->Draw("lp same");

	nPfosLegend->Draw();
	preliminaryNPfos->Draw();
	energiesText->Draw();

//	cc1->WaitPrimitive();
//	cc2->WaitPrimitive();
	cc5->WaitPrimitive();

	for( unsigned int i=0 ; i<overlayEventPFOAnalysisStorage.size() ; i++ )
		delete overlayEventPFOAnalysisStorage.at(i);
	overlayEventPFOAnalysisStorage.clear();

	for( unsigned int i=0 ; i<rootObjectStorage.size() ; i++ )
		delete rootObjectStorage.at(i);
	rootObjectStorage.clear();

}
