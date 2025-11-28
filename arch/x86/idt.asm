extern interrupt_handler

global load_idt

; void load_idt(push idt_ptr);
load_idt:
  mov eax, [esp+4]
  lidt [eax]
  ret

; INFO: interrupt pushes eflags, cs, eip, and optionally error_code to the stack

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
  ; INFO: specially eax register must not be modified
  ; because by conventions, it contains return value 

  ; push registers state
	push eax
	push ebx
	push ecx
	push edx
	push ebp
	push esi
	push edi

  ; address to interrupt_cpu_state_t
  mov edi, esp

  ; value of interrupt code
  mov esi, [esp + 7 * 4]

  lea edx, [esp + 8 * 4]

  ; x86 uses stack for passing args from right to left 
  push edx
  push esi
  push edi

  ; interrupt_handler(edi, esi, edx)
	call interrupt_handler

  ; pop args
  add esp, 12

  ; pop registers state
	pop edi
	pop esi
	pop ebp
	pop edx
	pop ecx
	pop ebx
	pop eax

  ; pop interrupt_code and error_code
	add esp, 8

  ; INFO: we did not popped eflags, cs, eip cuz before calling iret
  ; they must be at the top of stack

	iret

%define INTERRUPT_TIMER 32
%define INTERRUPT_KEYBOARD 33
%define INTERRUPT_MOUSE 44
%define INTERRUPT_PAGE_FAULT 14

no_error_interrupt_handler INTERRUPT_TIMER
no_error_interrupt_handler INTERRUPT_KEYBOARD
no_error_interrupt_handler INTERRUPT_MOUSE
error_interrupt_handler INTERRUPT_PAGE_FAULT
