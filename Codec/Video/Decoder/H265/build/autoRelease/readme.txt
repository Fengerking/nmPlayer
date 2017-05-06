[auto release]
1. run createh265decsdk.sh to copy necessary files
2. run buildh265decsdk.sh to build relative lib
   Example:   ./buildh265decsdk.sh WIN32


[auto test]
1. cd ./autoRelease/autoTest 
2. copy candidata files into ./input
3. set reference data in ./ref (must set ./ref on Android; On PC, TAppDecoder.exe can generate it)
4. run test_vectors.sh
   Example:   ./test_vectors.sh noRef WIN32
   args1: has refence data or not, value maybe ref or noRef
   args2: platform, value maybe WIN32 or ANDROIDNDKV7
5. get test report
   ./tools/generateReport h265dec_log.txt report.txt
