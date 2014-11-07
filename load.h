//date:2014/10/23
//���ļ�Ϊ���뺯��������ͨ�üĴ�����64λ�������Ժ��ĳ�32λ����lig��unsigned short�ͣ��������к����е�ligӦ���Ƚ��з�����չ��
//date:2014/10/25
//���Ĵ�����Ϊ32λ
unsigned short addrToMyAddr(unsigned tmp)
 {
     unsigned short addr=tmp%65536;
     return addr;
 }

unsigned lw(unsigned pc)//������
{
    if(lig<32768)
    {
        unsigned  tmp=lig+*RS2;
    }
    else
    {
        unsigned tmp=lig+*RS2+0xffff0000;
    }
    unsigned char* tmp_R=RS1;
    unsigned short addr=addrToMyAddr(tmp);
    for(int i=0;i<4;i++)
    {
        readMymemory(addr+i,tmp_R+i);//�ڴ���ô�˴洢
    }
    return pc+4;
}


unsigned lb(unsigned pc)//�����ֽ�
{
    if(lig<32768)
    {
        unsigned  tmp=lig+*RS2;
    }
    else
    {
        unsigned tmp=lig+*RS2+0xffff0000;
    }
    char* tmp_R=RS1;
    unsigned short addr=addrToMyAddr(tmp);
    readMymemory(addr,tmp_R+3);
    if(tmp[3]>127)
    {
        for(int i=0;i<3;i++)
        {
            tmp[i]=255;
        }
    }
    else
    {
        for(int i=0;i<3;i++)
        {
            tmp[i]=0;
        }
    }
    return pc+4;
}

unsigned lbu(unsigned pc)//�����ֽ�
{
    if(lig<32768)
    {
        unsigned  tmp=lig+*RS2;
    }
    else
    {
        unsigned tmp=lig+*RS2+0xffff0000;
    }
    char* tmp_R=RS1;
    unsigned short addr=addrToMyAddr(tmp);
    readMymemory(addr,tmp_R+3);
    for(int i=0;i<3;i++)
    {
       tmp[i]=0;
    }

    return pc+4;
}

unsigned sw(unsigned pc)
{
       if(lig<32768)
    {
        unsigned  tmp=lig+*RS2;
    }
    else
    {
        unsigned tmp=lig+*RS2+0xffff0000;
    }
    char* tmp_R=RS1;
    unsigned short addr=addrToMyAddr(tmp);
    char* tmp_R=RS1;
     for(int i=0;i<4;i++)
    {
        writeMymemory(addr+i,tmp_R+i);//�ڴ���ô�˴洢
    }
    return pc+4;
}

unsigned sb(unsigned pc)
{
    if(lig<32768)
    {
        unsigned  tmp=lig+*RS2;
    }
    else
    {
        unsigned tmp=lig+*RS2+0xffff0000;
    }
    char* tmp_R=RS1;
    unsigned short addr=addrToMyAddr(tmp);
    writeMymemory(addr,tmp_R+3);
    return pc+4;

}
unsigned lui(unsigned pc)
{
    unsigned tmp=lig;
    tmp=tmp<<16;
    *RS1=tmp;
    return pc+4;
}





