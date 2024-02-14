import os, sys, time, math

class JobSubmitter:
  def __init__(self, ROOTMacro):
    self.ROOTMacro = ROOTMacro

    # -- internal variable
    self.cwd = os.getcwd()
    self.baseDir = ""

    self.list_inputFile   = []
    self.list_nFilePerJob = []

  def Register(self, inputFile, nFilePerJob):
    self.list_inputFile.append( inputFile )
    self.list_nFilePerJob.append( nFilePerJob )

  def Submit(self):
    self.Make_Dir()

    nSample = len(self.list_inputFile)
    if nSample != len(self.list_nFilePerJob):
      print("nSample != len(self.list_nFilePerJob) .. nSample = %d, len(self.list_nFilePerJob) = %d" % nSample, len(self.list_nFilePerJob))
      sys.exit()

    f_hadd = open("%s/script_hadd_all.sh" % self.baseDir, "w")

    for i_sample in range(0, nSample):
      sampleJob = SampleJob()
      sampleJob.ROOTMacro = self.ROOTMacro
      sampleJob.motherDir = self.baseDir
      sampleJob.inputFile   = self.list_inputFile[i_sample]
      sampleJob.nFilePerJob = self.list_nFilePerJob[i_sample]

      sampleJob.Submit()

      f_hadd.write("cd %s\n" % sampleJob.sampleName)
      f_hadd.write("source script_hadd.sh\n")
      f_hadd.write("cd ..\n\n")

    f_hadd.close()

  def Make_Dir(self):
    dirName = self.Get_DirName()
    self.baseDir = "%s/%s" % (self.cwd, dirName)
    os.makedirs(self.baseDir)

  def Get_DirName(self):
    timeInfo = time.localtime()
    baseName = "CondorJob_%d%d%d" % (timeInfo.tm_year, timeInfo.tm_mon, timeInfo.tm_mday)

    dirName_final = ""
    for version in range(0, 100):
      dirName = "%s_v%03d" % (baseName, version)
      if not os.path.exists(dirName):
        dirName_final = dirName
        break

    return dirName_final


class SampleJob:
  def __init__(self):
    self.ROOTMacro = "" # -- C++ code
    self.motherDir = "" # -- mother directory
    self.inputFile = "" # -- .txt file with the paths to ntuples
    self.nFilePerJob = -1 # -- # files per job
 
    # -- internal variables
    self.baseDir = ""
    self.sampleName = ""

    self.dirPath_script = ""
    self.dirPath_log = ""

    self.bashScriptName = ""
    self.condorScriptName = ""

    self.nJob = -1 # -- total # job

  def Submit(self):
    self.Setup_Var()

    self.Make_Dirs()
    self.MakeScript_Bash()
    self.MakeScript_Condor()
    self.MakeScript_hadd()

    # cmd_condor = "condor_submit %s" % (self.condorScriptName)
    # print(cmd_condor)

    here = os.getcwd()
    os.chdir(self.baseDir) # -- submit the job after moving to the baseDir: so that the output file comes here
    # os.system("condor_submit -batch-name %s script/condorSub.txt" % self.sampleName)
    os.system("condor_submit script/condorSub.txt")
    os.chdir(here) # -- come back to the previous location

    print("Jobs are submitted for %s" % self.sampleName)

  def Setup_Var(self):
    if "/" in self.inputFile:
      self.sampleName = self.inputFile.split("/")[-1].split(".txt")[0]
    else:
      self.sampleName = self.inputFile.split(".txt")[0]

    self.baseDir = "%s/%s" % (self.motherDir, self.sampleName)
    self.dirPath_script = "%s/script" % (self.baseDir)
    self.dirPath_log = "%s/log" % (self.baseDir)

    self.nJob = self.Get_nJob()

    print("initialization for the job submission")
    print("  ROOTMacro   = %s" % self.ROOTMacro)
    print("  inputFile   = %s" % self.inputFile)
    print("  sampleName  = %s" % self.sampleName)
    print("  baseDir     = %s" % self.baseDir)
    print("  nFilePerJob = %d" % self.nFilePerJob)
    print("  nJob        = %d" % self.nJob)

  def Get_nJob(self):
    nFile = 0
    f = open(self.inputFile, "r")
    for line in f.readlines():
      if line.strip().endswith(".root"):
        nFile = nFile + 1

    f.close()

    if not (self.nFilePerJob > 0):
      print("nFilePerJob = %d is not > 0!" % self.nFilePerJob)
      sys.exit()

    return math.ceil(nFile / self.nFilePerJob)

  def Make_Dirs(self):
    os.makedirs(self.dirPath_script)
    os.makedirs(self.dirPath_log)

  def MakeScript_Bash(self):
    self.bashScriptName = "%s/job.sh" % (self.dirPath_script)

    f = open(self.bashScriptName, "w")
    f.write(
"""#!/bin/bash
echo "Will run:"
echo "Input file:      $1"
echo "Job number:      $2"
echo "# files per job: $3"

cmd="root -l -b -q '{ROOTMacro_}++(\\\"$1\\\", $2, $3)'"
echo "will run: $cmd"
eval $cmd

echo "done"

""".format(ROOTMacro_=self.ROOTMacro))
    f.close()

  def MakeScript_Condor(self):
    self.condorScriptName = "%s/condorSub.txt" % (self.dirPath_script)

    f = open(self.condorScriptName, "w")
    f.write(
"""universe    = vanilla
executable  = {bashScriptName_}
transfer_input_files = {cwd_}/{ROOTMacro_}
getenv      = True
output      = {dirPath_log_}/output_$(ProcId).txt
error       = {dirPath_log_}/error_$(ProcId).txt
log         = {dirPath_log_}/log_$(ProcId).txt
should_transfer_files = YES
when_to_transfer_output = ON_EXIT

Queue Arguments From (
""".format(bashScriptName_=self.bashScriptName, cwd_=os.getcwd(), ROOTMacro_=self.ROOTMacro, dirPath_log_=self.dirPath_log))

    for i_job in range(0, self.nJob):
      arg = "%s %d %d" % (self.inputFile, i_job, self.nFilePerJob)
      f.write(arg+"\n")

    f.write(")\n")

    f.close()

  def MakeScript_hadd(self):
    f = open("%s/script_hadd.sh" % self.baseDir, "w")
    cmd_hadd = "hadd %s/%s.root *.root" % (self.motherDir, self.sampleName)
    f.write("#!/bin/bash\n")
    f.write(cmd_hadd+"\n")
    f.close()




