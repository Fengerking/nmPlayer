set curDir=`pwd`
cd $curDir/../iOS/v7
make clean
make
cp ./libvoNPWrap.a $curDir/../../../../../voRelease/iOS/static/v7/debug/
cp ./libvoNPWrap.a $curDir/../../../../../voRelease/iOS/static/v7/
cd $curDir