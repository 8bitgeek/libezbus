define hook-next
  refresh
end
define hook-stop
  refresh
end
layout split
winheight src 24
target remote localhost:3333
load ezapp-stm32f091.elf
file ezapp-stm32f091.elf
break main
continue
