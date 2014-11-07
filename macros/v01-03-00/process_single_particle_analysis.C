

#include <TObject.h>
#include <TGraphErrors.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TH2.h>
#include <TH2D.h>
#include <TMultiGraph.h>
#include <TLegend.h>

#include <string>
#include <iostream>
#include <map>
#include <string>

#include <SingleParticlePFOAnalysis.h>

void process_single_particle_analysis()
{
	std::string fileDirectory = "/home/remi/git/ArborPFA/output/SingleParticle/";
	std::string treeName = "PFOAnalysis";
	std::string physicsList = "FTFP_BERT_HP";

	std::vector<SingleParticlePFOAnalysis*> singleParticlePFOAnalysisStorage;
	std::vector<TObject*> rootObjectStorage;

	std::vector<int> energies;
	energies.push_back(10);
	energies.push_back(20);
	energies.push_back(30);
	energies.push_back(40);
	energies.push_back(50);
	energies.push_back(60);
	energies.push_back(70);
	energies.push_back(80);

	CaliceStyle();

	TCanvas *cc1 = new TCanvas("cc1","Algorithm Efficiency");
	TCanvas *cc2 = new TCanvas("cc2","Mean charged energy");
	TCanvas *cc3 = new TCanvas("cc3","Total - charged energy");
	TCanvas *cc4 = new TCanvas("cc4","Number of PFOs");
	CanvasFormat(cc1);
	CanvasFormat(cc2);
	CanvasFormat(cc3);
	CanvasFormat(cc4);

//	TGraphErrors *arborSimGraphEfficiency = new TGraphErrors((int)energies.size());
//	TGraphErrors *arborTBGraphEfficiency = new TGraphErrors((int)energies.size());
//	TGraphErrors *pandoraSimGraphEfficiency = new TGraphErrors((int)energies.size());
//	TGraphErrors *pandoraTBGraphEfficiency = new TGraphErrors((int)energies.size());

	TGraphErrors *arborSimGraphEfficiency = new TGraphErrors((int)energies.size());
	TGraphErrors *arborTBGraphEfficiency = new TGraphErrors((int)energies.size());
	TGraphErrors *pandoraSimGraphEfficiency = new TGraphErrors((int)energies.size());
	TGraphErrors *pandoraTBGraphEfficiency = new TGraphErrors((int)energies.size());

	TGraphErrors *arborSimGraphChargedEnergy = new TGraphErrors((int)energies.size());
	TGraphErrors *arborTBGraphChargedEnergy = new TGraphErrors((int)energies.size());
	TGraphErrors *pandoraSimGraphChargedEnergy = new TGraphErrors((int)energies.size());
	TGraphErrors *pandoraTBGraphChargedEnergy = new TGraphErrors((int)energies.size());

	TGraphErrors *arborSimGraphTotalMinusCharged = new TGraphErrors((int)energies.size());
	TGraphErrors *arborTBGraphTotalMinusCharged = new TGraphErrors((int)energies.size());
	TGraphErrors *pandoraSimGraphTotalMinusCharged = new TGraphErrors((int)energies.size());
	TGraphErrors *pandoraTBGraphTotalMinusCharged = new TGraphErrors((int)energies.size());

	TGraphErrors *arborSimGraphNPfos = new TGraphErrors((int)energies.size());
	TGraphErrors *arborTBGraphNPfos = new TGraphErrors((int)energies.size());
	TGraphErrors *pandoraSimGraphNPfos = new TGraphErrors((int)energies.size());
	TGraphErrors *pandoraTBGraphNPfos = new TGraphErrors((int)energies.size());

	rootObjectStorage.push_back(cc1);
	rootObjectStorage.push_back(cc2);

	rootObjectStorage.push_back(arborSimGraphEfficiency);
	rootObjectStorage.push_back(arborTBGraphEfficiency);
	rootObjectStorage.push_back(pandoraSimGraphEfficiency);
	rootObjectStorage.push_back(pandoraTBGraphEfficiency);

	rootObjectStorage.push_back(arborSimGraphChargedEnergy);
	rootObjectStorage.push_back(arborTBGraphChargedEnergy);
	rootObjectStorage.push_back(pandoraSimGraphChargedEnergy);
	rootObjectStorage.push_back(pandoraTBGraphChargedEnergy);

	rootObjectStorage.push_back(arborSimGraphTotalMinusCharged);
	rootObjectStorage.push_back(arborTBGraphTotalMinusCharged);
	rootObjectStorage.push_back(pandoraSimGraphTotalMinusCharged);
	rootObjectStorage.push_back(pandoraTBGraphTotalMinusCharged);

	rootObjectStorage.push_back(arborSimGraphNPfos);
	rootObjectStorage.push_back(arborTBGraphNPfos);
	rootObjectStorage.push_back(pandoraSimGraphNPfos);
	rootObjectStorage.push_back(pandoraTBGraphNPfos);

	for(unsigned int e=0 ; e<energies.size() ; e++) {

		int energy = energies.at(e);

		std::string arborSimFileName   = fileDirectory + BuildSingleParticleFileName(energy, ARBOR_PFA, SIMULATION);
		TFile *arborSimRootFile = TFile::Open(arborSimFileName.c_str());
		TTree *arborSimTree     = (TTree*) arborSimRootFile->Get(treeName.c_str());
		SingleParticlePFOAnalysis *arborSimAnalysis = new SingleParticlePFOAnalysis(arborSimTree);
		arborSimAnalysis->Loop();

		std::string arborTBFileName    = fileDirectory + BuildSingleParticleFileName(energy, ARBOR_PFA, TEST_BEAM);
		TFile *arborTBRootFile = TFile::Open(arborTBFileName.c_str());
		TTree *arborTBTree     = (TTree*) arborTBRootFile->Get(treeName.c_str());
		SingleParticlePFOAnalysis *arborTBAnalysis = new SingleParticlePFOAnalysis(arborTBTree);
		arborTBAnalysis->Loop();

		std::string pandoraSimFileName = fileDirectory + BuildSingleParticleFileName(energy, PANDORA_PFA, SIMULATION);
		TFile *pandoraSimRootFile = TFile::Open(pandoraSimFileName.c_str());
		TTree *pandoraSimTree     = (TTree*) pandoraSimRootFile->Get(treeName.c_str());
		SingleParticlePFOAnalysis *pandoraSimAnalysis = new SingleParticlePFOAnalysis(pandoraSimTree);
		pandoraSimAnalysis->Loop();

		std::string pandoraTBFileName  = fileDirectory + BuildSingleParticleFileName(energy, PANDORA_PFA, TEST_BEAM);
		TFile *pandoraTBRootFile = TFile::Open(pandoraTBFileName.c_str());
		TTree *pandoraTBTree     = (TTree*) pandoraTBRootFile->Get(treeName.c_str());
		SingleParticlePFOAnalysis *pandoraTBAnalysis = new SingleParticlePFOAnalysis(pandoraTBTree);
		pandoraTBAnalysis->Loop();

		// arbor sim
		arborSimGraphEfficiency->SetPoint(e, energy, arborSimAnalysis->efficiencyDistribution.mean);
//		arborSimGraphEfficiency->SetPointError(e, 0, arborSimAnalysis->efficiencyDistribution.error);

		arborSimGraphChargedEnergy->SetPoint(e, energy, arborSimAnalysis->chargedEnergyDistribution.mean);
//		arborSimGraphChargedEnergy->SetPointError(e, 0, arborSimAnalysis->chargedEnergyDistribution.rms);

		arborSimGraphTotalMinusCharged->SetPoint(e, energy, arborSimAnalysis->totalMinusChargedDistribution.mean);
//		arborSimGraphTotalMinusCharged->SetPointError(e, 0, arborSimAnalysis->totalMinusChargedDistribution.rms);

		arborSimGraphNPfos->SetPoint(e, energy, arborSimAnalysis->nPfosDistribution.mean);

		// arbor TB
		arborTBGraphEfficiency->SetPoint(e, energy, arborTBAnalysis->efficiencyDistribution.mean);
//		arborTBGraphEfficiency->SetPointError(e, 0, arborTBAnalysis->efficiencyDistribution.error);

		arborTBGraphChargedEnergy->SetPoint(e, energy, arborTBAnalysis->chargedEnergyDistribution.mean);
//		arborTBGraphChargedEnergy->SetPointError(e, 0, arborTBAnalysis->chargedEnergyDistribution.rms);

		arborTBGraphTotalMinusCharged->SetPoint(e, energy, arborTBAnalysis->totalMinusChargedDistribution.mean);
		arborTBGraphNPfos->SetPoint(e, energy, arborTBAnalysis->nPfosDistribution.mean);

		// pandora sim
		pandoraSimGraphEfficiency->SetPoint(e, energy, pandoraSimAnalysis->efficiencyDistribution.mean);
//		pandoraSimGraphEfficiency->SetPointError(e, 0, pandoraSimAnalysis->efficiencyDistribution.error);

		pandoraSimGraphChargedEnergy->SetPoint(e, energy, pandoraSimAnalysis->chargedEnergyDistribution.mean);
//		pandoraSimGraphChargedEnergy->SetPointError(e, 0, pandoraSimAnalysis->chargedEnergyDistribution.rms);

		pandoraSimGraphTotalMinusCharged->SetPoint(e, energy, pandoraSimAnalysis->totalMinusChargedDistribution.mean);
		pandoraSimGraphNPfos->SetPoint(e, energy, pandoraSimAnalysis->nPfosDistribution.mean);

		// pandora TB
		pandoraTBGraphEfficiency->SetPoint(e, energy, pandoraTBAnalysis->efficiencyDistribution.mean);
//		pandoraTBGraphEfficiency->SetPointError(e, 0, pandoraTBAnalysis->efficiencyDistribution.error);

		pandoraTBGraphChargedEnergy->SetPoint(e, energy, pandoraTBAnalysis->chargedEnergyDistribution.mean);
//		pandoraTBGraphChargedEnergy->SetPointError(e, 0, pandoraTBAnalysis->chargedEnergyDistribution.rms);

		pandoraTBGraphTotalMinusCharged->SetPoint(e, energy, pandoraTBAnalysis->totalMinusChargedDistribution.mean);
		pandoraTBGraphNPfos->SetPoint(e, energy, pandoraTBAnalysis->nPfosDistribution.mean);

		singleParticlePFOAnalysisStorage.push_back(arborSimAnalysis);
		singleParticlePFOAnalysisStorage.push_back(arborTBAnalysis);
		singleParticlePFOAnalysisStorage.push_back(pandoraSimAnalysis);
		singleParticlePFOAnalysisStorage.push_back(pandoraTBAnalysis);
	}

	// code couleur :
	//    ArborPFA et sim   : rouge continue
	//    ArborPFA et TB    : rouge discontinue
	//    PandoraPFA et sim : bleu continue
	//    PandoraPFA et TB  : bleu discontinue

	cc1->cd();
	cc1->SetGrid();
	GraphFormat(arborSimGraphEfficiency);
	GraphFormat(arborTBGraphEfficiency);
	GraphFormat(pandoraSimGraphEfficiency);
	GraphFormat(pandoraTBGraphEfficiency);
	float rangeXMin = 0.f;
	float rangeXMax = 90.f;
	float rangeYMin = 0.f;
	float rangeYMax = 1.f;
	TLegend *efficiencyLegend = new TLegend(0.2,0.2,0.4,0.3);
	efficiencyLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	efficiencyLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	rootObjectStorage.push_back(efficiencyLegend);
	TPaveText *preliminaryEfficiency = CreateText("VERY PRELIMINARY");
	rootObjectStorage.push_back(preliminaryEfficiency);

	arborSimGraphEfficiency->SetMarkerColor(GraphSettings::arborSimMarkerColor);
	arborSimGraphEfficiency->SetMarkerStyle(GraphSettings::arborSimMarkerStyle);
	arborSimGraphEfficiency->SetTitle("");
	arborSimGraphEfficiency->SetLineStyle(GraphSettings::arborSimLineStyle);
	arborSimGraphEfficiency->SetLineWidth(GraphSettings::commonLineWidth);
	arborSimGraphEfficiency->SetLineColor(GraphSettings::arborSimLineColor);
	arborSimGraphEfficiency->GetXaxis()->SetTitle("Energy [GeV]");
	arborSimGraphEfficiency->GetXaxis()->SetRangeUser(rangeXMin, rangeXMax);
	arborSimGraphEfficiency->GetYaxis()->SetTitle("Efficiency (N_{ch} = 1)");
	arborSimGraphEfficiency->GetYaxis()->SetRangeUser(rangeYMin, rangeYMax);
	efficiencyLegend->AddEntry(arborSimGraphEfficiency,(std::string("ArborPFA ")+physicsList).c_str(),"lp");
	arborSimGraphEfficiency->Draw("alp");

	arborTBGraphEfficiency->SetMarkerColor(GraphSettings::arborTBMarkerColor);
	arborTBGraphEfficiency->SetMarkerStyle(GraphSettings::arborTBMarkerStyle);
	arborTBGraphEfficiency->SetTitle("");
	arborTBGraphEfficiency->SetLineStyle(GraphSettings::arborTBLineStyle);
	arborTBGraphEfficiency->SetLineWidth(GraphSettings::commonLineWidth);
	arborTBGraphEfficiency->SetLineColor(GraphSettings::arborTBLineColor);
	arborTBGraphEfficiency->GetXaxis()->SetTitle("Energy [GeV]");
	arborTBGraphEfficiency->GetYaxis()->SetRangeUser(rangeXMin, rangeXMax);
	arborTBGraphEfficiency->GetYaxis()->SetTitle("");
	arborTBGraphEfficiency->GetYaxis()->SetRangeUser(rangeYMin, rangeYMax);
	efficiencyLegend->AddEntry(arborTBGraphEfficiency,std::string("ArborPFA SDHCAL data").c_str(),"lp");
	arborTBGraphEfficiency->Draw("lp same");

	pandoraSimGraphEfficiency->SetMarkerColor(GraphSettings::pandoraSimMarkerColor);
	pandoraSimGraphEfficiency->SetMarkerStyle(GraphSettings::pandoraSimMarkerStyle);
	pandoraSimGraphEfficiency->SetTitle("");
	pandoraSimGraphEfficiency->SetLineStyle(GraphSettings::pandoraSimLineStyle);
	pandoraSimGraphEfficiency->SetLineColor(GraphSettings::pandoraSimLineColor);
	pandoraSimGraphEfficiency->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraSimGraphEfficiency->GetXaxis()->SetTitle("Energy [GeV]");
	pandoraSimGraphEfficiency->GetXaxis()->SetRangeUser(rangeXMin, rangeXMax);
	pandoraSimGraphEfficiency->GetYaxis()->SetTitle("");
	pandoraSimGraphEfficiency->GetYaxis()->SetRangeUser(rangeYMin, rangeYMax);
	efficiencyLegend->AddEntry(pandoraSimGraphEfficiency,(std::string("PandoraPFA ")+physicsList).c_str(),"lp");
	pandoraSimGraphEfficiency->Draw("lp same");

	pandoraTBGraphEfficiency->SetMarkerColor(GraphSettings::pandoraTBMarkerColor);
	pandoraTBGraphEfficiency->SetMarkerStyle(GraphSettings::pandoraTBMarkerStyle);
	pandoraTBGraphEfficiency->SetTitle("");
	pandoraTBGraphEfficiency->SetLineStyle(GraphSettings::pandoraTBLineStyle);
	pandoraTBGraphEfficiency->SetLineColor(GraphSettings::pandoraTBLineColor);
	pandoraTBGraphEfficiency->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraTBGraphEfficiency->GetXaxis()->SetTitle("Energy [GeV]");
	pandoraTBGraphEfficiency->GetXaxis()->SetRangeUser(rangeXMin, rangeXMax);
	pandoraTBGraphEfficiency->GetYaxis()->SetTitle("");
	pandoraTBGraphEfficiency->GetYaxis()->SetRangeUser(rangeYMin, rangeYMax);
	efficiencyLegend->AddEntry(pandoraTBGraphEfficiency,std::string("PandoraPFA SDHCAL data").c_str(),"lp");
	pandoraTBGraphEfficiency->Draw("lp same");

	efficiencyLegend->Draw();
	preliminaryEfficiency->Draw();


	cc2->cd();
	cc2->SetGrid();
	GraphFormat(arborSimGraphChargedEnergy);
	GraphFormat(arborTBGraphChargedEnergy);
	GraphFormat(pandoraSimGraphChargedEnergy);
	GraphFormat(pandoraTBGraphChargedEnergy);
	float rangeXMinChargedEnergy = 0.f;
	float rangeXMaxChargedEnergy = 90.f;
	float rangeYMinChargedEnergy = 0.f;
	float rangeYMaxChargedEnergy = 90.f;
	TLegend *chargedEnergyLegend = new TLegend(0.2,0.2,0.4,0.3);
	chargedEnergyLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	chargedEnergyLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	rootObjectStorage.push_back(chargedEnergyLegend);
	TPaveText *preliminaryChargedEnergy = CreateText("VERY PRELIMINARY");
	rootObjectStorage.push_back(preliminaryChargedEnergy);

	arborSimGraphChargedEnergy->SetMarkerColor(GraphSettings::arborSimMarkerColor);
	arborSimGraphChargedEnergy->SetMarkerStyle(GraphSettings::arborSimMarkerStyle);
	arborSimGraphChargedEnergy->SetTitle("");
	arborSimGraphChargedEnergy->SetLineStyle(GraphSettings::arborSimLineStyle);
	arborSimGraphChargedEnergy->SetLineWidth(GraphSettings::commonLineWidth);
	arborSimGraphChargedEnergy->SetLineColor(GraphSettings::arborSimLineColor);
	arborSimGraphChargedEnergy->GetXaxis()->SetTitle("Energy [GeV]");
	arborSimGraphChargedEnergy->GetXaxis()->SetRangeUser(rangeXMinChargedEnergy, rangeXMaxChargedEnergy);
	arborSimGraphChargedEnergy->GetYaxis()->SetTitle("Mean charged energy [GeV]");
	arborSimGraphChargedEnergy->GetYaxis()->SetRangeUser(rangeYMinChargedEnergy, rangeYMaxChargedEnergy);
	chargedEnergyLegend->AddEntry(arborSimGraphChargedEnergy, (std::string("ArborPFA ")+physicsList).c_str(), "lp");
	arborSimGraphChargedEnergy->Draw("alp");

	arborTBGraphChargedEnergy->SetMarkerColor(GraphSettings::arborTBMarkerColor);
	arborTBGraphChargedEnergy->SetTitle("");
	arborTBGraphChargedEnergy->SetLineStyle(GraphSettings::arborTBLineStyle);
	arborTBGraphChargedEnergy->SetLineWidth(GraphSettings::commonLineWidth);
	arborTBGraphChargedEnergy->SetLineColor(GraphSettings::arborTBLineColor);
	arborTBGraphChargedEnergy->GetXaxis()->SetTitle("Energy [GeV]");
	arborTBGraphChargedEnergy->GetYaxis()->SetRangeUser(rangeXMinChargedEnergy, rangeXMaxChargedEnergy);
	arborTBGraphChargedEnergy->GetYaxis()->SetTitle("");
	arborTBGraphChargedEnergy->GetYaxis()->SetRangeUser(rangeYMinChargedEnergy, rangeYMaxChargedEnergy);
	chargedEnergyLegend->AddEntry(arborTBGraphChargedEnergy, std::string("ArborPFA SDHCAL data").c_str(), "lp");
	arborTBGraphChargedEnergy->Draw("lp same");

	pandoraSimGraphChargedEnergy->SetMarkerColor(GraphSettings::pandoraSimMarkerColor);
	pandoraSimGraphChargedEnergy->SetMarkerStyle(GraphSettings::pandoraTBMarkerStyle);
	pandoraSimGraphChargedEnergy->SetTitle("");
	pandoraSimGraphChargedEnergy->SetLineStyle(GraphSettings::pandoraSimLineStyle);
	pandoraSimGraphChargedEnergy->SetLineColor(GraphSettings::pandoraSimLineColor);
	pandoraSimGraphChargedEnergy->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraSimGraphChargedEnergy->GetXaxis()->SetTitle("Energy [GeV]");
	pandoraSimGraphChargedEnergy->GetXaxis()->SetRangeUser(rangeXMinChargedEnergy, rangeXMaxChargedEnergy);
	pandoraSimGraphChargedEnergy->GetYaxis()->SetTitle("");
	pandoraSimGraphChargedEnergy->GetYaxis()->SetRangeUser(rangeYMinChargedEnergy, rangeYMaxChargedEnergy);
	chargedEnergyLegend->AddEntry(pandoraSimGraphChargedEnergy, (std::string("PandoraPFA ")+physicsList).c_str(), "lp");
	pandoraSimGraphChargedEnergy->Draw("lp same");

	pandoraTBGraphChargedEnergy->SetMarkerColor(GraphSettings::pandoraTBMarkerColor);
	pandoraTBGraphChargedEnergy->SetMarkerStyle(GraphSettings::pandoraTBMarkerStyle);
	pandoraTBGraphChargedEnergy->SetTitle("");
	pandoraTBGraphChargedEnergy->SetLineStyle(GraphSettings::pandoraTBLineStyle);
	pandoraTBGraphChargedEnergy->SetLineColor(GraphSettings::pandoraTBLineColor);
	pandoraTBGraphChargedEnergy->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraTBGraphChargedEnergy->GetXaxis()->SetTitle("Energy [GeV]");
	pandoraTBGraphChargedEnergy->GetXaxis()->SetRangeUser(rangeXMinChargedEnergy, rangeXMaxChargedEnergy);
	pandoraTBGraphChargedEnergy->GetYaxis()->SetTitle("");
	pandoraTBGraphChargedEnergy->GetYaxis()->SetRangeUser(rangeYMinChargedEnergy, rangeYMaxChargedEnergy);
	chargedEnergyLegend->AddEntry(pandoraTBGraphChargedEnergy, std::string("PandoraPFA SDHCAL data").c_str(), "lp");
	pandoraTBGraphChargedEnergy->Draw("lp same");

	chargedEnergyLegend->Draw();
	preliminaryChargedEnergy->Draw();

	cc3->cd();
	cc3->SetGrid();
	GraphFormat(arborSimGraphTotalMinusCharged);
	GraphFormat(arborTBGraphTotalMinusCharged);
	GraphFormat(pandoraSimGraphTotalMinusCharged);
	GraphFormat(pandoraTBGraphTotalMinusCharged);
	float rangeXMinTotalMinusCharged = 0.f;
	float rangeXMaxTotalMinusCharged = 90.f;
	float rangeYMinTotalMinusCharged = 0.f;
	float rangeYMaxTotalMinusCharged = 20.f;
	TLegend *totalMinusChargedEnergyLegend = new TLegend(0.2,0.2,0.4,0.3);
	totalMinusChargedEnergyLegend->SetLineColor(GraphSettings::commonLegendLineColor);
	totalMinusChargedEnergyLegend->SetFillColor(GraphSettings::commonLegendFillColor);
	rootObjectStorage.push_back(totalMinusChargedEnergyLegend);
	TPaveText *preliminaryTotalMinusChargedEnergy = CreateText("VERY PRELIMINARY");
	rootObjectStorage.push_back(preliminaryTotalMinusChargedEnergy);

	arborSimGraphTotalMinusCharged->SetMarkerColor(GraphSettings::arborSimMarkerColor);
	arborSimGraphTotalMinusCharged->SetMarkerStyle(GraphSettings::arborSimMarkerStyle);
	arborSimGraphTotalMinusCharged->SetTitle("");
	arborSimGraphTotalMinusCharged->SetLineStyle(GraphSettings::arborSimLineStyle);
	arborSimGraphTotalMinusCharged->SetLineWidth(GraphSettings::commonLineWidth);
	arborSimGraphTotalMinusCharged->SetLineColor(GraphSettings::arborSimLineColor);
	arborSimGraphTotalMinusCharged->GetXaxis()->SetTitle("Energy [GeV]");
	arborSimGraphTotalMinusCharged->GetXaxis()->SetRangeUser(rangeXMinTotalMinusCharged, rangeXMaxTotalMinusCharged);
	arborSimGraphTotalMinusCharged->GetYaxis()->SetTitle("Missing energy [GeV]");
	arborSimGraphTotalMinusCharged->GetYaxis()->SetRangeUser(rangeYMinTotalMinusCharged, rangeYMaxTotalMinusCharged);
	totalMinusChargedEnergyLegend->AddEntry(arborSimGraphTotalMinusCharged, (std::string("ArborPFA ")+physicsList).c_str(), "lp");
	arborSimGraphTotalMinusCharged->Draw("alp");

	arborTBGraphTotalMinusCharged->SetMarkerColor(GraphSettings::arborTBMarkerColor);
	arborTBGraphTotalMinusCharged->SetMarkerStyle(GraphSettings::arborTBMarkerStyle);
	arborTBGraphTotalMinusCharged->SetTitle("");
	arborTBGraphTotalMinusCharged->SetLineStyle(GraphSettings::arborTBLineStyle);
	arborTBGraphTotalMinusCharged->SetLineWidth(GraphSettings::commonLineWidth);
	arborTBGraphTotalMinusCharged->SetLineColor(GraphSettings::arborTBLineColor);
	arborTBGraphTotalMinusCharged->GetXaxis()->SetTitle("Energy [GeV]");
	arborTBGraphTotalMinusCharged->GetYaxis()->SetRangeUser(rangeXMinTotalMinusCharged, rangeXMaxTotalMinusCharged);
	arborTBGraphTotalMinusCharged->GetYaxis()->SetTitle("Missing energy [GeV]");
	arborTBGraphTotalMinusCharged->GetYaxis()->SetRangeUser(rangeYMinTotalMinusCharged, rangeYMaxTotalMinusCharged);
	totalMinusChargedEnergyLegend->AddEntry(arborTBGraphTotalMinusCharged, std::string("ArborPFA SDHCAL data").c_str(), "lp");
	arborTBGraphTotalMinusCharged->Draw("lp same");

	pandoraSimGraphTotalMinusCharged->SetMarkerColor(GraphSettings::pandoraSimMarkerColor);
	pandoraSimGraphTotalMinusCharged->SetMarkerStyle(GraphSettings::pandoraSimMarkerStyle);
	pandoraSimGraphTotalMinusCharged->SetTitle("");
	pandoraSimGraphTotalMinusCharged->SetLineStyle(GraphSettings::pandoraSimLineStyle);
	pandoraSimGraphTotalMinusCharged->SetLineColor(GraphSettings::pandoraSimLineColor);
	pandoraSimGraphTotalMinusCharged->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraSimGraphTotalMinusCharged->GetXaxis()->SetTitle("Energy [GeV]");
	pandoraSimGraphTotalMinusCharged->GetXaxis()->SetRangeUser(rangeXMinTotalMinusCharged, rangeXMaxTotalMinusCharged);
	pandoraSimGraphTotalMinusCharged->GetYaxis()->SetTitle("Missing energy [GeV]");
	pandoraSimGraphTotalMinusCharged->GetYaxis()->SetRangeUser(rangeYMinTotalMinusCharged, rangeYMaxTotalMinusCharged);
	totalMinusChargedEnergyLegend->AddEntry(pandoraSimGraphTotalMinusCharged, (std::string("PandoraPFA ")+physicsList).c_str(), "lp");
	pandoraSimGraphTotalMinusCharged->Draw("lp same");

	pandoraTBGraphTotalMinusCharged->SetMarkerColor(GraphSettings::pandoraTBMarkerColor);
	pandoraTBGraphTotalMinusCharged->SetMarkerStyle(GraphSettings::pandoraTBMarkerStyle);
	pandoraTBGraphTotalMinusCharged->SetTitle("");
	pandoraTBGraphTotalMinusCharged->SetLineStyle(GraphSettings::pandoraTBLineStyle);
	pandoraTBGraphTotalMinusCharged->SetLineColor(GraphSettings::pandoraTBLineColor);
	pandoraTBGraphTotalMinusCharged->SetLineWidth(GraphSettings::commonLineWidth);
	pandoraTBGraphTotalMinusCharged->GetXaxis()->SetTitle("Energy [GeV]");
	pandoraTBGraphTotalMinusCharged->GetXaxis()->SetRangeUser(rangeXMinTotalMinusCharged, rangeXMaxTotalMinusCharged);
	pandoraTBGraphTotalMinusCharged->GetYaxis()->SetTitle("Missing energy [GeV]");
	pandoraTBGraphTotalMinusCharged->GetYaxis()->SetRangeUser(rangeYMinTotalMinusCharged, rangeYMaxTotalMinusCharged);
	totalMinusChargedEnergyLegend->AddEntry(pandoraTBGraphTotalMinusCharged, std::string("PandoraPFA SDHCAL data").c_str(), "lp");
	pandoraTBGraphTotalMinusCharged->Draw("lp same");

	totalMinusChargedEnergyLegend->Draw();
	preliminaryTotalMinusChargedEnergy->Draw();

	cc4->cd();
	cc4->SetGrid();
	GraphFormat(arborSimGraphNPfos);
	GraphFormat(arborTBGraphNPfos);
	GraphFormat(pandoraSimGraphNPfos);
	GraphFormat(pandoraTBGraphNPfos);
	float rangeXMinNPfos = 0.f;
	float rangeXMaxNPfos = 90.f;
	float rangeYMinNPfos = 0.f;
	float rangeYMaxNPfos = 4.f;
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
	arborSimGraphNPfos->GetXaxis()->SetTitle("Energy [GeV]");
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

	cc4->WaitPrimitive();

	for( unsigned int i=0 ; i<singleParticlePFOAnalysisStorage.size() ; i++ )
		delete singleParticlePFOAnalysisStorage.at(i);
	singleParticlePFOAnalysisStorage.clear();

	for( unsigned int i=0 ; i<rootObjectStorage.size() ; i++ )
		delete rootObjectStorage.at(i);
	rootObjectStorage.clear();

}
