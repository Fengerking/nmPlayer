#!/bin/bash
#program:
#        Test H265 decoder for all platforms
#History:
#2011/05/12        Huaping create
#2013/02/25        Lina modify
#2013/03/01        implement by functions 
#useage: Please copy candidata files into autoRelease\autoTest\input 

# define usage function
usage(){
	#display usage
	echo "--------------------------------------------------------------"
	echo "           auto-test for  h265  decoder "
	echo "--------------------------------------------------------------"
	echo ""
	echo "Usage: " $MAKE " [version] "	
        echo " "
        echo "For e.g.:"
        echo "       "           test_verctors  ref  WIN32
        echo " "
}

# define ChangePrefix function 
# outpath=$(ChangePrefix "./input/customer/AVG/i_main/BlowingBubbles_416x240_50_qp37.bin" "./output")
ChangePrefix(){       
        in_string=$1
        to_prefix=$2
        in_string=${in_string:2}
        out_string=$to_prefix
       
        len=0
        for varnew in $(echo $in_string | fold -w1); do
        if [ $varnew != "/" ]; then
          let len=len+1          
        elif [ $varnew == "/" ]; then
          break
        fi
        done
        
        out_string=$out_string${in_string:len}  
        echo "$out_string"      
}

# define ChangePostfix function 
# outpath=$(ChangePostfix $outpath ".yuv")
ChangePostfix(){     
  
        out_string=$1
       
        while [ 1 ];
        do
          len=${#out_string}
          let len=len-1
          temp=${out_string:0-1:1}
          if [ $temp != "." ]; then
            out_string=${out_string:0:$len}
          elif [ $temp == "." ]; then
            out_string=${out_string:0:$len}
            break
          fi
        done
        
        out_string=$out_string$2
        echo "$out_string"      
}


# define GetDir function 
# out_dir=$(GetDir $outpath )
GetDir(){   
       
        out_dir=$1
       
        while [ 1 ];
        do
          len=${#out_dir}
          let len=len-1
          temp=${out_dir:0-1:1}
          if [ $temp != "/" ]; then
            out_dir=${out_dir:0:$len}
          elif [ $temp == "/" ]; then
            out_dir=${out_dir:0:$len}
            break
          fi
        done        
        
        echo "$out_dir"      
}


mode=$1      #need refdata or not   value: ref or noRef
version=$2   #platform  value: WIN32 ANDROIDNDKV6 ANDROIDNDKV7 IOSV6 IOSV7

usage    

#set global variable
INFIR=input
OUTDIR=output
TESTUTIL_DIR=tools
REFDIR=ref
LOG=./testResult/log
FAIL_DIR=./testResult/unsupport
CONFIG=../cfg
REF_DECODER="./TAppDecoder.exe"
TEST_DECODER="./voH265DecSample.exe"
CHECKRESULT="./fileDiff"
ANDROID_TEST_DIR="/data/local/tmp"
#cpu_numbers=([0]="1  " [1]="2  " [2]="4  ")
cpu_numbers=(1)


rm *.txt
rm -rf ./testResult
rm -rf $OUTDIR
if [ "$mode" == "noRef" ]; then
rm -rf $REFDIR
fi
touch h265dec_log.txt


if [ "$version" == "ANDROIDNDKV7" ]; then
adb push ./tools/voH265DecSample $ANDROID_TEST_DIR/
adb push ./tools/libvoH265Dec.so $ANDROID_TEST_DIR/

elif [ "$version" == "IOSV7" ]; then 
echo -e "Start create sdk for IOSV7! \n"
#toDo

fi

mkdir ref
mkdir output
mkdir testResult
cd testResult
mkdir log
mkdir unsupport
cd ../

find ./$INFIR -type f  > temp_list.ini
aa="`cat temp_list.ini`"

for var in $aa; do

  cpu_i=0
  one_input_item=$var
  len_cpu_numbers=${#cpu_numbers}
  
  while [ $cpu_i -lt $len_cpu_numbers ]
  do
  cpu=${cpu_numbers[$cpu_i]} 

  inpath=$one_input_item
  echo "input item $var len_cpu_numbers $len_cpu_numbers"
  
  #loop-step1: set variables
  #set outpath
  outpath=$(ChangePrefix $inpath ./$OUTDIR)
  outpath=$(ChangePostfix $outpath ".yuv.core$cpu") 
  var=$(GetDir $outpath)
  mkdir -p $var
  echo "outpath $outpath"
  echo "outpath dir $var"
  
  #set refpath
  refpath=$(ChangePrefix $inpath ./$REFDIR)
  refpath=$(ChangePostfix $refpath ".yuv")
  var=$(GetDir $refpath)
  mkdir -p $var
  echo "refpath $refpath"
  echo "refpath dir $var"
  
  
  #loop-step2: call ref decoder
  if [ "$mode" == "noRef" ]; then
  if [ "$version" == "WIN32" ]; then
  echo "loop-step2: call ref decoder"
  $TESTUTIL_DIR/$REF_DECODER -b $inpath -o $refpath
  fi
  fi
  
  #loop-step3: call test decoder
  if [ "$version" == "WIN32" ]; then
  echo "loop-step3: call test decoder"
  $TESTUTIL_DIR/$TEST_DECODER -i $inpath -o $outpath
  
  elif [ "$version" == "ANDROIDNDKV7" ]; then
    adb push $inpath $ANDROID_TEST_DIR/input.bin
    adb push h265dec_log.txt $ANDROID_TEST_DIR/  
    if [ "$mode" == "ref" ]; then
    adb shell $ANDROID_TEST_DIR/./voH265DecSample -i $ANDROID_TEST_DIR/input.bin -o $ANDROID_TEST_DIR/out.yuv -c $cpu
    adb pull $ANDROID_TEST_DIR/out.yuv $outpath
    else
    adb shell $ANDROID_TEST_DIR/./voH265DecSample -i $ANDROID_TEST_DIR/input.bin -c $cpu
    fi
    adb pull $ANDROID_TEST_DIR/h265dec_log.txt ./  
  elif [ "$version" == "IOSV7" ]; then 
  echo -e "Start create sdk for IOSV7! \n"
  #toDo
  
  fi
  
  #loop-step4: compare result
  file_diff_log="fileDiffLog"
  $TESTUTIL_DIR/$CHECKRESULT $outpath $refpath $LOG/$file_diff_log.log $cpu
  if [ ! -f "$LOG/$file_diff_log.log" ]; then
    echo "SUCCEED finish decoding and comparing for $inpath"
  else
    #set failpath
    failpath=$(ChangePrefix $inpath $FAIL_DIR)
    failpath=$(GetDir $failpath)
    mkdir -p $failpath
    cp $inpath $failpath
    rm $LOG/$file_diff_log.log
    echo "FAIL finish decoding and comparing for $inpath"
  fi
  
  echo ""
  echo ""  
  
  if [ "$mode" == "noRef" ]; then
  rm $refpath
  rm $outpath
  fi
  
  let cpu_i++ 
  done 
    
done

if [ "$mode" == "noRef" ]; then
rm -rf $REFDIR
rm -rf $OUTDIR
fi

rm -rf ./testResult/log

./tools/generateReport h265dec_log.txt h265dec_report.txt $len_cpu_numbers

echo ""
echo "finish decoding and comparing all. "
echo "Unsupported streams are listed in ./testResult/unsupport"


