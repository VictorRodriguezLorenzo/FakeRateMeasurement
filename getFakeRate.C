// Constants
//------------------------------------------------------------------------------
const Int_t   njetet = 7;

const Float_t muojetarray[njetet] = {10, 15, 20, 25, 30, 35, 45};
const Float_t elejetarray[njetet] = {10, 15, 20, 25, 30, 35, 45};
const Float_t colors     [njetet] = {kRed, kRed+1, kRed+2, kRed+3, kRed+4, kRed+5, kRed+6};

const Float_t year_lumi[6] = {
  19.5,  // 2016_HIPM
  17.0,  // 2016_noHIPM
  41.5,  // 2017
  59.7,  // 2018
  8.47,    // 2022 
  21.1,   // 2022EE
};

const TString year_name[6] = {
  "2016_HIPM",
  "2016_noHIPM",
  "2017",
  "2018",
  "2022",
  "2022EE",
};


// Data members
//------------------------------------------------------------------------------
Float_t lepscale = 1.;

Bool_t  debug        = false;
Bool_t  debugWeights = false;
Bool_t  setgrid      = true;
Bool_t  Wsubtraction = true;
Bool_t  Zsubtraction = true;

Bool_t  performPromptRate       = true;
Bool_t  performElectronFakeRate = true;
Bool_t  performMuonFakeRate     = true;
Bool_t  performAllJetFakeRate   = false;
Bool_t  performDataMC           = true; 

TFile*  dataFR;
TFile*  wjetsFR;
TFile*  zjetsFR;
TFile*  zjetsPR;

Int_t   year_index;

TString inputdir;
TString outputdir;
TString pngdir;
TString year;
TString muon_wp;
TString ele_wp;

TString btagdir = "";


// Functions
//------------------------------------------------------------------------------
void     Cosmetics   (TH1D*       hist,
		      Option_t*   option,
		      TString     xtitle,
		      TString     ytitle,
		      Color_t     color,
		      Float_t     ymin = 0.0,
		      Float_t     ymax = 1.0);

void     DrawAllJetEt(TString     flavour,
		      TString     variable,
		      TString     xtitle);

void     DrawFR      (TString     flavour,
		      TString     variable,
		      TString     xtitle,
		      Float_t     jetet);

void     DrawPR      (TString     flavour,
		      TString     variable,
		      TString     xtitle);

Float_t  GetLepScale (TString     flavour,
		      TString     loose_tight,
		      TString     variable,
		      TString     xtitle,
		      TString     units,
		      Float_t     jetet);

void     WriteFR     (TString     flavour,
		      Float_t     jetet);

void     WritePR     (TString     flavour);

void     DrawLatex   (Font_t      tfont,
		      Float_t     x,
		      Float_t     y,
		      Float_t     tsize,
		      Short_t     align,
		      const char* text,
		      Bool_t      setndc = true);

TLegend* DrawLegend  (Float_t     x1,
		      Float_t     y1,
		      TH1*        hist,
		      TString     label,
		      TString     option  = "p",
		      Float_t     tsize   = 0.035,
		      Float_t     xoffset = 0.184,
		      Float_t     yoffset = 0.043);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// getFakeRate
