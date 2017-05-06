
.set PLD_ENABLE          ,1

.set PRO_VER              , 1	@ 0:simple profile, 1:main profile
.set DECODEREPEATPAD_PROFILE         ,40      @ match autoprofile.h
.set DECINVIBQUANTESCCODE_PROFILE    ,15      @ match autoprofile.h
.macro WMV_LEAF_ENTRY 				          
_$0:
.endmacro
.macro WMV_ENTRY_END	
.endmacro
.macro  FUNCTION_PROFILE_START
.endmacro
.macro  FUNCTION_PROFILE_STOP
.endmacro
.macro  FRAME_PROFILE_COUNT
.endmacro
.macro  FRAME_PROFILE_COUNT_SAVE_LR
.endmacro
