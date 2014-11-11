//date:2014/10/23
//本文件为载入函数，现在通用寄存器是64位（可能以后会改成32位），lig是unsigned short型，下面所有函数中的lig应该先进行符号扩展。
//date:2014/10/25
//将寄存器改为32位
int readMymemory(unsigned short addr,unsigned char* Register);
int writeMymemory(unsigned short addr,unsigned char* Register);
unsigned short addrToMyAddr(unsigned tmp)
 {
     unsigned short addr=tmp%65536;
     return addr;
 }

unsigned lw(unsigned pc)//载入字
{
    unsigned tmp;
    if(lig<32768)
    {
        tmp=lig+*RS2;
    }
    else
    {
        tmp=lig+*RS2+0xffff0000;
    }
    unsigned char* tmp_R=RS1;
    unsigned short addr=addrToMyAddr(tmp);
    unsigned short i;
    for(i=0;i<4;i++)
    {
        readMymemory(addr++,tmp_R++);//内存采用大端存储
    }
    return pc+4;
}


unsigned lb(unsigned pc)//载入字节
{
    unsigned tmp;
    if(lig<32768)
    {
          tmp=lig+*RS2;
    }
    else
    {
        tmp=lig+*RS2+0xffff0000;
    }
    unsigned char* tmp_R=RS1;
    unsigned short addr=addrToMyAddr(tmp);
    readMymemory(addr,tmp_R+3);
    int i;
    if(tmp_R[3]>127)
    {
        for(i=0;i<3;i++)
        {
            tmp_R[i]=255;
        }
    }
    else
    {
        for(i=0;i<3;i++)
        {
            tmp_R[i]=0;
        }
    }
    return pc+4;
}

unsigned lbu(unsigned pc)//载入字节
{
    unsigned tmp;
    if(lig<32768)
    {
        tmp=lig+*RS2;
    }
    else
    {
        tmp=lig+*RS2+0xffff0000;
    }
    unsigned char* tmp_R=RS1;
    unsigned short addr=addrToMyAddr(tmp);
    readMymemory(addr,tmp_R+3);
    int i;
    for(i=0;i<3;i++)
    {
       tmp_R[i]=0;
    }

    return pc+4;
}

unsigned sw(unsigned pc)
{
       unsigned tmp;
       if(lig<32768)
    {
        tmp=lig+*RS2;
    }
    else
    {
        tmp=lig+*RS2+0xffff0000;
    }
    unsigned char* tmp_R=RS1;
    unsigned short addr=addrToMyAddr(tmp);
    int i;
     for(i=0;i<4;i++)
    {
        writeMymemory(addr+i,tmp_R+i);//内存采用大端存储
    }
    return pc+4;
}

unsigned sb(unsigned pc)
{
    unsigned tmp;
    if(lig<32768)
    {
        tmp=lig+*RS2;
    }
    else
    {
        tmp=lig+*RS2+0xffff0000;
    }
    unsigned char* tmp_R=RS1;
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





