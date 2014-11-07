
unsigned slt(unsigned pc)
{
    int rs1=*RS1;
    int rs2=*RS2;
    if(rs2<rs1)
    {
        *RD=1;
    }
    else
    {
        *RD=0;
    }
    return pc+4;
}

unsigned sltu(unsigned pc)
{
    unsigned rs1=*RS1;
    unsigned rs2=*RS2;
    if(rs2<rs1)
    {
        *RD=1;
    }
    else
    {
        *RD=0;
    }
    return pc+4;
}

unsigned slti(unsigned pc)
{
   int rs2=*RS2;
   short tmp=lig;
   int a=tmp; //À©Õ¹·ûºÅÎ»
   if(rs2<a)
   {
       *RS1=1;
   }
   else
   {
       *RS1=0;
   }
   return pc+4;
}
unsigned sltiu(unsigned pc)
{
   unsigned rs2=*RS2;
   short tmp=lig;
   int a=tmp; //À©Õ¹·ûºÅÎ»
   unsigned b=a;
   if(rs2<b)
   {
       *RS1=1;
   }
   else
   {
       *RS1=0;
   }
   return pc+4;
}

unsigned myAnd(unsigned pc)
{
    unsigned rs1=*RS1;
    unsigned rs2=*RS2;
    *RD=rs1&rs2;
    return pc+4;
}


unsigned myOr(unsigned pc)
{
    unsigned rs1=*RS1;
    unsigned rs2=*RS2;
    *RD=rs1|rs2;
    return pc+4;
}

unsigned nor(unsigned pc)
{
    unsigned rs1=*RS1;
    unsigned rs2=*RS2;
    unsigned tmp=rs1|rs2;
    *RD=~tmp;
    return pc+4;
}

unsigned myXor(unsigned pc)
{
    unsigned rs1=*RS1;
    unsigned rs2=*RS2;
    *RD=rs1^rs2;
    return pc+4;
}
unsigned andi(unsigned pc)
{
    unsigned tmp=lig;
    unsigned rs2=*RS2;
    *RS1=lig&rs2;
    return pc+4;
}

unsigned ori(unsigned pc)
{
    unsigned tmp=lig;
    unsigned rs2=*RS2;
    *RS1=lig|rs2;
    return pc+4;
}

unsigned xori(unsigned pc)
{
    unsigned tmp=lig;
    unsigned rs2=*RS2;
    *RS1=lig^rs2;
    return pc+4;
}