//
// Open issues:
//
//    * lepscale is not applied in DrawAllJetEt;
//    * lepscale is not applied in the debug histograms of DrawFR.
//
//    Float_t the_elejetet = 35 | -1
//    Float_t the_muojetet = 25 | -1
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void getFakeRate(TString the_year      = "2022EE",
		 //TString the_leptondir = "cut_Tight80x_tthmva_80__mvaFall17V2Iso_WP90",
		 TString the_leptondir = "",
		 Float_t the_elejetet  = 35,
		 Float_t the_muojetet  = 25)
{
  if (the_year == "1984") {

    printf("\n");
    printf(" root -l -b -q \'getFakeRate.C(\"2016_HIPM\")\'\n");
    printf(" root -l -b -q \'getFakeRate.C(\"2016_noHIPM\")\'\n");
    printf(" root -l -b -q \'getFakeRate.C(\"2017\")\'\n");
    printf(" root -l -b -q \'getFakeRate.C(\"2018\")\'\n");
    printf("\n");
    return;
  }

  if      (the_year.Contains("noHIPM")) year_index = 1;
  else if (the_year.Contains("HIPM"))   year_index = 0;
  else if (the_year.Contains("2017"))   year_index = 2;
  else if (the_year.Contains("2018"))   year_index = 3;
  else if (the_year.Contains("2022"))   year_index = 4;
  else if (the_year.Contains("2022EE"))   year_index = 5;
  else {
    printf("\n Wrong year format\n");
    return;
  }

  year = the_year;


  // Get the lepton working points
  //----------------------------------------------------------------------------
  TString tok;

  Ssiz_t from = 0;

  while (the_leptondir.Tokenize(tok, from, "__"))
    (from > 0) ? muon_wp = tok : ele_wp = tok;


  // Set directories
  //----------------------------------------------------------------------------
  inputdir  = Form("results/%s/%s", year.Data(), the_leptondir.Data());
  pngdir    = Form("png/%s/%s",     year.Data(), the_leptondir.Data());
  outputdir = Form("fakerate/%s",   year.Data());

  gInterpreter->ExecuteMacro("PaperStyle.C");

  gSystem->mkdir(pngdir, kTRUE);

  gSystem->mkdir(outputdir + "/" + muon_wp, kTRUE);
  gSystem->mkdir(outputdir + "/" + ele_wp,  kTRUE);

  dataFR  = new TFile (inputdir + "/hadd_data.root",  "read");
  wjetsFR = new TFile (inputdir + "/hadd_wjets.root", "read");
  zjetsFR = new TFile (inputdir + "/hadd_zjets.root", "read");
  zjetsPR = new TFile (inputdir + "/hadd_zjets.root", "read");


  // Prompt rate
  //----------------------------------------------------------------------------
  if (performPromptRate)
    {
      WritePR("Ele");
      WritePR("Muon");

      DrawPR("Ele",  "pt",  "p_{T} [GeV]");
      DrawPR("Muon", "pt",  "p_{T} [GeV]");
      DrawPR("Ele",  "eta", "|#eta|");
      DrawPR("Muon", "eta", "|#eta|");
    }


  // Electron fake rate
  //----------------------------------------------------------------------------
  if (performElectronFakeRate)
    {
      if (the_elejetet > 0) {

	lepscale = GetLepScale("Ele", "loose", "m2l", "m_{#font[12]{ll}}", "GeV", the_elejetet);

	WriteFR("Ele", the_elejetet);

	DrawFR("Ele", "pt",  "p_{T} [GeV]", the_elejetet);
	DrawFR("Ele", "eta", "|#eta|",      the_elejetet);
      }
      else {
	for (Int_t i=0; i<njetet; i++) {

	  lepscale = GetLepScale("Ele", "loose", "m2l", "m_{#font[12]{ll}}", "GeV", elejetarray[i]);

	  WriteFR("Ele", elejetarray[i]);

	  DrawFR("Ele", "pt",  "p_{T} [GeV]", elejetarray[i]);
	  DrawFR("Ele", "eta", "|#eta|",      elejetarray[i]);
	}
      }

      lepscale = 1.;
    }


  // Muon fake rate
  //----------------------------------------------------------------------------
  if (performMuonFakeRate)
    {
      if (the_muojetet > 0) {

	lepscale = GetLepScale("Muon", "loose", "m2l", "m_{#font[12]{ll}}", "GeV", the_muojetet);

	WriteFR("Muon", the_muojetet);

	DrawFR("Muon", "pt",  "p_{T} [GeV]", the_muojetet);
	DrawFR("Muon", "eta", "|#eta|",      the_muojetet);
      }
      else {
	for (Int_t i=0; i<njetet; i++) {

	  lepscale = GetLepScale("Muon", "loose", "m2l", "m_{#font[12]{ll}}", "GeV", muojetarray[i]);

	  WriteFR("Muon", muojetarray[i]);
      
	  DrawFR("Muon", "pt",  "p_{T} [GeV]", muojetarray[i]);
	  DrawFR("Muon", "eta", "|#eta|",      muojetarray[i]);
	}
      }

      lepscale = 1.;
    }


  // All fake rates together
  //----------------------------------------------------------------------------
  if (performAllJetFakeRate)
    {
      if (the_elejetet < 0 && the_muojetet < 0) {

	DrawAllJetEt("Ele",  "pt",  "p_{T} [GeV]");
	DrawAllJetEt("Muon", "pt",  "p_{T} [GeV]");
	DrawAllJetEt("Ele",  "eta", "|#eta|");
	DrawAllJetEt("Muon", "eta", "|#eta|");
      }
    }
}


