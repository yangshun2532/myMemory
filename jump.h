unsigned beq(unsigned pc)
{
    unsigned rs1=RS1;
    unsigned rs2=RS2;
    short tmp=lig;
    int a=tmp;
    unsigned b=a;
    b=b<<2;
    if(rs1==rs2)
    {
        return pc+4+b;
    }
    else{return pc+4;}
}

unsigned bne(unsigned pc)
{
    unsigned rs1=RS1;
    unsigned rs2=RS2;
    short tmp=lig;
    int a=tmp;
    unsigned b=a;
    b=b<<2;
    if(rs1!=rs2)
    {
        return pc+4+b;
    }
    else{return pc+4;}
}

unsigned blez(unsigned pc)
{
    int rs2=RS2;
    short tmp=lig;
    int a=tmp;
    unsigned b=a;
    b=b<<2;
    if(rs2<=0)
    {
        return pc+4+b;
    }
    else{return pc+4;}
}

unsigned bgtz(unsigned pc)
{
    int rs2=RS2;
    short tmp=lig;
    int a=tmp;
    unsigned b=a;
    b=b<<2;
    if(rs1>0)
    {
        return pc+4+b;
    }
    else{return pc+4;}
}

unsigned bltz(unsigned pc)
{
    int rs2=RS2;
    short tmp=lig;
    int a=tmp;
    unsigned b=a;
    b=b<<2;
    if(rs1<0)
    {
        return pc+4+b;
    }
    else{return pc+4;}
}

unsigned j(unsigned pc)
{
    unsigned tmp=target_addr;
    tmp=tmp<<6;
    tmp=tmp>>4;
    unsigned tmp_pc=pc;
    tmp_pc>>28;
    tmp_pc<<28;
    return tmp_pc+tmp;
}

unsigned jal(unsigned pc)
{
    myRegister[31]=pc+4;
     unsigned tmp=target_addr;
    tmp=tmp<<6;
    tmp=tmp>>4;
    unsigned tmp_pc=pc;
    tmp_pc>>28;
    tmp_pc<<28;
    return tmp_pc+tmp;
}

unsigned jr(unsigned pc)
{
  return RS2;
}

unsigned jalr(unsigned pc)
{
    *RD=pc+4;
    return *RS2;
}

unsigned nop(unsigned pc)
{
    return pc+4;
}

unsigned mfhi(unsigned pc)
{
    *RD=hi;
    return pc+4;
}

unsigned mflo(unsigned pc)
{
    *RD=lo;
    return pc+4;
}

unsigned mfepc(unsigned pc)
{
    *RS1=EPC;
    return pc+4;
}

unsigned mfco(unsigned pc)
{
    *RD=Cause_Reg;
    return pc+4;
}























