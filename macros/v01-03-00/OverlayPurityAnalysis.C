#define OverlayPurityAnalysis_cxx
#include "OverlayPurityAnalysis.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

void OverlayPurityAnalysis::Loop()
{
	if (fChain == 0)
		return;

	Long64_t nentries = fChain->GetEntriesFast();
	Long64_t nbytes = 0, nb = 0;

	for (Long64_t jentry=0; jentry<nentries;jentry++)
	{
		Long64_t ientry = LoadTree(jentry);

		if (ientry < 0)
			break;

		nb = fChain->GetEntry(jentry);
		nbytes += nb;

		nPfosDistribution.distribution.push_back(nPfos);

		unsigned int totalHitType1 = 0;
		unsigned int totalHitType2 = 0;
		unsigned int totalHitType3 = 0;

		float originalNHitCharged = 0;
		float originalNHitNeutral = 0;

		float reconstructedNHitCharged = 0;
		float reconstructedNHitNeutral = 0;

		float reconstructedNHitGoodCharged = 0;
		float reconstructedNHitBadCharged = 0;
		float reconstructedNHitGoodNeutral = 0;
		float reconstructedNHitBadNeutral = 0;

		if(1 != nChargedPfos)
			continue;

		bool chargedFound = false;
		bool neutralFound = false;

		// fill energy information
		neutralEnergyDifferenceDistribution.distribution.push_back(neutralEnergy-mcParticleEnergy1);
	 neutralEnergyDistribution.distribution.push_back(neutralEnergy);
	 chargedEnergyDifferenceDistribution.distribution.push_back(chargedEnergy-mcParticleEnergy2);
	 chargedEnergyDistribution.distribution.push_back(chargedEnergy);

	 // analyze pfos and extract the purity/efficiency
		for(int p=0 ; p<nPfos ; p++)
		{
			totalHitType1 += lcioFlagType1->at(p);
			totalHitType2 += lcioFlagType2->at(p);
			totalHitType3 += lcioFlagType3->at(p);

			// charged case
			if(0 != charge->at(p))
			{
				chargedFound = true;
				reconstructedNHitCharged += lcioFlagType1->at(p) + lcioFlagType2->at(p) + lcioFlagType3->at(p);
				reconstructedNHitBadCharged += lcioFlagType1->at(p) + lcioFlagType3->at(p)/2.f;
				reconstructedNHitGoodCharged += lcioFlagType2->at(p) + lcioFlagType3->at(p)/2.f;
			}
			// neutral case
			else
			{
				neutralFound = true;
				reconstructedNHitNeutral += lcioFlagType1->at(p) + lcioFlagType2->at(p) + lcioFlagType3->at(p);
				reconstructedNHitBadNeutral += lcioFlagType2->at(p) + lcioFlagType3->at(p)/2.f;
				reconstructedNHitGoodNeutral += lcioFlagType1->at(p) + lcioFlagType3->at(p)/2.f;
			}
		}

		originalNHitNeutral = totalHitType1 + totalHitType3/2.f;
		originalNHitCharged = totalHitType2 + totalHitType3/2.f;

		float chargedPurity = reconstructedNHitGoodCharged / (reconstructedNHitGoodCharged + reconstructedNHitBadCharged);
		float chargedEfficiency = reconstructedNHitGoodCharged/originalNHitCharged;
		float neutralPurity = reconstructedNHitGoodNeutral / (reconstructedNHitGoodNeutral + reconstructedNHitBadNeutral);
		float neutralEfficiency = reconstructedNHitGoodNeutral/originalNHitNeutral;

		// fill efficiency/purity for the charged particle
		chargedPurityDistribution.distribution.push_back(chargedFound ? chargedPurity : 0.f);
		chargedEfficiencyDistribution.distribution.push_back(chargedFound ? chargedEfficiency : 0.f);

		// fill the efficiency/purity for the neutral particle
		neutralPurityDistribution.distribution.push_back(neutralFound ? neutralPurity : 0.f);
		neutralEfficiencyDistribution.distribution.push_back(neutralFound ? neutralEfficiency : 0.f);

	} // end of loop

	chargedPurityDistribution.mean = Mean<float>(chargedPurityDistribution.distribution);
	chargedEfficiencyDistribution.mean = Mean<float>(chargedEfficiencyDistribution.distribution);
 chargedEnergyDifferenceDistribution.mean = Mean<float>(chargedEnergyDifferenceDistribution.distribution);
 chargedEnergyDistribution.mean = Mean<float>(chargedEnergyDistribution.distribution);

	neutralPurityDistribution.mean = Mean<float>(neutralPurityDistribution.distribution);
	neutralEfficiencyDistribution.mean = Mean<float>(neutralEfficiencyDistribution.distribution);
 neutralEnergyDifferenceDistribution.mean = Mean<float>(neutralEnergyDifferenceDistribution.distribution);
 neutralEnergyDistribution.mean = Mean<float>(neutralEnergyDistribution.distribution);

	nPfosDistribution.mean = Mean<float>(nPfosDistribution.distribution);
}
