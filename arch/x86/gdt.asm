global gdt_load_registers
global gdt_load

%define CODE_SEGMENT_OFFSET 0x08
%define DATA_SEGMENT_OFFSET 0x10

gdt_load:
  lgdt [esp + 4]
  ret

gdt_load_registers:
  mov ax, DATA_SEGMENT_OFFSET
  mov ds, ax
  mov ss, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  jmp CODE_SEGMENT_OFFSET: flush_cs

flush_cs:
  ret
