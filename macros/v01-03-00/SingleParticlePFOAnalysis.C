#define SingleParticlePFOAnalysis_cxx
#include "SingleParticlePFOAnalysis.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>

void SingleParticlePFOAnalysis::Loop()
{
	if (fChain == 0)
		return;

	Long64_t nentries = fChain->GetEntriesFast();

	Long64_t nbytes = 0, nb = 0;
	float p = 0.f;

	for (Long64_t jentry=0; jentry<nentries;jentry++) {

		Long64_t ientry = LoadTree(jentry);

		if (ientry < 0)
			break;

		nb = fChain->GetEntry(jentry);
		nbytes += nb;

		if((nChargedParticles == 1 && nNeutralParticles == 0) || (nChargedParticles == 0 && nNeutralParticles == 1))
		{
			efficiencyDistribution.distribution.push_back(1);
			p++;
		}
		else
			efficiencyDistribution.distribution.push_back(0);

		totalEventEnergyDistribution.distribution.push_back(totalEventEnergy);

		if(nChargedParticles >= 1)
		{
			totalMinusChargedDistribution.distribution.push_back(totalEventEnergy - chargedEnergy);
			chargedEnergyDistribution.distribution.push_back(chargedEnergy);
		}

		if(nNeutralParticles >= 0)
		{
			neutralEnergyDistribution.distribution.push_back(neutralEnergy);
		}

		nPfosDistribution.distribution.push_back(nPfos);
	}

	p /= nentries;

	efficiencyDistribution.mean  = Mean<float>(efficiencyDistribution.distribution);
	std::cout << "p = " << p << std::endl;
	efficiencyDistribution.error = std::sqrt(p*(1-p));
	std::cout << "efficiencyDistribution.error = " << efficiencyDistribution.error << std::endl;

	totalEventEnergyDistribution.mean  = Mean<float>(totalEventEnergyDistribution.distribution);
	totalEventEnergyDistribution.rms   = RMS<float>(totalEventEnergyDistribution.distribution);
	totalEventEnergyDistribution.rms90 = RMS90<float>(totalEventEnergyDistribution.distribution);

	chargedEnergyDistribution.mean  = Mean<float>(chargedEnergyDistribution.distribution);
	chargedEnergyDistribution.rms   = RMS<float>(chargedEnergyDistribution.distribution);
	chargedEnergyDistribution.rms90 = RMS90<float>(chargedEnergyDistribution.distribution);

	neutralEnergyDistribution.mean  = Mean<float>(neutralEnergyDistribution.distribution);
	neutralEnergyDistribution.rms   = RMS<float>(neutralEnergyDistribution.distribution);
	neutralEnergyDistribution.rms90 = RMS90<float>(neutralEnergyDistribution.distribution);

	totalMinusChargedDistribution.mean  = Mean<float>(totalMinusChargedDistribution.distribution);
	totalMinusChargedDistribution.rms   = RMS<float>(totalMinusChargedDistribution.distribution);
	totalMinusChargedDistribution.rms90 = RMS90<float>(totalMinusChargedDistribution.distribution);

	nPfosDistribution.mean  = Mean<float>(nPfosDistribution.distribution);
	nPfosDistribution.rms   = RMS<float>(nPfosDistribution.distribution);
	nPfosDistribution.rms90 = RMS90<float>(nPfosDistribution.distribution);

}
