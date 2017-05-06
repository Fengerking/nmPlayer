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

	END