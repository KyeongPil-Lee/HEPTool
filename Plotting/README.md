# Plotting Tools

## SimplePlotTools.h: collection of functions and classes to draw plots
* namespace: `PlotTool`

### Classes
* `HistCanvas`
* `HistCanvaswRatio`
* `HistStackCanvaswRatio`
* `Hist2DCanvas`
* `GraphCanvas`
* `GraphCanvaswRatio`

### Functions
* `TH1D* Get_Hist(Get_Hist(TString fileName, TString histName, TString histName_new = "")`
* `TH2D* Get_Hist2D(TString fileName, TString histName, TString histName_new = "" )`
* `TGraphAsymmErrors* Get_Graph(TString fileName, TString graphName, TString graphName_New = "" )`
* `TH1D* DivideEachBin_ByBinWidth( TH1D* h, TString HistName = "" )`
* `TH1D* MultiplyEachBin_byBinWidth( TH1D* h, TString HistName = "" )`
* `Bool_t IsRatio1( TH1D* h1, TH1D* h2)`
* `TH1D* Convert_GraphToHist( TGraphAsymmErrors *g )`
* `void Print_Histogram( TH1D* h, Bool_t NegativeCheck = kFALSE )`
* `void Print_Histogram2D( TH2D* h2D )`
* `TH1D* HistOperation(TString histName, TH1D* h1, TH1D* h2, TString operation)`
  * operation: `"+", "-", "*", "/"`
  * Warning: curerntly, the errors are not properly propagated now (the error is set to 0)
* `TH2D* Hist2DOperation(TString histName, TH2D* h1, TH2D* h2, TString operation)`
  * same with above, but for 2D
  * Warning: curerntly, the errors are not properly propagated now (the error is set to 0)
