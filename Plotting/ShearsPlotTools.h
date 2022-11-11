#include "SimplePlotTools.h"

class ShearsPlotTool {
public:

  void Set_BaseDir(TString dir) { baseDir_ = dir; }

  void Set_Data(TString tag, Double_t lumi)     { tag_data_ = tag; lumi_ = lumi; }
  void Set_SignalMC(TString tag) { tag_signalMC_ = tag; }
  void Set_BackgroundMC(vector<TString> vec_tag) { vec_tag_bkgMC_ = vec_tag; }
  void Register_MergedMC(TString mergedTag, vector<TString> vec_tag) { map_mergedTag_vecTag_.insert( std::make_pair(mergedTag, vec_tag) ); }

  void Set_Rebin(Int_t nRebin) { nRebin_ = nRebin; }

  void Set_Tag_Canvas(TString tag) { tag_canvas_ = tag; }

  // -- customize
  void Set_Prefix(TString prefix) { prefix_ = prefix; }

  void Produce(TString histName) {
    // -- get histograms
    TH1D* h_data     = Get_ShearsHist(histName, tag_data_);
    TH1D* h_signalMC = Get_ShearsHist(histName, tag_signalMC_, kTRUE);
    RemoveNegativeBins(h_signalMC, tag_signalMC_);

    if( nRebin_ != 1 ) {
      h_data->Rebin( nRebin_ );
      h_signalMC->Rebin( nRebin_ );
    }

    vector<TH1D*> vec_hist_bkgMC;
    for(auto& tag_bkgMC : vec_tag_bkgMC_ ) {
      TH1D* h_bkgMC = nullptr;
      if( map_mergedTag_vecTag_.find(tag_bkgMC) != map_mergedTag_vecTag_.end() ) // -- if it is found in the map:
        h_bkgMC = Get_MergedMC(histName, tag_bkgMC);
      else
        h_bkgMC = Get_ShearsHist(histName, tag_bkgMC, kTRUE);

      if( nRebin_ != 1 )
        h_bkgMC->Rebin(nRebin_);

      RemoveNegativeBins(h_bkgMC, tag_bkgMC);

      vec_hist_bkgMC.push_back( h_bkgMC );
    }

    TString canvasName = histName;
    if( canvasName.BeginsWith("h_") ) canvasName.ReplaceAll("h_", "c_");
    else                              canvasName = "c_" + canvasName;

    if( !(tag_canvas_ == "") ) canvasName = canvasName + "_" + tag_canvas_;

    Bool_t isLogX = Get_IsLogX(histName);

    PlotTool::HistStackCanvaswRatio* canvas = new PlotTool::HistStackCanvaswRatio(canvasName, isLogX, 1);
    canvas->Ratio_Reversed();
    canvas->SetTitle( TitleX(histName), "# events", "MC/data" );

    canvas->RegisterData(h_data, "Data", kBlack);

    Int_t nBkg = vec_hist_bkgMC.size();
    for(Int_t i=0; i<nBkg; i++) {
      TString tag = vec_tag_bkgMC_[i];
      TH1D* h_bkgMC = vec_hist_bkgMC[i];
      canvas->Register(h_bkgMC, Get_Legend(tag), Get_Color(tag));
    }
    canvas->Register(h_signalMC, "Z/#gamma*#rightarrow#mu#mu", kGreen-8);

    // canvas->SetRangeX(81, 101);

    canvas->SetAutoRangeY();
    if( histName.Contains("mass_inc0jet") ) canvas->SetRangeRatio(0.94, 1.06);
    if( histName.Contains("mass_wide_range_inc0jet") ) canvas->SetRangeRatio(0.7, 1.3);

    canvas->ShowDataMCRatio();

    canvas->SetLegendColumn(2);
    canvas->SetLegendPosition(0.60, 0.80, 0.95, 0.95);

    canvas->Latex_CMSInternal();
    canvas->Latex_LumiEnergy(lumi_/1000.0, 13);

    canvas->Draw();

    h_data_         = (TH1D*)h_data->Clone();
    h_signalMC_     = (TH1D*)h_signalMC->Clone();
    copy(vec_hist_bkgMC.begin(), vec_hist_bkgMC.end(), back_inserter(vec_hist_bkgMC_));
  }

  // -- when you want to get the histogram exactly used in the plot
  // -- it should be called after Produce()
  TH1D* Get_Hist_Data()     { return h_data_; };
  TH1D* Get_Hist_SignalMC() { return h_signalMC_; };
  vector<TH1D*> Get_Hist_BkgMC() { return vec_hist_bkgMC_; };

private:
  TString baseDir_ = "";

  TString tag_data_ = "";
  Double_t lumi_ = -1; // -- in /pb;
  TString tag_signalMC_ = "";
  vector<TString> vec_tag_bkgMC_;
  map<TString, vector<TString>> map_mergedTag_vecTag_;

  TString tag_canvas_ = "";

  TString prefix_ = "dyjets-";

  Int_t nRebin_ = 1;

  TH1D* h_data_ = nullptr;
  TH1D* h_signalMC_= nullptr;;
  vector<TH1D*> vec_hist_bkgMC_;

