extern interrupt_handler

global load_idt

; void load_idt(unsigned int idt_ptr);
load_idt:
  mov eax, [esp+4]
  lidt [eax]
  ret

; before calling C interrupt_handler stack state should be
;   eflags, cs, eip, error_code 
;   interrupt_code
;   eax, ebx, ecx, edx, ebp, esi, edi

; pushes error_code, and interrupt_code
%macro no_error_interrupt_handler 1
  global interrupt_handler_%1
  interrupt_handler_%1:
    push dword 0
    push dword %1
    jmp common_interrupt_handler
%endmacro

; pushes interrupt_code only as error_code is pushed by interrupt
%macro error_interrupt_handler 1
  global interrupt_handler_%1
  interrupt_handler_%1:
    push dword %1
    jmp common_interrupt_handler
%endmacro

common_interrupt_handler:
  ; save registers state
	push eax
	push ebx
	push ecx
	push edx
	push ebp
	push esi
	push edi

	call interrupt_handler

  ; restore registers state
	pop edi
	pop esi
	pop ebp
	pop edx
	pop ecx
	pop ebx
	pop eax

  ; to pop error_code:4byte and interrupt_code:4byte we pushed before 
	add esp, 8

	iret

%define INTERRUPT_KEYBOARD 33

no_error_interrupt_handler INTERRUPT_KEYBOARD
