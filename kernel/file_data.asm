%macro define_file 2
  global %1_data
  global %1_end
  %1_data: 
  incbin %2
  %1_end: db 0
%endmacro

section .data
define_file message_file, "../disk_files/message.txt"
