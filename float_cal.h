unsigned lwc1(unsigned pc)
{
    char* R=FS1;
    short tmp=lig;
    int a=tmp;
    unsigned short addr=addrToMyAddr(*RS2+(unsigned)a);
    for(int i=0;i<4;i++)
    {
        readMymemory(addr+i,R+i);
    }
    return pc+4;
}

unsigned swc1(unsigned pc)
{
    char* R=FS1;
    short tmp=lig;
    int a=tmp;
    unsigned short addr=addrToMyAddr(*RS2+(unsigned)a);
    for(int i=0;i<4;i++)
    {
        writeMymemory(addr+i,R+i);
    }
    return pc+4;
}

unsigned add.s(unsigned pc)
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

unsigned add.d(unsigned pc) //双精度浮点数加法
{
    double* fs1=FS1;
    double* fs2=FS2;
    double* fd=FD;
    *fd=*fs1+*fs2;
    if(overflow)
    {
        exit(EXIT_FAILURE);
    }
    return pc+4;
}

unsigned sub.s(unsigned pc)
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

unsigned sub.d(unsigned pc)
{
    double* fs1=FS1;
    double* fs2=FS2;
    double* fd=FD;
    *fd=*fs2-*fs1;
    if(overflow)
    {
        exit(EXIT_FAILURE);
    }
    return pc+4;
}

unsigned mul.s(unsigned pc)
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

unsigned mul.d(unsigned pc)
{
    double* fs1=FS1;
    double* fs2=FS2;
    double* fd=FD;
    *fd=*fs2*(*fs1);
    if(overflow)
    {
        exit(EXIT_FAILURE);
    }
    return pc+4;
}

unsigned div.d(unsigned pc)
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

unsigned div.d(unsigned pc)
{
    double* fs1=FS1;
    double* fs2=FS2;
    double* fd=FD;
    *fd=*fs2/(*fs1);
    if(overflow)
    {
        exit(EXIT_FAILURE);
    }
    return pc+4;
}





