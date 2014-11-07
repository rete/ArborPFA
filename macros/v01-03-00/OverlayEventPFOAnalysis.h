//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Mar 16 17:41:50 2014 by ROOT version 5.34/05
// from TTree PFOAnalysis/PFOAnalysis
// found on file: ../output/SingleParticle/SingleParticle_10_GeV_ArborPFA_MC.root
//////////////////////////////////////////////////////////

#ifndef OverlayEventPFOAnalysis_h
#define OverlayEventPFOAnalysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <stdexcept>

// Header file for the classes stored in the TTree if any.
#include <vector>

#include "CommonTools.h"

class OverlayEventPFOAnalysis {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           nPfos;
   Float_t         totalEventEnergy;
   Float_t         energyMC1;
   Float_t         energyMC2;
   Int_t           nChargedParticles;
   Int_t           nNeutralParticles;
   Float_t         chargedEnergy;
   Float_t         neutralEnergy;
   vector<int>     *isCharged;
   vector<int>     *nHitType1;
   vector<int>     *nHitType2;
   vector<int>     *nHitType3;
   vector<float>   *chi2TrackClusterComparison;
   vector<float>   *pfoEnergies;

   // List of branches
   TBranch        *b_nPfos;   //!
   TBranch        *b_totalEventEnergy;   //!
   TBranch        *b_energyMC1;   //!
   TBranch        *b_energyMC2;   //!
   TBranch        *b_nChargedParticles;   //!
   TBranch        *b_nNeutralParticles;   //!
   TBranch        *b_chargedEnergy;   //!
   TBranch        *b_neutralEnergy;   //!
   TBranch        *b_isCharged;   //!
   TBranch        *b_nHitType1;   //!
   TBranch        *b_nHitType2;   //!
   TBranch        *b_nHitType3;   //!
   TBranch        *b_chi2TrackClusterComparison;   //!
   TBranch        *b_pfoEnergies;   //!

   OverlayEventPFOAnalysis(TTree *tree=0);
   virtual ~OverlayEventPFOAnalysis();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   Distribution<float> efficiencyChargedNeutral;
   Distribution<float> efficiencyChargedNeutralAndAdditional;
   Distribution<float> totalMinusChargedParticleEnergyDistribution;
   Distribution<float> totalMinusNeutralParticleEnergyDistribution;
   Distribution<float> nPfosDistribution;
};

#endif

#ifdef OverlayEventPFOAnalysis_cxx
OverlayEventPFOAnalysis::OverlayEventPFOAnalysis(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0)
   {
   	throw std::invalid_argument("Tree is NULL !");
   }
   Init(tree);
}

OverlayEventPFOAnalysis::~OverlayEventPFOAnalysis()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t OverlayEventPFOAnalysis::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t OverlayEventPFOAnalysis::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void OverlayEventPFOAnalysis::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   isCharged = 0;
   nHitType1 = 0;
   nHitType2 = 0;
   nHitType3 = 0;
   chi2TrackClusterComparison = 0;
   pfoEnergies = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("nPfos", &nPfos, &b_nPfos);
   fChain->SetBranchAddress("totalEventEnergy", &totalEventEnergy, &b_totalEventEnergy);
   fChain->SetBranchAddress("energyMC1", &energyMC1, &b_energyMC1);
   fChain->SetBranchAddress("energyMC2", &energyMC2, &b_energyMC2);
   fChain->SetBranchAddress("nChargedParticles", &nChargedParticles, &b_nChargedParticles);
   fChain->SetBranchAddress("nNeutralParticles", &nNeutralParticles, &b_nNeutralParticles);
   fChain->SetBranchAddress("chargedEnergy", &chargedEnergy, &b_chargedEnergy);
   fChain->SetBranchAddress("neutralEnergy", &neutralEnergy, &b_neutralEnergy);
   fChain->SetBranchAddress("isCharged", &isCharged, &b_isCharged);
   fChain->SetBranchAddress("nHitType1", &nHitType1, &b_nHitType1);
   fChain->SetBranchAddress("nHitType2", &nHitType2, &b_nHitType2);
   fChain->SetBranchAddress("nHitType3", &nHitType3, &b_nHitType3);
   fChain->SetBranchAddress("chi2TrackClusterComparison", &chi2TrackClusterComparison, &b_chi2TrackClusterComparison);
   fChain->SetBranchAddress("pfoEnergies", &pfoEnergies, &b_pfoEnergies);
   Notify();
}

Bool_t OverlayEventPFOAnalysis::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void OverlayEventPFOAnalysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t OverlayEventPFOAnalysis::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef OverlayEventPFOAnalysis_cxx