//------------------------------------------------------------------------------
// DrawAllJetEt
//------------------------------------------------------------------------------
void DrawAllJetEt(TString flavour,
		  TString variable,
		  TString xtitle)
{
  Float_t jetet;

  TString title1 = Form("%s fake rate considering various jet p_{T}", flavour.Data());    

  TCanvas* canvas1 = new TCanvas(title1 + " " + variable, title1 + " " + variable);
  
  canvas1->SetGridx(setgrid);
  canvas1->SetGridy(setgrid);
  canvas1->cd();

  for (Int_t i=0; i<njetet; i++) {

    jetet = (flavour == "Ele") ? elejetarray[i] : muojetarray[i];

    TString suffix = Form("%s_bin_%.0fGeV", variable.Data(), jetet);


    // Read loose and tight histograms
    //--------------------------------------------------------------------------
    TH1D* h_loose_data  = (TH1D*)dataFR ->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix);
    TH1D* h_tight_data  = (TH1D*)dataFR ->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix);
    TH1D* h_loose_wjets = (TH1D*)wjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix);
    TH1D* h_tight_wjets = (TH1D*)wjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix);
    TH1D* h_loose_zjets = (TH1D*)zjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix);
    TH1D* h_tight_zjets = (TH1D*)zjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix);


    // Prepare fake rate histograms
    //--------------------------------------------------------------------------
    TH1D* h_FR_EWK             = (TH1D*)h_tight_data->Clone("h_" + flavour + "_FR_EWK_"             + variable);
    TH1D* h_FR_EWK_denominator = (TH1D*)h_loose_data->Clone("h_" + flavour + "_FR_EWK_denominator_" + variable);


    // Do the math
    //--------------------------------------------------------------------------
    if (Zsubtraction) h_FR_EWK->Add(h_tight_zjets, -lepscale);
    if (Wsubtraction) h_FR_EWK->Add(h_tight_wjets, -lepscale);
    
    if (Zsubtraction) h_FR_EWK_denominator->Add(h_loose_zjets, -lepscale);
    if (Wsubtraction) h_FR_EWK_denominator->Add(h_loose_wjets, -lepscale);
    
    h_FR_EWK->Divide(h_FR_EWK_denominator);

    Cosmetics(h_FR_EWK, "ep,same", xtitle, title1, colors[i]);

    DrawLegend(0.22, 0.58+(0.042*i), h_FR_EWK, Form("%.0f GeV threshold", jetet));
  }


  // Save
  //----------------------------------------------------------------------------
  canvas1->SaveAs(Form("%s/%s_FR_%s_combined.png",
		       pngdir.Data(),
		       flavour.Data(),
		       variable.Data()));
}


