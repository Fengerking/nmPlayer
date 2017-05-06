cat fastquantizeb_neon.asm            | perl ../../../../build/make/ads2gas.pl >fastquantizeb_neon_gnu.S    
cat shortfdct_neon.asm                | perl ../../../../build/make/ads2gas.pl >shortfdct_neon_gnu.S        
cat subtract_neon.asm                 | perl ../../../../build/make/ads2gas.pl >subtract_neon_gnu.S         
cat vp8_memcpy_neon.asm               | perl ../../../../build/make/ads2gas.pl >vp8_memcpy_neon_gnu.S       
cat vp8_mse16x16_neon.asm             | perl ../../../../build/make/ads2gas.pl >vp8_mse16x16_neon_gnu.S     
cat vp8_shortwalsh4x4_neon.asm        | perl ../../../../build/make/ads2gas.pl >vp8_shortwalsh4x4_neon_gnu.S