  TString TitleX(TString histName) {
    TString title = "undefined";
    if( histName == "mass_inc0jet" ) title = "m(ll) [GeV]";
    if( histName == "mass_inc0jet_BB" ) title = "m(ll) [GeV] (Barrel-Barrel)";
    if( histName == "mass_inc0jet_BE" ) title = "m(ll) [GeV] (Barrel-Endcap)";
    if( histName == "mass_inc0jet_EE" ) title = "m(ll) [GeV] (Endcap-Endcap)";
    if( histName == "mass_wide_range_inc0jet" ) title = "m(ll) [GeV]";
    if( histName == "mass_wide_range_inc0jet_BB" ) title = "m(ll) [GeV] (Barrel-Barrel)";
    if( histName == "mass_wide_range_inc0jet_BE" ) title = "m(ll) [GeV] (Barrel-Endcap)";
    if( histName == "mass_wide_range_inc0jet_EE" ) title = "m(ll) [GeV] (Endcap-Endcap)";

    if( histName == "pt_inc0jet" ) title = "p_{T}(ll) [GeV]";

    return title;
  }

  TH1D* Get_ShearsHist(TString histName, TString tag, Bool_t doNorm=kFALSE) {
    TString fileName = TString::Format("%s/%s%s.root", baseDir_.Data(), prefix_.Data(), tag.Data());
    TH1D* h = PlotTool::Get_Hist(fileName, histName);

    if( doNorm ) {
      TH1D* h_jobInfo = PlotTool::Get_Hist(fileName, "_job_info");
      Double_t sumWeight = h_jobInfo->GetBinContent(2);
      Double_t xSec = Get_XSec(fileName);

      Double_t normFactor = (lumi_ * xSec) / sumWeight;
      h->Scale( normFactor );
    }

    return h;
  }

  Double_t Get_XSec(TString fileName) {
    TVectorD* job_info_average = nullptr;
    TFile* f = TFile::Open(fileName);
    f->GetObject("_job_info_average", job_info_average);
    f->Close();

    return (*job_info_average)[1];
  }

  TH1D* Get_MergedMC(TString histName, TString mergedTag) {
    cout << "Make merged MC for " << mergedTag << "..." << endl;
    vector<TString> vec_tag = map_mergedTag_vecTag_[mergedTag];
    TH1D* h_merged = nullptr;
    for(auto& tag : vec_tag ) {
      cout << " add " << tag << endl;
      TH1D* h_MC = Get_ShearsHist(histName, tag, kTRUE);
      if( h_merged == nullptr ) h_merged = (TH1D*)h_MC->Clone();
      else                      h_merged->Add( h_MC );
    }
    cout << "done" << endl;
    cout << endl;

    return h_merged;
  }

  TString Get_Legend(TString tag) {
    TString legend = "";
    if( tag == "TT" ) legend = "t#bar{t}";
    if( tag == "singleTop" ) legend = "single t";
    if( tag == "TauTau" ) legend = "Z/#gamma*#rightarrow#tau#tau";
    if( tag == "WJetsToLNu" ) legend = "W+jets";
    if( tag == "VV" ) legend = "VV";

    return legend;
  }

  Int_t Get_Color(TString tag) {
    Int_t color = 1;
    if( tag == "TT" ) color = kBlue;
    if( tag == "singleTop" ) color = kMagenta;
    if( tag == "TauTau" ) color = kCyan + 2;
    if( tag == "WJetsToLNu" ) color = kOrange;
    if( tag == "VV" ) color = kRed + 1;

    return color;
  }

  Bool_t Get_IsLogX(TString histName) {
    Bool_t isLogX = kFALSE;
    if( histName == "pt_inc0jet" ) isLogX = kTRUE;
    if( histName.Contains("mass_wide_range") ) isLogX = kTRUE;

    return isLogX;
  }

  void RemoveNegativeBins( TH1D* h, TString tag )
  {
    Bool_t isFound = kFALSE;
    // -- Remove negative bins -- //
    for(Int_t i=0; i<h->GetNbinsX(); i++)
    {
      Double_t content = h->GetBinContent(i+1);

      if( content < 0 )
      {
        isFound = kTRUE;
        h->SetBinContent(i+1, 0);
        h->SetBinError(i+1, 0);
      }
    }

    if( isFound )
      cout << "*** Negative bins are found in " << tag << "... negative bins are set to 0 ***" << endl;
  }

};

void Example() {

  TString baseDir = "./file_MM";
  TString dataName = "data";
  TString signalMCName = "DYJets";
  // -- order: stack order (the last element will be on top of the stack)
  vector<TString> vec_bkgMCName = {"VV", "WJetsToLNu", "TauTau", "singleTop", "TT"};  
  vector<TString> vec_histName = {"mass_inc0jet"};

  for(auto& histName : vec_histName ) {
    ShearsPlotTool* tool = new ShearsPlotTool();
    tool->Set_BaseDir(baseDir); // -- the directory with the shears output file

    tool->Set_Data(dataName, 19520.0); // -- data tag

    tool->Set_SignalMC(signalMCName); // -- signal tag

    tool->Register_MergedMC("VV", {"WWTo2L2Nu", "WZ", "ZZ"});
    tool->Register_MergedMC("singleTop", {"ST_s-channel", "ST_t-channel_antitop", "ST_t-channel_top", "ST_tW_antitop", "ST_tW_top"});
    tool->Set_BackgroundMC(vec_bkgMCName); // -- vector of background tags

    // tool->Set_Rebin(3);

    tool->Produce(histName);
  }
}