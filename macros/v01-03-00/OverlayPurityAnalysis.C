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

		float energyCharged = 0.f;

		if(1 != nChargedPfos)
			continue;

		bool chargedFound = false;
		bool neutralFound = false;

		energyDifferenceNeutralDistribution.distribution.push_back(neutralEnergy-mcParticleEnergy1);

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
				reconstructedNHitBadCharged += lcioFlagType1->at(p);// + pfoHitType3->at(p)/2.f;
				reconstructedNHitGoodCharged += lcioFlagType2->at(p) + lcioFlagType3->at(p);
				energyCharged += energy->at(p);
			}
			// neutral case
			else
			{
				neutralFound = true;
				reconstructedNHitNeutral += lcioFlagType1->at(p) + lcioFlagType2->at(p) + lcioFlagType3->at(p);
				reconstructedNHitBadNeutral += lcioFlagType2->at(p);// + pfoHitType3->at(p)/2.f;
				reconstructedNHitGoodNeutral += lcioFlagType1->at(p) + lcioFlagType3->at(p);
			}
		}

		originalNHitNeutral = totalHitType1 + totalHitType3;
		originalNHitCharged = totalHitType2 + totalHitType3;

		float chargedPurity = reconstructedNHitGoodCharged / (reconstructedNHitGoodCharged + reconstructedNHitBadCharged);

		if(neutralFound)
		{
			if(chargedPurity > m_purityCut)
			{
				pureNeutralEnergyDistribution.distribution.push_back(neutralEnergy);
			}
		}

		if(chargedFound)
		{
		 purityChargedDistribution.distribution.push_back(chargedPurity);
		 efficiencyChargedDistribution.distribution.push_back(reconstructedNHitGoodCharged/originalNHitCharged);
//		 contaminationChargedDistribution.distribution.push_back((reconstructedNHitBadCharged + reconstructedNHitGoodCharged - originalNHitCharged) / originalNHitCharged);
		 energyChargedDistribution.distribution.push_back(energyCharged);

		 if(chargedPurity > m_purityCut)
		  pureChargedEnergyDistribution.distribution.push_back(energyCharged);
		}
		else
		{
			purityChargedDistribution.distribution.push_back(0.f);
			efficiencyChargedDistribution.distribution.push_back(0.f);
//			contaminationChargedDistribution.distribution.push_back(1.f);
			energyChargedDistribution.distribution.push_back(0.f);
		}
	} // end of loop

	purityChargedDistribution.mean = Mean<float>(purityChargedDistribution.distribution);
	efficiencyChargedDistribution.mean = Mean<float>(efficiencyChargedDistribution.distribution);
//	contaminationChargedDistribution.mean = Mean<float>(contaminationChargedDistribution.distribution);
	nPfosDistribution.mean = Mean<float>(nPfosDistribution.distribution);
	energyChargedDistribution.mean = Mean<float>(energyChargedDistribution.distribution);

	std::cout << "purityChargedDistribution.mean : " << purityChargedDistribution.mean << std::endl;
}
