from CondorJobSubmitter import *

submitter = JobSubmitter("example.cxx")

nFilesPerJob = 1

submitter.Register("./Catalog/RelValZMM.txt", nFilesPerJob)
submitter.Register("./Catalog/RelValTTbar.txt", nFilesPerJob)
submitter.Register("./Catalog/RelValQCDMuEnriched.txt", nFilesPerJob)

submitter.Submit()

