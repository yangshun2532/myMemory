
unsigned myRegister[32]={0};//申请一个数组来模拟寄存器
unsigned long myFloatReg[32]={0};
unsigned lo=0;
unsigned hi=0;
unsigned bad=0;
unsigned sr=0;
unsigned EPC=0;
unsigned Cause_Reg=0;
unsigned pc=0;
unsigned* RD=NULL; //Rd
unsigned* RS1=NULL;//Rt
unsigned* RS2=NULL;//Rs
unsigned long* FD=NULL;
unsigned long* FS1=NULL;//Rt
unsigned long* FS2=NULL;//Rs
unsigned short lig=0;   //I型指令的立即数
unsigned target_addr;

/*DATE:2014/10/22
功能：从通用寄存器取无符号数
*/
unsigned  getU(unsigned * RS)
{
    return *RS;
}

/*无符号加
DATE:2014/10/22
RD=RS1+RS2
*/
unsigned add(unsigned pc)
{
    int rs1=*RS1;
    int rs2=*RS2;
    int tmp=rs1+rs2;
    *RD=tmp;
    if(rs1>0&&rs2>0&&tmp<0)  //溢出判断
        exit(EXIT_FAILURE);
    if(rs1<0&&rs2<0&&tmp>0)
        exit(EXIT_FAILURE);
    return pc+4;
}
/*有符号加
RD=RS1+RS2
*/
unsigned addu(unsigned pc)
{
   unsigned rs1=*RS1;
   unsigned rs2=*RS2;
   *RD=rs1+rs2;
   return pc+4;
}

unsigned sub(unsigned pc)
{
    int rs1=*RS1;
    int rs2=*RS2;
    tmp=rs1-rs2;
    *RD=rs2-rs1;
   if(rs1>0&&rs2<0&&tmp<0)
        exit(EXIT_FAILURE);
   if(rs1<0&&rs2>0&&tmp>0)
        exit(EXIT_FAILURE);
    return pc+4;
}


unsigned subu(unsigned pc)
{
    unsigned rs1=*RS1;
    unsigned rs2=*RS2;
    *RD=rs2-rs1;
    return pc+4;
}

unsigned addi(unsigned pc)
{
    int rs2=*RS2;
    short tmp=lig;
    int tmp_int=(int)tmp;
    *RS1=rs2+tmp_int;
    int a=*RS1;
  if(tmp_int0&&rs2>0&&a<0)
        exit(EXIT_FAILURE);
  if(tmp_int<0&&rs2<0&&tmp>0)
        exit(EXIT_FAILURE);
    return pc+4;

}

 unsigned addiu(unsigned pc)
 {
    int rs2=*RS2;
    short tmp=lig;
    *RS1=rs2+(int)tmp;
    return pc+4;
 }

unsigned mul(unsigned pc)
{
    int rs1=*RS1;
    int rs2=*RS2;
    long result=(long)rs1*(long)rs2;
    unsigned* tmp=&result;
    hi=*tmp;
    lo=*(tmp+1);
    return pc+4;
}

unsigned mulu(unsigned pc)
{
    unsigned rs1=*RS1;
    unsigned rs2=*RS2;
    long result=(unsigned long)rs1*(unsigned long)rs2;
    unsigned* tmp=&result;
    hi=*tmp;
    lo=*(tmp+1);
    return pc+4;
}

unsigned myDiv(unsigned pc)
{
    int rs1=*RS1;
    int rs2=*RS2;
    hi=rs2%rs1;
    lo=rs2/rs1;
    return pc+4;
}

unsigned divu(unsigned pc)
{
    unsigned rs1=*RS1;
    unsigned rs2=*RS2;
    hi=rs2%rs1;
    lo=rs2/rs1;
    return pc+4;
}





