cat vp8_vpxyv12_copyframe_func_neon.asm       | perl ../../../build/make/ads2gas.pl > vp8_vpxyv12_copyframe_func_neon_gnu.S    
cat vp8_vpxyv12_copysrcframe_func_neon.asm    | perl ../../../build/make/ads2gas.pl > vp8_vpxyv12_copysrcframe_func_neon_gnu.S 
cat vp8_vpxyv12_copy_y_neon.asm               | perl ../../../build/make/ads2gas.pl > vp8_vpxyv12_copy_y_neon_gnu.S            
cat vp8_vpxyv12_extendframeborders_neon.asm   | perl ../../../build/make/ads2gas.pl > vp8_vpxyv12_extendframeborders_neon_gnu.S