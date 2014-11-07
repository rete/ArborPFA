#define SinglePurityAnalysis_cxx
#include "SinglePurityAnalysis.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

void SinglePurityAnalysis::Loop()
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

		int totalNHit = 0;
		int chargedNHit = 0;
		float energyCharged = 0.f;

		if(1 != nChargedPfos)
			continue;

		for(int p=0 ; p<nPfos ; p++)
		{
			totalNHit += nHit->at(p);

			// charged case
			if(0 != charge->at(p))
			{
				energyCharged += energy->at(p);
				chargedNHit += nHit->at(p);
			}
		}

		purityChargedDistribution.distribution.push_back(float(chargedNHit)/float(totalNHit));
		energyChargedDistribution.distribution.push_back(energyCharged);
	}

	purityChargedDistribution.mean = Mean<float>(purityChargedDistribution.distribution);
	nPfosDistribution.mean = Mean<float>(nPfosDistribution.distribution);
	energyChargedDistribution.mean = Mean<float>(energyChargedDistribution.distribution);

	std::cout << "purityChargedDistribution.mean : " << purityChargedDistribution.mean << std::endl;
}
