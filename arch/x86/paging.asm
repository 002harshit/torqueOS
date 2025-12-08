global load_page_dir
global invlpg_flush_page

load_page_dir:
  ; INFO: cr3 should be set to physical address that points to a valid page directory 
  ; and structure of page directory in x86 is
  ; -- uint32 page_dir[1024];
  ; -- uint32 page_table[1024][1024];
  ; 
  ; higher 20bits of page_dir[i] must contain addr to a page_table[i]
  ; -- page_dir[i] >> 12 == &page_table[i] >> 12
  ; 
  ; and higher 20bits of page_table[i][j] must contain addr to a corrosponding physical address
  ; -- page_table[i][j] >> 12 == (physical_address) >> 12

  mov eax, [esp+4]
  mov cr3, eax

  ; INFO: set PG bit (ie bit 32) of cr0 control register
  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax

  ret

invlpg_flush_page:
  ; INFO: invlpg takes virtual address as arguement, and reload its page cache
  mov eax, [esp+4]
  invlpg [eax]
  ret