//------------------------------------------------------------------------------
// DrawFR
//------------------------------------------------------------------------------
void DrawFR(TString flavour,
	    TString variable,
	    TString xtitle,
	    Float_t jetet)
{
  TString suffix = Form("%s_bin_%.0fGeV", variable.Data(), jetet);


  // Read loose and tight histograms
  //----------------------------------------------------------------------------
  TH1D* h_loose_data  = (TH1D*)dataFR ->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix);
  TH1D* h_tight_data  = (TH1D*)dataFR ->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix);
  TH1D* h_loose_wjets = (TH1D*)wjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix);
  TH1D* h_tight_wjets = (TH1D*)wjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix);
  TH1D* h_loose_zjets = (TH1D*)zjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix);
  TH1D* h_tight_zjets = (TH1D*)zjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix);


  // Debug weights
  //----------------------------------------------------------------------------
  if (debugWeights && variable.EqualTo("pt"))
    {
      TString suffix_raw = Form("%s_bin_raw_%.0fGeV", variable.Data(), jetet);

      TH1D* h_loose_data_raw  = (TH1D*)dataFR ->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix_raw);
      TH1D* h_tight_data_raw  = (TH1D*)dataFR ->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix_raw);
      TH1D* h_loose_wjets_raw = (TH1D*)wjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix_raw);
      TH1D* h_tight_wjets_raw = (TH1D*)wjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix_raw);
      TH1D* h_loose_zjets_raw = (TH1D*)zjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix_raw);
      TH1D* h_tight_zjets_raw = (TH1D*)zjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix_raw);

      printf("\n  %s Z+jets %s %s when jet pt > %.0f GeV\n\n",
	     year.Data(), flavour.Data(), variable.Data(), jetet);

      printf("  %s bin LOOSE   raw weight | TIGHT    raw weight\n",
	     variable.Data());

      for (Int_t ibin=1; ibin<=h_loose_data->GetNbinsX(); ibin++) {

	printf(" [%.0f,%.0f]      %6.0f  %.3f |       %6.0f  %.3f\n",
	       h_loose_zjets->GetBinLowEdge(ibin),
	       h_loose_zjets->GetBinLowEdge(ibin+1),
	       h_loose_zjets_raw->GetBinContent(ibin),
	       h_loose_zjets->GetBinContent(ibin) / h_loose_zjets_raw->GetBinContent(ibin),
	       h_tight_zjets_raw->GetBinContent(ibin),
	       h_tight_zjets->GetBinContent(ibin) / h_tight_zjets_raw->GetBinContent(ibin));
      }
      
      printf("\n");
    }


  // Make EWK correction histograms
  //----------------------------------------------------------------------------
  TH1D* h_tight_correction = (TH1D*)h_tight_zjets->Clone("h_" + flavour + "_FR_" + variable);
  TH1D* h_loose_correction = (TH1D*)h_loose_zjets->Clone("h_" + flavour + "_FR_" + variable);
  TH1D* h_EWKrel_tight     = (TH1D*)h_tight_data ->Clone("h_" + flavour + "_FR_" + variable);
  TH1D* h_EWKrel_loose     = (TH1D*)h_loose_data ->Clone("h_" + flavour + "_FR_" + variable);
  
  h_tight_correction->Add(h_tight_wjets, 1);  // tight_correction = tight_zjets + tight_wjets
  h_loose_correction->Add(h_loose_wjets, 1);  // loose_correction = loose_zjets + loose_wjets

  h_EWKrel_tight->Divide(h_tight_correction, h_tight_data);  // EWKrel_tight = tight_correction / tight_data
  h_EWKrel_loose->Divide(h_loose_correction, h_loose_data);  // EWKrel_loose = loose_correction / loose_data
 

  // Prepare fake rate histograms
  //----------------------------------------------------------------------------
  TH1D* h_FR                 = (TH1D*)h_tight_data->Clone("h_" + flavour + "_FR_"                 + variable);
  TH1D* h_FR_denominator     = (TH1D*)h_loose_data->Clone("h_" + flavour + "_FR_denominator_"     + variable);
  TH1D* h_FR_EWK             = (TH1D*)h_tight_data->Clone("h_" + flavour + "_FR_EWK_"             + variable);
  TH1D* h_FR_EWK_denominator = (TH1D*)h_loose_data->Clone("h_" + flavour + "_FR_EWK_denominator_" + variable);


  // Do the math
  //----------------------------------------------------------------------------
  h_FR->Divide(h_FR_denominator);

  if (Zsubtraction) h_FR_EWK->Add(h_tight_zjets, -lepscale);
  if (Wsubtraction) h_FR_EWK->Add(h_tight_wjets, -lepscale);

  if (Zsubtraction) h_FR_EWK_denominator->Add(h_loose_zjets, -lepscale);
  if (Wsubtraction) h_FR_EWK_denominator->Add(h_loose_wjets, -lepscale);

  h_FR_EWK->Divide(h_FR_EWK_denominator);


  // Draw fake rate
  //----------------------------------------------------------------------------
  TString title1 = Form("%s fake rate when jet p_{T} > %.0f GeV", flavour.Data(), jetet);

  TCanvas* canvas1 = new TCanvas(title1 + " " + variable, title1 + " " +  variable);
      
  canvas1->SetGridx(setgrid);
  canvas1->SetGridy(setgrid);
  
  Cosmetics(h_FR,     "ep",      xtitle, title1, kBlack);
  Cosmetics(h_FR_EWK, "ep,same", xtitle, title1, kRed+1);

  DrawLatex(42, 0.940, 0.945, 0.045, 31, Form("%.1f fb^{-1} (13 TeV)", year_lumi[year_index]));

  DrawLegend(0.22, 0.845, h_FR,     "Without EWK correction");
  DrawLegend(0.22, 0.800, h_FR_EWK, "With EWK correction");

  canvas1->SaveAs(Form("%s/%s_FR_%s_%.0fGeV.png", pngdir.Data(), flavour.Data(), variable.Data(), jetet));


  // Draw EWK correction
  //----------------------------------------------------------------------------
  TString title2 = Form("%s EWK correction when jet p_{T} > %.0f GeV", flavour.Data(), jetet);

  TCanvas* canvas2 = new TCanvas(title2 + " " + variable, title2 + " " + variable);

  canvas2->SetGridx(setgrid);
  canvas2->SetGridy(setgrid);

  Cosmetics(h_EWKrel_tight, "ep",      xtitle, title2, kBlack, 0.0, 1.6);
  Cosmetics(h_EWKrel_loose, "ep,same", xtitle, title2, kRed+1, 0.0, 1.6);

  DrawLegend(0.22, 0.845, h_EWKrel_tight, "tight EWK / tight data");
  DrawLegend(0.22, 0.800, h_EWKrel_loose, "loose EWK / loose data");

  DrawLatex(42, 0.940, 0.945, 0.045, 31, Form("%.1f fb^{-1} (13 TeV)", year_lumi[year_index]));

  canvas2->SaveAs(Form("%s/%s_EWKrel_%s_%.0fGeV.png", pngdir.Data(), flavour.Data(), variable.Data(), jetet));


  if (!debug) return;


  // Draw tight histograms
  //----------------------------------------------------------------------------
  TString title3 = Form("%s tight histograms when jet p_{T} > %.0f GeV", flavour.Data(), jetet);

  TCanvas* canvas3 = new TCanvas(title3 + " " + variable, title3 + " " + variable);

  canvas3->SetGridx(setgrid);
  canvas3->SetGridy(setgrid);

  Cosmetics(h_tight_data,  "ep",      xtitle, title3, kBlack, 0, -999);
  Cosmetics(h_tight_zjets, "ep,same", xtitle, title3, kRed+1, 0, -999);
  Cosmetics(h_tight_wjets, "ep,same", xtitle, title3, kBlue,  0, -999);
  
  if (variable.Contains("pt"))
    {
      DrawLegend(0.67, 0.845, h_tight_data,  "tight data");
      DrawLegend(0.67, 0.800, h_tight_zjets, "tight Z+jets");
      DrawLegend(0.67, 0.755, h_tight_wjets, "tight W+jets");
    }

  DrawLatex(42, 0.940, 0.945, 0.045, 31, Form("%.1f fb^{-1} (13 TeV)", year_lumi[year_index]));

  canvas3->SaveAs(Form("%s/%s_tight-histograms_%s_%.0fGeV.png", pngdir.Data(), flavour.Data(), variable.Data(), jetet));


  // Draw loose histograms
  //----------------------------------------------------------------------------
  TString title4 = Form("%s loose histograms when jet p_{T} > %.0f GeV", flavour.Data(), jetet);

  TCanvas* canvas4 = new TCanvas(title4 + " " + variable, title4 + " " + variable);

  canvas4->SetGridx(setgrid);
  canvas4->SetGridy(setgrid);

  Cosmetics(h_loose_data,  "ep",      xtitle, title4, kBlack, 0, -999);
  Cosmetics(h_loose_zjets, "ep,same", xtitle, title4, kRed+1, 0, -999);
  Cosmetics(h_loose_wjets, "ep,same", xtitle, title4, kBlue,  0, -999);

  if (variable.Contains("pt"))
    {
      DrawLegend(0.67, 0.845, h_loose_data,  "loose data");
      DrawLegend(0.67, 0.800, h_loose_zjets, "loose Z+jets");
      DrawLegend(0.67, 0.755, h_loose_wjets, "loose W+jets");
    }

  DrawLatex(42, 0.940, 0.945, 0.045, 31, Form("%.1f fb^{-1} (13 TeV)", year_lumi[year_index]));

  canvas4->SaveAs(Form("%s/%s_loose-histograms_%s_%.0fGeV.png", pngdir.Data(), flavour.Data(), variable.Data(), jetet));
}


