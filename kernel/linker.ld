ENTRY(kernel_loader)
SECTIONS {
  . = 0x00100000;

  _kernel_start = .;

  .multiboot ALIGN(8):
  {
    KEEP(*(.multiboot_header))
  }

  .text ALIGN(0x1000):
  {
    *(.text)
  }

  .rodata ALIGN(0x1000):
  {
    *(.rodata)  
  }

  .data ALIGN(0x1000):
  {
    *(.data)
  }

  .bss ALIGN(0x1000):
  {
    *(COMMON)
    *(.bss)
  }

  _kernel_end = .;

}
