; void load_page_directory(unsigned int* page_dir_ptr)
global load_page_directory

load_page_directory:
  ; load page directory ptr into cr0
  mov eax, [esp+4]
  mov cr3, eax

  ; enable paging
  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax

  ret
