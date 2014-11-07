#define OverlayEventPFOAnalysis_cxx
#include "OverlayEventPFOAnalysis.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void OverlayEventPFOAnalysis::Loop()
{
	if (fChain == 0)
		return;

	Long64_t nentries = fChain->GetEntriesFast();
	Long64_t nbytes = 0, nb = 0;
	float pNeutralCharged = 0.f;
	float pNeutralChargedAndAdditional = 0.f;

	for (Long64_t jentry=0; jentry<nentries;jentry++)
	{
		Long64_t ientry = LoadTree(jentry);

		if (ientry < 0)
			break;

		nb = fChain->GetEntry(jentry);
		nbytes += nb;

		if(nChargedParticles == 1 && nNeutralParticles == 1)
		{
			efficiencyChargedNeutral.distribution.push_back(1);
			efficiencyChargedNeutralAndAdditional.distribution.push_back(1);
			pNeutralCharged++;
			pNeutralChargedAndAdditional++;

			totalMinusChargedParticleEnergyDistribution.distribution.push_back(energyMC2 - chargedEnergy);
			totalMinusNeutralParticleEnergyDistribution.distribution.push_back(energyMC1 - neutralEnergy);
		}
		else
		{
			efficiencyChargedNeutral.distribution.push_back(0);

			if(nChargedParticles == 1 && nNeutralParticles > 1)
			{
				efficiencyChargedNeutralAndAdditional.distribution.push_back(1);
				pNeutralChargedAndAdditional++;
				totalMinusChargedParticleEnergyDistribution.distribution.push_back(energyMC2 - chargedEnergy);
				totalMinusNeutralParticleEnergyDistribution.distribution.push_back(energyMC1 - neutralEnergy);
			}
			else
			{
				efficiencyChargedNeutralAndAdditional.distribution.push_back(0);
			}
		}

		nPfosDistribution.distribution.push_back(nPfos);
	}

	pNeutralCharged /= nentries;
	efficiencyChargedNeutral.mean = Mean<float>(efficiencyChargedNeutral.distribution);
	efficiencyChargedNeutral.error = std::sqrt(pNeutralCharged*(1-pNeutralCharged));

	pNeutralChargedAndAdditional /= nentries;
	efficiencyChargedNeutralAndAdditional.mean = Mean<float>(efficiencyChargedNeutralAndAdditional.distribution);
	efficiencyChargedNeutralAndAdditional.error = std::sqrt(pNeutralChargedAndAdditional*(1-pNeutralChargedAndAdditional));

	totalMinusNeutralParticleEnergyDistribution.mean  = Mean<float>(totalMinusNeutralParticleEnergyDistribution.distribution);
	totalMinusNeutralParticleEnergyDistribution.rms   = RMS<float>(totalMinusNeutralParticleEnergyDistribution.distribution);
	totalMinusNeutralParticleEnergyDistribution.rms90 = RMS90<float>(totalMinusNeutralParticleEnergyDistribution.distribution);

	totalMinusChargedParticleEnergyDistribution.mean  = Mean<float>(totalMinusChargedParticleEnergyDistribution.distribution);
	totalMinusChargedParticleEnergyDistribution.rms   = RMS<float>(totalMinusChargedParticleEnergyDistribution.distribution);
	totalMinusChargedParticleEnergyDistribution.rms90 = RMS90<float>(totalMinusChargedParticleEnergyDistribution.distribution);

	nPfosDistribution.mean  = Mean<float>(nPfosDistribution.distribution);
	nPfosDistribution.rms   = RMS<float>(nPfosDistribution.distribution);
	nPfosDistribution.rms90 = RMS90<float>(nPfosDistribution.distribution);

}
