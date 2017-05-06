cat vp8_fast_quantize_b_armv6.asm  | perl ../../../../build/make/ads2gas.pl >vp8_fast_quantize_b_armv6_gnu.S 
cat vp8_mse16x16_armv6.asm         | perl ../../../../build/make/ads2gas.pl >vp8_mse16x16_armv6_gnu.S        
cat vp8_short_fdct4x4_armv6.asm    | perl ../../../../build/make/ads2gas.pl >vp8_short_fdct4x4_armv6_gnu.S  
cat vp8_subtract_armv6.asm         | perl ../../../../build/make/ads2gas.pl >vp8_subtract_armv6_gnu.S       
cat walsh_v6.asm                   | perl ../../../../build/make/ads2gas.pl >walsh_v6_gnu.S                
