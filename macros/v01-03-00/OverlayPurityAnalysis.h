//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jun 11 11:03:55 2014 by ROOT version 5.34/05
// from TTree ArborOutputTree/ArborOutputTree
// found on file: ../ArborOutputFile_single_pi-_20GeV.root
//////////////////////////////////////////////////////////

#ifndef OverlayPurityAnalysis_h
#define OverlayPurityAnalysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <stdexcept>

// Header file for the classes stored in the TTree if any.
#include <vector>

#include "CommonTools.h"

// Fixed size dimensions of array or collections stored in the TTree if any.

class OverlayPurityAnalysis {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           nPfos;
   Int_t           nNeutralPfos;
   Int_t           nChargedPfos;
   Float_t         neutralEnergy;
   Float_t         chargedEnergy;
   vector<float>   *energy;
   vector<float>   *momentum;
   vector<int>     *nTracks;
   vector<int>     *nClusters;
   vector<int>     *charge;
   vector<float>   *mass;
   vector<int>     *pid;
   vector<int>     *nHit;
   vector<int>     *nHit1;
   vector<int>     *nHit2;
   vector<int>     *nHit3;
   Float_t         mcParticleEnergy1;
   Float_t         mcParticleEnergy2;
   vector<int>     *lcioFlagType1;
   vector<int>     *lcioFlagType2;
   vector<int>     *lcioFlagType3;

   // List of branches
   TBranch        *b_nPfos;   //!
   TBranch        *b_nNeutralPfos;   //!
   TBranch        *b_nChargedPfos;   //!
   TBranch        *b_neutralEnergy;   //!
   TBranch        *b_chargedEnergy;   //!
   TBranch        *b_energy;   //!
   TBranch        *b_momentum;   //!
   TBranch        *b_nTracks;   //!
   TBranch        *b_nClusters;   //!
   TBranch        *b_charge;   //!
   TBranch        *b_mass;   //!
   TBranch        *b_pid;   //!
   TBranch        *b_nHit;   //!
   TBranch        *b_nHit1;   //!
   TBranch        *b_nHit2;   //!
   TBranch        *b_nHit3;   //!
   TBranch        *b_mcParticleEnergy1;   //!
   TBranch        *b_mcParticleEnergy2;   //!
   TBranch        *b_lcioFlagType1;   //!
   TBranch        *b_lcioFlagType2;   //!
   TBranch        *b_lcioFlagType3;   //!

   OverlayPurityAnalysis(TTree *tree=0);
   virtual ~OverlayPurityAnalysis();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   void SetPurityCut(float cut) { m_purityCut = cut; }


   Distribution<float> nPfosDistribution;
   Distribution<float> purityChargedDistribution;
   Distribution<float> efficiencyChargedDistribution;
   Distribution<float> energyChargedDistribution;
   Distribution<float> energyDifferenceChargedDistribution;
   Distribution<float> energyDifferenceNeutralDistribution;
   Distribution<float> pureNeutralEnergyDistribution;
   Distribution<float> pureChargedEnergyDistribution;

   float               m_purityCut;
};

#endif

#ifdef OverlayPurityAnalysis_cxx
OverlayPurityAnalysis::OverlayPurityAnalysis(TTree *tree) : fChain(0) 
{
  	m_purityCut = 0;

// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
			if (tree == 0)
			{
				throw std::invalid_argument("Tree is NULL !");
			}
   Init(tree);
}

OverlayPurityAnalysis::~OverlayPurityAnalysis()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t OverlayPurityAnalysis::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t OverlayPurityAnalysis::LoadTree(Long64_t entry)
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

void OverlayPurityAnalysis::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

 // Set object pointer
 energy = 0;
 momentum = 0;
 nTracks = 0;
 nClusters = 0;
 charge = 0;
 mass = 0;
 pid = 0;
 nHit = 0;
 nHit1 = 0;
 nHit2 = 0;
 nHit3 = 0;
 mcParticleEnergy1 = 0;
 mcParticleEnergy2 = 0;
 lcioFlagType1 = 0;
 lcioFlagType2 = 0;
 lcioFlagType3 = 0;

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("nPfos", &nPfos, &b_nPfos);
   fChain->SetBranchAddress("nNeutralPfos", &nNeutralPfos, &b_nNeutralPfos);
   fChain->SetBranchAddress("nChargedPfos", &nChargedPfos, &b_nChargedPfos);
   fChain->SetBranchAddress("neutralEnergy", &neutralEnergy, &b_neutralEnergy);
   fChain->SetBranchAddress("chargedEnergy", &chargedEnergy, &b_chargedEnergy);
   fChain->SetBranchAddress("energy", &energy, &b_energy);
   fChain->SetBranchAddress("momentum", &momentum, &b_momentum);
   fChain->SetBranchAddress("nTracks", &nTracks, &b_nTracks);
   fChain->SetBranchAddress("nClusters", &nClusters, &b_nClusters);
   fChain->SetBranchAddress("charge", &charge, &b_charge);
   fChain->SetBranchAddress("mass", &mass, &b_mass);
   fChain->SetBranchAddress("pid", &pid, &b_pid);
   fChain->SetBranchAddress("nHit", &nHit, &b_nHit);
   fChain->SetBranchAddress("nHit1", &nHit1, &b_nHit1);
   fChain->SetBranchAddress("nHit2", &nHit2, &b_nHit2);
   fChain->SetBranchAddress("nHit3", &nHit3, &b_nHit3);
   fChain->SetBranchAddress("mcParticleEnergy1", &mcParticleEnergy1, &b_mcParticleEnergy1);
   fChain->SetBranchAddress("mcParticleEnergy2", &mcParticleEnergy2, &b_mcParticleEnergy2);
   fChain->SetBranchAddress("lcioFlagType1", &lcioFlagType1, &b_lcioFlagType1);
   fChain->SetBranchAddress("lcioFlagType2", &lcioFlagType2, &b_lcioFlagType2);
   fChain->SetBranchAddress("lcioFlagType3", &lcioFlagType3, &b_lcioFlagType3);
   Notify();
}

Bool_t OverlayPurityAnalysis::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void OverlayPurityAnalysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t OverlayPurityAnalysis::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
	if(entry) {}
   return 1;
}
#endif // #ifdef OverlayPurityAnalysis_cxx
