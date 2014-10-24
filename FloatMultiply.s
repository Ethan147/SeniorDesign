;Version 3 assembly floating point 
; run with floating-point hardware active         
		AREA    DATA, ALIGN=2 
T       SPACE   4 
N       SPACE   4         
		AREA    |.text|, CODE, READONLY, ALIGN=2         
		THUMB
		EXPORT FloatMultiply

FloatMultiply	
	LDR R1,=N    ;pointer to N       
	LDR R2,=T    ;pointer to T
	           
	VMUL.F32 S0,S0,S1    ; X*Y
	VSTR.F32 S0,[R2]     ; T=X*Y          
	BX  LR
	ALIGN
	END