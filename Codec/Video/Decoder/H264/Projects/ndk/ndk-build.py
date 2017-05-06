import os,sys,glob,re,subprocess,filecmp,time

#set the correct path
sourcePath  = "../../Sources"
buildPath   = "./"
preBuildPath = "./v6/release/"
incPath = "../../../../../../Include/"
libPath     = [
                "../../../../../../../voRelease/Android_ndk/so/",
                #"/home/huang_bafeng/Numen/voRelease/Customer/Google/eclair/so/v6",
                #"/home/huang_bafeng/Numen/voRelease/Customer/Google/eclair/so/v7",
              ]
makeFileList = [

                "./v6/debug/HP_LIB/jni/",
               "./v6/release/BP_LIB/jni/",
                "./v6/release/HP_LIB/jni/",
                "./v7/debug/BP_LIB/jni/",
                "./v7/debug/HP_LIB/jni/",
                "./v7/release/BP_LIB/jni/",
                "./v7/release/HP_LIB/jni/",
               ]



cleanBuild = 0
revisionStr=" "
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
    curTime2 = time.strftime("  %y/%m/%d %H:%M", now)
    cmd ="svn ci \""+checkin_dir + "\" -m \"" + checkin_comment+ "  "+ revisionStr + " "+curTime2 + "\""
    return checkExeResult(cmd)



def svnCheckinHeadFile(checkin_comment,dir):
    svnCheckin(checkin_comment,dir)

def svnCheckinLib(checkin_comment,dirs):

    for dir in dirs:
        svnCheckin(checkin_comment,dir)

def svnCheckinMakefile(checkin_comment,dir):
     svnCheckin(checkin_comment,dir)

def ModifyRevision(fileList,revisionID):

    for file in fileList:
        file = file +"Application.mk"
        f = open(file, "r")
        if(f):
            content = f.read()
            print content
           
            content = re.sub(r"VOSRCNO\s*?=\s*.*",revisionID,content)
            #content[-1] = revisionID
            print content
           
            f.close()
            f = open(file, "w")
            #for line in content:
            f.write(content)
            f.close()
           


def CheckinAll(checkin_comment):
    #check in src first
    outInfo = svnCheckin(checkin_comment,sourcePath) #["test","committed revision 9982"] #
    #print outInfo
    global revisionStr
    if(outInfo!=[]):
        m = re.search(r"\d+",outInfo[-1])
        if(m):
         revisionID =  m.group()
         revisionStr = str(revisionID)
         #print revisionStr
         #return
         ModifyRevision(makeFileList,"VOSRCNO := "+revisionStr)
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

def PureCheckIn(checkin_comment):

     svnCheckin(checkin_comment,sourcePath)
     svnCheckinLib(checkin_comment,libPath)
     #checkin makefile also
     svnCheckinMakefile(checkin_comment, buildPath)
     svnCheckinHeadFile(checkin_comment, incPath)

def IsBuildOK(outInfo):
    buf = "".join(outInfo[-5:])
    # libvoH264Dec.so => libs/armeabi/libvoH264Dec.so"
    if(buf.find("Install        :")!=-1):
        return 1
    else:
        return 0

def PreBuild():
    cmd1 = "ndk-build -C " + preBuildPath + "BP_LIB/jni/" + " -B"
    checkExeResult(cmd1,buildPath)
    cmd1 = "ndk-build -C " + preBuildPath + "HP_LIB/jni/" + " -B"
    outInfo = checkExeResult(cmd1,buildPath)
    if(IsBuildOK(outInfo)):
        print "preBuild succeed!!\n"
        return 1
    else:
        print "preBuild failed!!\n"
        return 0

def Build():

    checkExeResult("make;make devel",buildPath)

pattern = re.compile(r"mophy (\d+) ")
def Test(line1):
     #ModifyRevision(makeFileList,"VOSRCNO := 1057")
     m = re.match(pattern,line1)
     if m:
        return int(m.group(1))


if __name__=="__main__":


     if 1:#for test
        #svnCheckin("just test",sourcePath)
        ModifyRevision(makeFileList,"VOSRCNO := 20")
     else:
         if(len(sys.argv)>2):
            print "pure check in\n"
            PureCheckIn(sys.argv[1])
         else:
             if(PreBuild()):
                CheckinAll(sys.argv[1])




