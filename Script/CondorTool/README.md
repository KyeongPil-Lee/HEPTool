# (Simple) condor job submission framework

## Overview

Simple scripts to easily submit the condor jobs for ROOT marcos, in a way similar to "CRAB" style

1. Write the job information in a python configuration (`submit.py`)

   * ROOT marcro name

   * Full ntuple list (in a form of .txt file)

   * number of files per job

2. Submit

```bash
python submit.py
```

It will create a dedicated directory & submit the jobs in condor.

Once the job is finished, you can run `script_hadd_all.sh` in the directory to merge and collect all output `.root` file

## Prerequisite

### Text files that contains the paths to the ntuples

* The text files can be automatically generated using `Make_FileList/create_fileList.py`
  * Check `script_create_fileList.sh` to produce many text files for each sample

### Environment setup

Setup to make `submit.py` recognize the classes in `CondorJobSubmitter.py`

1. Include your current working directory in `PYTHONPATH`

   * Example:
     ```bash
     export CWDPATH=$(pwd)
     export PYTHONPATH=${CWDPATH}:${PYTHONPATH}
     ```

2. Put `__init__.py` in a directory with the `CondorJobSubmitter.py`

3. Modify the path to `CondorJobSubmitter.py` in `submit.py` to be consistent with your paths

   * Relative path to your `PYTHONPATH`
   * `from Some.Path.To.CondorJobSubmitter import *`

## Structure of ROOT Macro

ROOT Macro should follow a specific style to make it run via this framework

1. Argument of the ROOT macro: `TString inputFile, Int_t i_job, Int_t nFilePerJob`

   * e.g. `void MakeHist_Isolation(TString inputFile, Int_t i_job, Int_t nFilePerJob)`
   * The order is important
   * No more arguments are allowed

2. Add ntuples into TChain by this way:
   `CondorTool::Add_NtupleToTChain_nFilePerJob(chain, inputFile, i_job, nFilePerJob);`

   * example:
     ```C++
     TChain* chain = new TChain("ntupler/ntuple");
     if( inputFile == "test" )  chain->Add("../../../Example/ntuple_example.root");
     else                       CondorTool::Add_NtupleToTChain_nFilePerJob(chain, inputFile, i_job, nFilePerJob);
     ```

   * `CondorTool.h` should be included to use `CondorTool::Add_NtupleToTChain_nFilePerJob` function

3. The output ROOT file name should be determined by this function:
   `CondorTool::Get_OutputFileName(inputFile, i_job)`

   * Example:
     ```c++
     TString outputFName = (inputFile == "test") ? "test.root" : CondorTool::Get_OutputFileName(inputFile, i_job);
     ```

   * `CondorTool.h` should be included to use `CondorTool::Get_OutputFileName` function

   * Only **one** output ROOT file is allowed