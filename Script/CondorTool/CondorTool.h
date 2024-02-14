#include <fstream>
#include <iostream>
#include <cmath>

#include <TChain.h>
#include <TObjString.h>

namespace CondorTool {

vector<TString> GetVector_Line(TString inputFile) {
  vector<TString> vec_line;

  ifstream openFile( inputFile.Data() );
  cout << "===============================" << endl;
  cout << "Read " << inputFile << endl;
  cout << "===============================" << endl;

  if( openFile.is_open() ) {
    string line;
    while(getline(openFile, line)) {
      TString tstr_line = line;
      if( tstr_line.EndsWith(".root") ) // -- select only the line that ends with ".root"
        vec_line.push_back( tstr_line );
    }
    openFile.close();
  }
  else
    throw std::runtime_error("[GetVector_Line] "+inputFile + " is not able to open!");

  return vec_line;
}

void Add_NtupleToTChain(TChain* chain, const vector<TString>& vec_line, 
                        const Int_t i_job, const Int_t nFilePerJob, const Int_t nJob) {
  // -- sanity check
  if( i_job >= nJob )
    throw std::invalid_argument( TString::Format("i_job=%d should be smaller than nJob=%d!", i_job, nJob) );

  // -- start index
  // -- e.g. i_job == 1 --> index_start = 1 * 3 = 3
  Int_t index_start = i_job * nFilePerJob;
  // -- upper limit for the loop
  // -- if it is the last job: loop until the end
  // -- if not, loop until just before the start of the next job
  Int_t index_end = (i_job == nJob-1) ? (Int_t)vec_line.size() : (i_job+1)*nFilePerJob;

  for(Int_t i=index_start; i<index_end; ++i) {
    chain->Add( vec_line[i].Data() );
    cout << "  " << vec_line[i].Data() << endl;
  }
}

// -- when you want to constrain total # jobs
// -- inputs
// -- inputFile: path to the .txt file with the tree file paths
// -- i_job: job number for this job (0 ... nJob-1)
// -- nJob: total # jobs
// --> add the corresponding ntuples for this job
void Add_NtupleToTChain_nJob(TChain* chain, TString inputFile, Int_t i_job, Int_t nJob) {
  vector<TString> vec_line = GetVector_Line(inputFile);
  Int_t nFile = (Int_t)vec_line.size();

  // -- e.g. nFile = 10, nJob = 3 --> nFilePerJob = floor(3.33...) = 3
  Int_t nFilePerJob = floor((Double_t)nFile/nJob);

  Add_NtupleToTChain(chain, vec_line, i_job, nFilePerJob, nJob);
}

// -- when you constrain # files per job
void Add_NtupleToTChain_nFilePerJob(TChain* chain, TString inputFile, Int_t i_job, Int_t nFilePerJob) {
  vector<TString> vec_line = GetVector_Line(inputFile);
  Int_t nFile = (Int_t)vec_line.size();

  // -- e.g. nFile = 10, nFilePerJob = 3 --> nJob = 4 (3, 3, 3, 1)
  Int_t nJob = ceil( (Double_t)nFile/nFilePerJob );

  printf("[Add_NtupleToTChain_nFilePerJob] (nFile, nFilePerJob, nJob) = (%d, %d, %d)\n", nFile, nFilePerJob, nJob);

  Add_NtupleToTChain(chain, vec_line, i_job, nFilePerJob, nJob);
}

vector<TString> SplitString(TString theString, TString delimeter) {
  vector<TString> vec_str;

  TObjArray *objArr_token = theString.Tokenize(delimeter);
  for(Int_t i=0; i< objArr_token->GetEntries(); ++i) {
    vec_str.push_back( ((TObjString *)(objArr_token->At(i)))->String() );
  }

  return vec_str;
}

TString Get_SampleName(TString inputFile) {
  vector<TString> vec_str = SplitString(inputFile, "/");

  TString fileName = vec_str.back();

  return fileName.ReplaceAll(".txt", "");
}

TString Get_OutputFileName(TString inputFile, Int_t i_job) {
  TString sampleName = Get_SampleName(inputFile);

  return TString::Format("%s_%03d.root", sampleName.Data(), i_job);
}

};



// void test() {
  // TString fileName = "testFile.txt";
  // TChain *chain = new TChain("dummy");

  // Int_t nJob = 4;
  // for(Int_t i=0; i<nJob; ++i) {
  //   printf("[%d job]\n", i);
  //   Add_NtupleToTChain_nJob(chain, fileName, i, nJob);
  //   printf("\n");
  // }

  // Int_t nFilePerJob = 3;
  // Int_t nFile = (Int_t)GetVector_Line(fileName).size();
  // for(Int_t i=0; (nFilePerJob*(i))<nFile; ++i) {
  //   printf("[%d job]\n", i);
  //   Add_NtupleToTChain_nFilePerJob(chain, fileName, i, nFilePerJob);
  //   printf("\n");

  // }
// }