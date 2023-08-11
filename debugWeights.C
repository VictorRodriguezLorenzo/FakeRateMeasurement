#include <TCanvas.h>
#include <TChain.h>
#include <TString.h>


//------------------------------------------------------------------------------
//
// Simple macro that draws the weight applied on W+jets MC events, both for
// electrons (pdgId = 11) and muons (pdgId = 13). A rough fake selection has
// been applied.
//
//------------------------------------------------------------------------------
void debugWeights(TString prefix = "/eos/cms/store/group/phys_higgs/cmshww/calderon/HWWNano",
		  TString sample = "Summer22EE_126x_nAODv11_Full2022v11/MCl1loose2022EEv11__fakeSelKinMC/nanoLatino_WToLNu-2Jets__part")
{
  TChain* chain = new TChain("Events");

  for (int i=1; i<81; i++)
    chain->Add(Form("%s/%s%d.root", prefix.Data(), sample.Data(), i));

  TCanvas* c1 = new TCanvas("c1", "c1");

  c1->SetLogy();

  chain->Draw("baseW*Generator_weight*puWeight/1e3","(abs(Lepton_pdgId[0]) == 13) && (Lepton_pt[0] > 20.) && (fabs(Lepton_eta[0]) < 2.4) && (mtw1 < 20.) && (PuppiMET_pt < 20.) && (HLT_Mu17_TrkIsoVVL > 0.5)");
}
