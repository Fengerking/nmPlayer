CARMV4			EQU 0
CARMV5E			EQU 1
CARMV6			EQU 2

CPU_TYPE		EQU CARMV6


AGINED      	EQU	0

	IF  AGINED = 1

		MACRO	
		LOAD_DATA	$Ra, $Rb, $Rc, $off
		ldrd	$Ra, [$Rc, $off]
		MEND  	

	ELSE
		MACRO	
		LOAD_DATA	$Ra, $Rb, $Rc, $off
		ldr		$Ra, [$Rc, $off]
		ldr		$Rb, [$Rc, $off + 4]
		MEND
	ENDIF

	IF  AGINED = 1

		MACRO	
		STR_DATA	$Ra, $Rb, $Rc, $off
		strd	$Ra, [$Rc, $off]
		MEND  	

	ELSE
		MACRO	
		STR_DATA	$Ra, $Rb, $Rc, $off
		str		$Ra, [$Rc, $off]
		str		$Rb, [$Rc, $off + 4]
		MEND
	ENDIF

	END