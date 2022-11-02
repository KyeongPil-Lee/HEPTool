# -- usage: python CRAB_Status.py <CRAB directory>

#!/usr/bin/env python
import subprocess
import os
import sys
import time

if len(sys.argv) == 2:
    crabDirBase = sys.argv[1]
else:
    print "usage: python CRAB_Status.py <CRAB directory>"
    sys.exit()

print "CRAB directory: ", crabDirBase

proxy = subprocess.check_output(['voms-proxy-info', '-p']).strip()
if proxy == "":
    print "proxy path is not found: please run the command first:"
    print "voms-proxy-init --voms"
    sys.exit()

print "proxy : ", proxy

FileList = os.listdir("./%s" % crabDirBase)
List_CRABDir = []
print "available crabDir list: "
for filename in FileList:
    if "crab_" in filename:
        print "'"+filename+"',"
        List_CRABDir.append( filename )

List_CRABDir.sort()

ResubmtCMD = []
CompletedList = []
UnknownList = []
OthersList = []

theTime = time.strftime('%Y%m%d_%H%M%S', time.localtime(time.time()))
f_log = open( "CRABStatusLog_v%s.txt" % (theTime), 'w' )

for crabDir in List_CRABDir:
    crabDirPath = "%s/%s" % (crabDirBase, crabDir)
    # outputDir = "v" + crabDir.split("_v")[1]
    
    cmd = 'crab status --verboseErrors "'+crabDirPath+'" --proxy='+proxy
    result = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    (stdout, stderr) = result.communicate()
    print "#" * 100
    print cmd+'\n'
    print "[stdout]"
    print stdout
    print "[stderr]"
    print stderr
    print "#" * 100 +'\n\n'

    f_log.write("#" * 100)
    f_log.write('\n')
    f_log.write(cmd+'\n')
    f_log.write("[stdout]\n")
    f_log.write(stdout)
    f_log.write("[stderr]\n")
    f_log.write(stderr)
    f_log.write('\n')
    f_log.write("#" * 100 +'\n\n\n')

    if "jobs failed with" in stdout or "jobs failed in" in stdout:
        ResubmtCMD += ['crab resubmit '+crabDirPath+' --proxy='+proxy]

    elif "COMPLETED" in stdout:
        CompletedList.append( crabDir )

    elif "UNKNOWN" in stdout:
        UnknownList.append( crabDir )

    else:
        OthersList.append( crabDir )

print "[Completed list]"
f_log.write("[Completed list]\n")
for one in CompletedList:
    print "'"+one+"',"
    f_log.write("'"+one+"',"+"\n")

print "[Unknown list]"
f_log.write("[Unknown list]\n")
for one in UnknownList:
    print "'"+one+"',"
    f_log.write("'"+one+"',"+"\n")

print "[Others]"
f_log.write("[Others]\n")
for one in OthersList:
    print "'"+one+"',"
    f_log.write("'"+one+"',"+"\n")

print "\n[CRAB jobs which should be resubmitted]"
f_log.write("\n[CRAB jobs which should be resubmitted]\n")
for theCMD in ResubmtCMD:
    print theCMD
    f_log.write(theCMD+'\n')

f_log.write("\n")
f_log.close()