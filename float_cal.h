unsigned lwc1(unsigned pc)
{
    unsigned char* R=FS1;
    short tmp=lig;
    int a=tmp;
    unsigned short addr=addrToMyAddr(*RS2+(unsigned)a);
    int i;
    for(i=0;i<4;i++)
    {
        readMymemory(addr+i,R+i);
    }
    return pc+4;
}
unsigned L_D(unsigned pc)  //载入双字
{
    unsigned char* R=FS1;
    short tmp=lig;
    int a=tmp;
    unsigned short addr=addrToMyAddr(*RS2+(unsigned)a);
    int i;
    for(i=0;i<8;i++)
    {
        readMymemory(addr+i,R+i);
    }
    return pc+4;
}

unsigned S_D(unsigned pc)  //存入双字
{
    unsigned char* R=FS1;
    short tmp=lig;
    int a=tmp;
    //printf("%u %d\n",*RS2,a);
    unsigned short addr=addrToMyAddr(*RS2+(unsigned)a);
    //printf("%u\n",addr);
    int i;
    for(i=0;i<8;i++)
    {
        writeMymemory(addr+i,R+i);
    }
    return pc+4;
}
unsigned swc1(unsigned pc)
{
    unsigned char* R=FS1;
    short tmp=lig;
    int a=tmp;
    unsigned short addr=addrToMyAddr(*RS2+(unsigned)a);
    int i;
    for(i=0;i<4;i++)
    {
        writeMymemory(addr+i,R+i);
    }
    return pc+4;
}

unsigned add_s(unsigned pc)
{
    float *fs1=FS1;
    float *fs2=FS2;
    float *fd=FD;
    *fd=*fs1+*fs2;
    if("overflow")
    {
        exit(EXIT_FAILURE);
    }
    return pc+4;
}

unsigned add_d(unsigned pc) //双精度浮点数加法
{
    double* fs1=FS1;
    double* fs2=FS2;
    double* fd=FD;
    *fd=*fs1+*fs2;
   // if(overflow)
    //{
    //    exit(EXIT_FAILURE);
   // }
    return pc+4;
}

unsigned sub_s(unsigned pc)
{
    float *fs1=FS1;
    float *fs2=FS2;
    float *fd=FD;
    *fd=*fs2-*fs1;
    if("overflow")
    {
        exit(EXIT_FAILURE);
    }
    return pc+4;
}

unsigned sub_d(unsigned pc)
{
    double* fs1=FS1;
    double* fs2=FS2;
    double* fd=FD;
    *fd=*fs2-*fs1;
   // if(overflow)
    //{
   //     exit(EXIT_FAILURE);
   // }
    return pc+4;
}

unsigned mul_s(unsigned pc)
{
    float *fs1=FS1;
    float *fs2=FS2;
    float *fd=FD;
    *fd=*fs2*(*fs1);
    if("overflow")
    {
        exit(EXIT_FAILURE);
    }
    return pc+4;
}

unsigned mul_d(unsigned pc)
{
    double* fs1=FS1;
    double* fs2=FS2;
    double* fd=FD;
    *fd=*fs2*(*fs1);
    //if(overflow)
    //{
   //     exit(EXIT_FAILURE);
   // }
    return pc+4;
}

unsigned div_s(unsigned pc)
{
    float *fs1=FS1;
    float *fs2=FS2;
    float *fd=FD;
    *fd=*fs2/(*fs1);
    if("overflow")
    {
        exit(EXIT_FAILURE);
    }
    return pc+4;
}

unsigned div_d(unsigned pc)
{
    double* fs1=FS1;
    double* fs2=FS2;
    double* fd=FD;
    *fd=*fs2/(*fs1);
   // if(overflow)
   // {
   //     exit(EXIT_FAILURE);
    //}
    return pc+4;
}





