

spi flash 第一次使用需要格式化
           The first use must be formatted
{
    BYTE *work;
    char logic_nbr[2] = {'0',':'};
    work = rt_malloc(_MAX_SS);
    f_mkfs(logic_nbr, FM_FAT, 0, work, _MAX_SS);
}

 register：这个关键字请求编译器尽可能的将变量存在CPU内部寄存器中，而不是通过内存寻址访问，以提高效率。注意是尽可能，不是绝对
 
 $PROJ_DIR$\..\protocolstack\uIP-1.0\uip
 
 
 
 