//------------------------------------------------------------------------------
// DrawPR
//------------------------------------------------------------------------------
void DrawPR(TString flavour,
	    TString variable,
	    TString xtitle)
{
  TH1D* h_loose_zjets = (TH1D*)zjetsPR -> Get("PR/00/h_" + flavour + "_loose_" + variable + "_PR");
  TH1D* h_tight_zjets = (TH1D*)zjetsPR -> Get("PR/00/h_" + flavour + "_tight_" + variable + "_PR");

  TH1D* h_PR = (TH1D*)h_tight_zjets->Clone("h_" + flavour + "_PR_" + variable);
      
  h_PR->Divide(h_tight_zjets, h_loose_zjets);


  // Draw
  //----------------------------------------------------------------------------
  TString title = flavour + " prompt rate";

  TCanvas* canvas = new TCanvas(title + " " + variable, title + " " + variable);
      
  canvas->SetGridx(setgrid);
  canvas->SetGridy(setgrid);

  Cosmetics(h_PR, "ep", xtitle, title, kBlack);


  // Save
  //----------------------------------------------------------------------------
  canvas->SaveAs(Form("%s/%s_PR_%s.png", pngdir.Data(), flavour.Data(), variable.Data()));
}


//------------------------------------------------------------------------------
// GetLepScale
//------------------------------------------------------------------------------
Float_t GetLepScale(TString flavour,
		    TString loose_tight,
		    TString variable,
		    TString xtitle,
		    TString units,
		    Float_t jetet)
{
  Float_t ratio = 1.;

  if (!performDataMC) return ratio;

  TString suffix = Form("%s_%.0fGeV", variable.Data(), jetet);

  TH1D* h_data  = (TH1D*)dataFR ->Get(btagdir + "FR/01_Zpeak/h_" + flavour + "_" + loose_tight + "_" + suffix);
  TH1D* h_zjets = (TH1D*)zjetsFR->Get(btagdir + "FR/01_Zpeak/h_" + flavour + "_" + loose_tight + "_" + suffix);
  TH1D* h_wjets = (TH1D*)wjetsFR->Get(btagdir + "FR/01_Zpeak/h_" + flavour + "_" + loose_tight + "_" + suffix);


  // Get the ratio
  //----------------------------------------------------------------------------
  if (h_zjets->Integral() > 0.) ratio = h_data->Integral() / h_zjets->Integral();


  // Rebin histograms and prepare canvas
  //----------------------------------------------------------------------------
  h_data ->Rebin(5);
  h_zjets->Rebin(5);
  h_wjets->Rebin(5);

  TString title = Form("%s %s %s when jet p_{T} > %.0f GeV", flavour.Data(), loose_tight.Data(), variable.Data(), jetet);

  TCanvas* canvas = new TCanvas(title + " " + variable, title + " " + variable);


  // Set xtitle and ytitle
  //----------------------------------------------------------------------------
  TString ytitle = (flavour.EqualTo("Muon")) ? "muons" : "electrons";

  ytitle = "events with " + loose_tight + " " + ytitle;

  Int_t precision = 0;

  if (precision > -1) ytitle = Form("%s / %s.%df", ytitle.Data(), "%", precision);

  ytitle = Form(ytitle.Data(), h_data->GetBinWidth(0));

  if (!units.Contains("NULL")) {
    xtitle = Form("%s [%s]", xtitle.Data(), units.Data());
    ytitle = Form("%s %s",   ytitle.Data(), units.Data());
  }


  // Cosmetics
  //----------------------------------------------------------------------------
  h_data->SetLineColor(kBlack);
  h_data->SetMarkerColor(kBlack);
  h_data->SetMarkerStyle(kFullCircle);

  h_zjets->SetLineColor(kGreen+2);
  h_zjets->SetFillColor(kGreen+2);
  h_zjets->SetFillStyle(1001);

  h_wjets->SetLineColor(kGray+1);
  h_wjets->SetFillColor(kGray+1);
  h_wjets->SetFillStyle(1001);

  h_zjets->SetTitle("");
  h_zjets->SetXTitle(xtitle);
  h_zjets->SetYTitle(ytitle);

  h_zjets->GetXaxis()->SetRangeUser(50., 130.);
  h_zjets->GetXaxis()->SetTitleOffset(1.5);
  h_zjets->GetYaxis()->SetTitleOffset(1.8);


  // Draw
  //----------------------------------------------------------------------------
  h_zjets->Draw("hist");
  h_data ->Draw("ep,same");
  h_wjets->Draw("ep,same");

  TString lepton_wp = (flavour.EqualTo("Muon")) ? muon_wp : ele_wp;

  DrawLatex(42, 0.940, 0.945, 0.035, 31, year_name[year_index] + " " + lepton_wp);

  DrawLatex(42, 0.905, 0.845, 0.035, 31, Form("data / Z+jets = %.3f", ratio));


  // Save
  //----------------------------------------------------------------------------
  canvas->SaveAs(Form("%s/%s_%s_%s_%.0fGeV.png",
		      pngdir.Data(),
		      flavour.Data(),
		      loose_tight.Data(),
		      variable.Data(),
		      jetet));


  // Return the lepton scale
  //----------------------------------------------------------------------------
  return ratio;
}


