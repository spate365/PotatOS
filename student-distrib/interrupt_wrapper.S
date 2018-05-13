####################################
##		ASSEMBLY LINKAGE FILE	  ##
####################################

.globl handle_kb
.globl handle_rtc
.globl handle_pit

.globl call_system
.globl switch_to_user_mode
.globl halt_jump

jmp_table:
		.long halt, execute, read, write, open, close, getargs, vidmap#, set_handler, sigreturn

handle_kb:

	pushal
	cld
	
	call kb_handler
	
	popal

	iret
	
handle_rtc:

	pushal
	cld
	
	call rtc_handler
	
	popal
	
	iret

handle_pit:

	pushal
	cld
	
	call pit_handler
	
	popal
	
	iret

call_system:
	
	#Save Registers
	push	%ds
	push	%es
	push	%fs
	push	%gs

	pushl 	%edi
	pushl 	%esi

	pushl 	%edx
	pushl 	%ecx
	pushl 	%ebx
	pushfl

	# push args in correct order
	pushl 	%edx	
	pushl 	%ecx
	pushl 	%ebx

	# check for errors
	cmpl	$1, %eax	#minimum val for jump table
	jl 		ERROR

	cmpl	$8, %eax	#maximum val for jump table
	jg		ERROR

	decl 	%eax
	call 	*jmp_table( , %eax, 4) 
	jmp 	POP

ERROR:
	movl	$-1, %eax

POP:

	#pop args
	popl 	%ebx	
	popl 	%ecx
	popl 	%edx

	popfl
	
	#restore registers
	popl 	%ebx
	popl 	%ecx
	popl 	%edx

	popl 	%esi
	popl 	%edi

	pop		%gs
	pop		%fs
	pop		%es
	pop 	%ds

	iret

switch_to_user_mode:

	# C Calling Convention
	pushl 	%ebp
	movl 	%esp, %ebp

	# store this esp and ebp && restore these values in halt, right before the jump to return

	movl	12(%ebp), %ecx 		# get the arguement from correct location
	movl	%esp, (%ecx)
	movl	%ebp, 4(%ecx)		# place it in correct offset
	
	# Inspired by OSdev

	mov 	$USER_DS, %ax
	mov 	%ax, %ds
	mov 	%ax, %es
	mov 	%ax, %gs
	mov 	%ax, %fs

	movl 	%esp, %eax
	push	$USER_DS
	pushl	$ENTRY_POINT
	pushf
	push	$USER_CS

	movl 	8(%ebp), %ebx
	pushl   %ebx
	
	iret

.data
	ENTRY_POINT		= 		0x083FFFFC
	USER_CS     	= 		0x23
	USER_DS     	=		0x2B

halt_jump:

	leave	
	ret
