

spi flash ��һ��ʹ����Ҫ��ʽ��
           The first use must be formatted
{
    BYTE *work;
    char logic_nbr[2] = {'0',':'};
    work = rt_malloc(_MAX_SS);
    f_mkfs(logic_nbr, FM_FAT, 0, work, _MAX_SS);
}

 register������ؼ�����������������ܵĽ���������CPU�ڲ��Ĵ����У�������ͨ���ڴ�Ѱַ���ʣ������Ч�ʡ�ע���Ǿ����ܣ����Ǿ���
 
 $PROJ_DIR$\..\protocolstack\uIP-1.0\uip
 
 
 
 