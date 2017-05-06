import os,sys,glob,re,subprocess,filecmp,time

#set the correct path
sourcePath  = "../../Sources"
buildPath   = "./"
libPath     = [
                "../../../../../../../voRelease/Customer/Google/eclair/so/",
                #"/home/huang_bafeng/Numen/voRelease/Customer/Google/eclair/so/v6",
                #"/home/huang_bafeng/Numen/voRelease/Customer/Google/eclair/so/v7",
              ]
incPath = "../../../../../../Include/"
makeFileList = [

                #"./eclairv4/non_baseline/",
                "./eclairv6/non_baseline/",
                "./eclairv7/non_baseline/",
                "./eclairv6Debug/non_baseline/",
                "./eclairv7Debug/non_baseline/",
               ]

cleanBuild = 0
revisionStr=""
def checkExeResult(cmd,workingpath=None):

    print cmd
    p = subprocess.Popen(cmd,stdout = subprocess.PIPE, stderr=subprocess.STDOUT, cwd=workingpath , shell=True)
    outInfo = []
    while True:
        line = p.stdout.readline()
        if not line:
            break
        print line
        outInfo.append(line);
    return outInfo

def svnCheckin(checkin_comment,checkin_dir):

    makewrite = "chmod 777 "+ checkin_dir + " -R * "
    checkExeResult(makewrite)
    cleanup = "svn cleanup "+ checkin_dir
    checkExeResult(cleanup)
    now = time.localtime(time.time())
    curTime = time.strftime("  %y/%m/%d %H:%M", now)
    cmd ="svn ci \""+checkin_dir + "\" -m \"" + checkin_comment+ "  "+ revisionStr + " "+curTime + "\"";
    return checkExeResult(cmd)



def svnCheckinHeadFile(checkin_comment,dir):
    svnCheckin(checkin_comment,dir)

def svnCheckinLib(checkin_comment,dirs):

    for dir in dirs:
        svnCheckin(checkin_comment,dir)

def svnCheckinMakefile(checkin_comment,dir):
     svnCheckin(checkin_comment,dir)

def GetCurRevision(file):
    f = open(file, "r")
    if(f):
        content = f.read()
        #print content
        id = re.search(r"VOSRCNO:=(\d+)",content).group(1)
        #print content
        f.close()
        return id

def ModifyRevision(fileList,revisionID):

    for file in fileList:
        file = file +"Makefile"
        f = open(file, "r")
        if(f):
            content = f.read()
            #print content
            content = re.sub(r"VOSRCNO:=.*",revisionID,content)
            #print content
            f.close()
            f = open(file, "w")
            f.write(content)
            f.close()



def CheckinAll(checkin_comment):
    #check in src first
    outInfo = svnCheckin(checkin_comment,sourcePath) #["test","committed revision 9982"] #
    #print outInfo
    global revisionStr
    if(outInfo!=[]):
        m = re.search(r"Committed revision (\d+)",outInfo[-1])
        if(m):
         revisionStr =  m.group(1)
         print "the new revision is %s\n"%revisionStr
         ModifyRevision(makeFileList,"VOSRCNO:="+revisionStr)
         #build again and checkin lib
         Build()
         svnCheckinLib(checkin_comment,libPath)
         #checkin makefile also
         svnCheckinMakefile(checkin_comment, buildPath)
         svnCheckinHeadFile(checkin_comment, incPath)
        else:
         print "error: there is no revision ID"
    else:
         print "error: there is no new code change"

def Build():

    if(cleanBuild):
        checkExeResult("make clean",buildPath)

    checkExeResult("make;make devel",buildPath)

def Test(checkin_comment):

    outInfo = svnCheckin(checkin_comment,sourcePath) #["test","committed revision 9982"] #
    #print outInfo
    global revisionStr
    if(outInfo!=[]):
        m = re.search(r"Committed revision (\d+)",outInfo[-1])
        if(m):
         revisionStr =  m.group(1)
         print "the new revision is %s\n"%revisionStr
         ModifyRevision(makeFileList,"VOSRCNO:="+revisionStr)

        else:
         print "error: there is no revision ID"
    else:
         print "error: there is no new code change"

def PureCheckin(checkin_comment):
     checkin_comment += "  " + GetCurRevision("./eclairv7/non_baseline/Makefile") +"  "
     svnCheckin(checkin_comment,sourcePath)
     svnCheckinLib(checkin_comment,libPath)
     #checkin makefile also
     svnCheckinMakefile(checkin_comment, buildPath)
     svnCheckinHeadFile(checkin_comment, incPath)

def IsBuildOK(outInfo):
    buf = "".join(outInfo[-5:])
    # libvoH264Dec.so => libs/armeabi/libvoH264Dec.so"
    if(buf.find("/arm-eabi-strip libvoH264Dec.so")!=-1):
        return 1
    else:
        return 0

def PreBuild():
    #-$(MAKE) -C ./eclairv7/baseline
	#$(MAKE) -C ./eclairv7/non_baseline
    checkExeResult("make -C ./eclairv7/baseline",buildPath)
    outInfo = checkExeResult("make -C ./eclairv7/non_baseline",buildPath)
    if(IsBuildOK(outInfo)):
        print "preBuild succeed!!\n"
        return 1
    else:
        print "preBuild failed!!\n"
        return 0

if __name__=="__main__":


     pureCheckin = 0
     comment = sys.argv[1]
     if(len(sys.argv)>2):
         cleanBuild =  sys.argv[2]=="-c"
         pureCheckin = sys.argv[2]=="-p"
     else:
         cleanBuild = 0
     if 0:
        Test(comment)
     else:
         if pureCheckin==0:
            if PreBuild():
               CheckinAll(comment)
         else:
            PureCheckin(comment)




