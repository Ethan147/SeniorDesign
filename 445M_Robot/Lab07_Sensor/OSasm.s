	
	AREA    |.text|, CODE, READONLY
	
	IMPORT	RunPt
	IMPORT  NextPt
	EXPORT	StartOS 
	EXPORT  PendSV_Handler
	
StartOS	LDR		R0,	=RunPt	;page 179
		LDR		R2,	[R0]
		LDR		SP,	[R2]
		POP		{R4-R11}
		POP		{R0-R3}
		POP		{R12}
		POP		{LR}
		POP		{LR}
		POP		{R1}
		CPSIE	I
		BX		LR
						
PendSV_Handler				;1) saves R0-R3, R12, LR, PC, PSR
		CPSID	I			;2) prevent interrupt during switch
		PUSH	{R4-R11}	;3) save remaining regs R4-R11
		LDR		R0, =RunPt	;4) R0 = pointer to RunPt, old thread	
		LDR		R1, [R0]	;   R1 = RunPt
		STR		SP, [R1]	;5) Save SP into TCB
		LDR		R1, =NextPt ;6) R1 = pointer to NextPt
		LDR		R1, [R1]	;6) R1= NextPt
		STR		R1, [R0]	;   RunPt = R1
		LDR		SP, [R1]	;7) new thread SP; SP = RunPt-> SP
		POP		{R4-R11}	;8) restore regs R4-R11
		CPSIE	I			;9) tasks run with interrupts enabled
		BX		LR			;10) restore R0-R3, R12, LR, PC, PSR

;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Tell the assembler that we're done.
;
;******************************************************************************
        END