//------------------------------------------------------------------------------
// WriteFR
//------------------------------------------------------------------------------
void WriteFR(TString flavour,
	     Float_t jetet)
{
  TString suffix = Form("pt_eta_bin_%.0fGeV", jetet);

  
  // Read loose and tight histograms
  //----------------------------------------------------------------------------
  TH2D* h_loose_data  = (TH2D*)dataFR ->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix);
  TH2D* h_tight_data  = (TH2D*)dataFR ->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix);
  TH2D* h_loose_wjets = (TH2D*)wjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix);
  TH2D* h_tight_wjets = (TH2D*)wjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix);
  TH2D* h_loose_zjets = (TH2D*)zjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_loose_" + suffix);
  TH2D* h_tight_zjets = (TH2D*)zjetsFR->Get(btagdir + "FR/00_QCD/h_" + flavour + "_tight_" + suffix);


  // Prepare fake rate histograms
  //----------------------------------------------------------------------------
  TH2D* h_FR                 = (TH2D*)h_tight_data->Clone("h_" + flavour + "_FR");
  TH2D* h_FR_numerator       = (TH2D*)h_tight_data->Clone("h_" + flavour + "_FR_numerator");
  TH2D* h_FR_denominator     = (TH2D*)h_loose_data->Clone("h_" + flavour + "_FR_denominator");
  TH2D* h_FR_EWK             = (TH2D*)h_tight_data->Clone("h_" + flavour + "_FR_EWK");
  TH2D* h_FR_EWK_numerator   = (TH2D*)h_tight_data->Clone("h_" + flavour + "_FR_EWK_numerator");
  TH2D* h_FR_EWK_denominator = (TH2D*)h_loose_data->Clone("h_" + flavour + "_FR_EWK_denominator");


  // Do the math
  //----------------------------------------------------------------------------
  h_FR->Divide(h_FR_numerator, h_FR_denominator);

  if (Zsubtraction) h_FR_EWK_numerator->Add(h_tight_zjets, -lepscale);
  if (Wsubtraction) h_FR_EWK_numerator->Add(h_tight_wjets, -lepscale);

  if (Zsubtraction) h_FR_EWK_denominator->Add(h_loose_zjets, -lepscale);
  if (Wsubtraction) h_FR_EWK_denominator->Add(h_loose_wjets, -lepscale);

  h_FR_EWK->Divide(h_FR_EWK_numerator, h_FR_EWK_denominator);


  // Write
  //----------------------------------------------------------------------------
  TString lepton_wp = (flavour.EqualTo("Muon")) ? muon_wp : ele_wp;

  TFile *file = new TFile(Form("%s/%s/%sFR_jet%0.f.root",
			       outputdir.Data(),
			       lepton_wp.Data(),
			       flavour.Data(),
			       jetet),
			  "recreate");

  h_FR            ->Write("FR_pT_eta");
  h_FR_numerator  ->Write("FR_pT_eta_numerator");
  h_FR_denominator->Write("FR_pT_eta_denominator");

  h_FR_EWK            ->Write("FR_pT_eta_EWKcorr");
  h_FR_EWK_numerator  ->Write("FR_pT_eta_EWKcorr_numerator");
  h_FR_EWK_denominator->Write("FR_pT_eta_EWKcorr_denominator");

  file->Close();
}


