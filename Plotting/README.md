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

Basic structure is same for all above classes:
```
TString canvasName = "c_variable";
PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
canvas->SetTitle("var", "# entry");

canvas->Register(h1, "1st hist", kBlack);
canvas->Register(h2, "2nd hist", kRed);

canvas->SetAutoRangeY();
// canvas->SetRangeX(minX, maxX);
// canvas->SetRangeY(minY, maxY);

canvas->Latex_CMSInternal();
canvas->Latex_LumiEnergy(10.0, 13);
canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Test}}");

canvas->Draw();
```
<br>
Example codes are available in `Example` directory.
<br>

### Functions
* `TH1D* Get_Hist(TString fileName, TString histName, TString histName_new = "")`
  * Get a 1D histogram in a .root file
<br>

* `TH2D* Get_Hist2D(TString fileName, TString histName, TString histName_new = "" )`
  * Get a 2D histogram in a .root file
<br>

* `TGraphAsymmErrors* Get_Graph(TString fileName, TString graphName, TString graphName_New = "" )`
  * Get a graph in a .root file
<br>

* `TH1D* DivideEachBin_ByBinWidth( TH1D* h, TString HistName = "" )`
  * Divide each bin content by its width
  * Useful when the histogram is converted from cross section -> differential cross section
<br>

* `TH1D* MultiplyEachBin_byBinWidth( TH1D* h, TString HistName = "" )`
  * Multiply each bin content by its width
  * Useful when the histogram is converted from differnetial cross section -> cross section
<br>

* `Bool_t IsRatio1( TH1D* h1, TH1D* h2)`
  * Print whether two histograms are exactly same (the ratio of two is exactly 1.0)
<br>

* `TH1D* Convert_GraphToHist( TGraphAsymmErrors *g )`
  * Convert a graph to histogram
  * Asymmetric error on a point: take the larger error when it is converted to the bin error in the histogram
<br>

* `void Print_Histogram( TH1D* h, Bool_t NegativeCheck = kFALSE )`
  * Print the histogram information bin by bin (bin content, error, relative error, etc)
<br>

* `void Print_Histogram2D( TH2D* h2D )`
  * Print the 2D histogram information bin by bin (bin content, error, relative error, etc)
<br>

* `TH1D* HistOperation(TString histName, TH1D* h1, TH1D* h2, TString operation)`
  * operation: `"+", "-", "*", "/"`
  * Warning: curerntly, the errors are not properly propagated now (the error is set to 0)
<br>

* `TH2D* Hist2DOperation(TString histName, TH2D* h1, TH2D* h2, TString operation)`
  * same with above, but for 2D
  * Warning: curerntly, the errors are not properly propagated now (the error is set to 0)
<br>

* `TH1D* MakeHist_fromVector(TString histName, vector<Double_t> vec_binEdge)`
  * make TH1D* using vector, not array for binning
<br>

* `TH2D* MakeHist2D_fromVector(TString histName, vector<Double_t> vec_binEdgeX, vector<Double_t> vec_binEdgeY)`
  * make TH2D* using vector, not array for binning
<br>

