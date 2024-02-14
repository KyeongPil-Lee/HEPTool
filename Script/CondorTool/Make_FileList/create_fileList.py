import os, sys
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-d", "--dir", action="store", type="string", dest="dir", default="")
parser.add_option("-o", "--output", action="store", type="string", dest="output", default="")

def getList_rootFilePath(baseDir):
    list_rootFilePath = []

    # -- search for all subdirectories
    for (path, list_dirName, list_fileName) in os.walk(baseDir):
        for fileName in list_fileName:
            if ".root" in fileName:
                rootFilePath = "%s/%s" % (path, fileName)
                list_rootFilePath.append( rootFilePath )

    if len(list_rootFilePath) == 0:
        print "[getList_rootFilePath] no root file under %s ... exit" % baseDir
        sys.exit()

    list_rootFilePath.sort()

    return list_rootFilePath

def create_fileList_txt(baseDir, outputName):
    list_rootFilePath = getList_rootFilePath(baseDir)

    f_textFile = open(outputName, "w")
    for fileName in list_rootFilePath:
        f_textFile.write( fileName + "\n" )
    f_textFile.close()


if __name__ == "__main__":
    (options, args) = parser.parse_args()

    print("arguments:")
    print("directory:   %s" % options.dir)
    print("output name: %s" % options.output)

    if options.dir == "" or options.output == "":
        print("--> the arguments are not corretly set")
        sys.exit()

    create_fileList_txt(options.dir, options.output)