//------------------------------------------------------------------------------
// WritePR
//------------------------------------------------------------------------------
void WritePR(TString flavour)
{
  TString lepton_wp = (flavour.EqualTo("Muon")) ? muon_wp : ele_wp;

  TH2D* h_loose_zjets = (TH2D*)zjetsPR->Get("PR/00/h_" + flavour + "_loose_pt_eta_PR");
  TH2D* h_tight_zjets = (TH2D*)zjetsPR->Get("PR/00/h_" + flavour + "_tight_pt_eta_PR");

  TH2D* h_PR = (TH2D*)h_tight_zjets->Clone("h_" + flavour + "_signal_pt_eta_bin");

  h_PR->Divide(h_tight_zjets, h_loose_zjets);


  // Write
  //----------------------------------------------------------------------------
  TFile* file = new TFile(Form("%s/%s/%sPR.root",
			       outputdir.Data(),
			       lepton_wp.Data(),
			       flavour.Data()),
			  "recreate");

  h_PR->Write();
  
  file->Close();
}


//------------------------------------------------------------------------------
// DrawLatex
//------------------------------------------------------------------------------
void DrawLatex(Font_t      tfont,
	       Float_t     x,
	       Float_t     y,
	       Float_t     tsize,
	       Short_t     align,
	       const char* text,
	       Bool_t      setndc)
{
  TLatex* tl = new TLatex(x, y, text);

  tl->SetNDC      (setndc);
  tl->SetTextAlign( align);
  tl->SetTextFont ( tfont);
  tl->SetTextSize ( tsize);

  tl->Draw("same");
}


//------------------------------------------------------------------------------
// DrawLegend
//------------------------------------------------------------------------------
TLegend* DrawLegend(Float_t x1,
		    Float_t y1,
		    TH1*    hist,
		    TString label,
		    TString option,
		    Float_t tsize,
		    Float_t xoffset,
		    Float_t yoffset)
{
  TLegend* legend = new TLegend(x1,
				y1,
				x1 + xoffset,
				y1 + yoffset);
  
  legend->SetBorderSize(    0);
  legend->SetFillColor (    0);
  legend->SetTextAlign (   12);
  legend->SetTextFont  (   42);
  legend->SetTextSize  (tsize);

  TString final_label = Form(" %s", label.Data());

  legend->AddEntry(hist, final_label.Data(), option.Data());
  legend->Draw();

  return legend;
}


//------------------------------------------------------------------------------
// Cosmetics
//------------------------------------------------------------------------------
void Cosmetics(TH1D*     hist,
	       Option_t* option,
	       TString   xtitle,
	       TString   ytitle,
	       Color_t   color,
	       Float_t   ymin,
	       Float_t   ymax)
{
  hist->Draw(option);

  hist->SetLineColor  (color);
  hist->SetLineWidth  (2);
  hist->SetMarkerColor(color);
  hist->SetMarkerStyle(kFullCircle);
  hist->SetTitle      ("");
  hist->SetXTitle     (xtitle);
  hist->SetYTitle     (ytitle);

  if (ymin > -999) hist->SetMinimum(ymin);
  if (ymax > -999) hist->SetMaximum(ymax);

  hist->GetXaxis()->SetTitleOffset(1.5);
  hist->GetYaxis()->SetTitleOffset(1.8);
}
