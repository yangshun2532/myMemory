/*************************************************
实现了页式虚拟存储、内存的分配和释放，显示内存剩余容量，耗损均衡等功能。
MyMemory以512B为一页，共128页，第一页用于存放页表和空闲块管理队列。
页表共有128个item，每个item为8个bit，是一个静态数组。第一个bit为有效位，
后七位对应该页的物理地址。
空闲页管理队列共有128个item，每个item也是8个bit，是一个静态链表。第一个bit
表示该物理页是否空闲，后七位是指向下一个空闲页的指针。分配页时，从队首取，
释放内存页时，挂在队尾。
**************************************************/
#include <stdio.h>
#include <stdlib.h>
#include"INSN_ADD.h"
#include "load.h"
#include "logic.h"
#include "jump.h"
#include "float_cal.h"

/*静态队列freePage用来管理空闲物理页，有损耗均衡的功能*/
struct freePage
{
  unsigned char* firstFree;
  unsigned short head;
  unsigned short end;
  unsigned short count;
  int freeCount;
};
/*内存*/
struct myMemory
{
  unsigned char* myBuffer;//内存首地址
  unsigned  vm_size; //内存大小
  unsigned char* vm_start;
  unsigned char* vm_end;
  unsigned char* pageHead;//地址映射表
  struct freePage* FreePage;//空闲页管理队列
};

struct myMemory* MyMemory;

int init()
{
  MyMemory=(struct myMemory*)malloc(sizeof(struct myMemory));
  MyMemory->vm_size=65536;//64KB
  printf("%u",MyMemory->vm_size);
  MyMemory->myBuffer=(unsigned char*)malloc(MyMemory->vm_size);
  printf("the addr of buffer is : %p\n",MyMemory->myBuffer);
  MyMemory->vm_start=MyMemory->myBuffer;
  MyMemory->vm_end=MyMemory->vm_start+MyMemory->vm_size;
  /*将页表和空闲页管理队列都放在myMemory的第一页，页表的起始地址就是myMemory的起
  始地址，空闲页管理模块的起始地址为256B，初始化页表和空闲页管理队列*/
  MyMemory->pageHead=MyMemory->vm_start;
  printf("the addr of pageHead is : %p\n",MyMemory->pageHead);
  MyMemory->FreePage=(struct freePage*)malloc(sizeof(struct freePage));
  MyMemory->FreePage->firstFree=MyMemory->vm_start+256;
  printf("the addr of firstFree is : %p\n",MyMemory->FreePage->firstFree);
  MyMemory->FreePage->count=128;
  MyMemory->FreePage->freeCount=127;
  MyMemory->FreePage->head=1;
  MyMemory->FreePage->end=127;
  unsigned char i;
  for(i=1;i<127;i++)
  {
      MyMemory->FreePage->firstFree[i]=i+1;
  }
  MyMemory->FreePage->firstFree[0]=128;
  MyMemory->pageHead[0]=0;
  printf("finishing initing FreePage\n");
  for(i=1;i<=127;i++)
  {
    MyMemory->pageHead[i]=128;
  }
  printf("finishing initing\n");
  //printf("%d",sizeof(unsigned int));
  //printf("%p\n%p\n",MyMemory->vm_start,MyMemory->vm_end);
  //printf("%u",myRegister[10]);
  return 0;
}
/**************************************************************
函数名：myMalloc
功能：分配空闲页
**************************************************************/
unsigned short myMalloc(unsigned short offest)
{
  //printf("begin malloc\n");
  unsigned short p_addr=((MyMemory->FreePage->head)<<9)+offest;
  //printf("offset is %u\n",p_addr);
  unsigned short a=MyMemory->FreePage->head;
  if(MyMemory->FreePage->firstFree[a]<128)
    MyMemory->FreePage->firstFree[a]+=128; //最高位置1，表示非空闲
  //printf("%d",MyMemory->FreePage->firstFree[a]);
  MyMemory->FreePage->head=(unsigned short)(MyMemory->FreePage->firstFree[a]%128);//修改头指针
  MyMemory->FreePage->freeCount--;
  //printf("finish malloc\n");
  //printf("the malloc p_addr is : %u\n",p_addr);
  return p_addr;
}
/**************************************************************
函数名：myFtl
功能：将逻辑地址转换成物理地址
**************************************************************/
unsigned short myFtl(unsigned short v_addr)
{
  //printf("begin ftl\n");
  unsigned short v_page=v_addr>>9;
  unsigned short p_addr=0;
  if(MyMemory->pageHead[v_page]/128==1)//最高位为1，无效，分配新页
  {
    p_addr=myMalloc(v_addr%512);
    MyMemory->pageHead[v_page]=(unsigned char)(p_addr/512);
  }
  else//有效，直接查表转换
  {
    p_addr=((unsigned short)MyMemory->pageHead[v_page]<<9)+v_addr%512;
  }
  //printf("finish ftl\n");
  //printf("the p_addr is : %u\n",p_addr);
  return p_addr;
}
/**************************************************************
函数名：myFree
功能：释放内存空间
**************************************************************/
int myFree(unsigned short v_addr)
{
  //printf("begin freeing\n");
  if(MyMemory->pageHead[v_addr/512]/128==1)
  {
      return 0;//若该逻辑地址本来就是空闲，返回
  }
  unsigned short p_addr=myFtl(v_addr);
  MyMemory->FreePage->firstFree[p_addr/512]-=128;//修改有效位
  unsigned short a=MyMemory->FreePage->end;
  MyMemory->FreePage->firstFree[a]=(unsigned char)(p_addr/512);
  if(MyMemory->FreePage->firstFree[a]>=128)
    MyMemory->FreePage->firstFree[a]-=128;
  MyMemory->FreePage->freeCount++;
  MyMemory->FreePage->end=p_addr/512; //修改尾指针
  //printf("finish freeing\n");
  return 0;
}
/**************************************************************
函数名：readMymemory
功能：将数据从MyMemory读入到我的寄存器
**************************************************************/

int readMymemory(unsigned short addr,unsigned char* Register)
{
  //printf("begin read\n");
  //if((addr%4!=0)||(addr<512))
  //return -1;
  unsigned char* tmp=MyMemory->myBuffer+myFtl(addr);//读一个字节
  *Register=*tmp;
  //printf("finish read\n");
  return 0;
}
/**************************************************************
函数名：writeMymemory
功能：将寄存器数据写到MyMemory
**************************************************************/
int writeMymemory(unsigned short addr,unsigned char* Register)
{
    //printf("begin write\n");
    //if(addr%4!=0||addr<512)
       // return -1;
       unsigned char* tmp=MyMemory->myBuffer+myFtl(addr);
    *tmp=*Register;
    //printf("finish write\n");
    return 0;
}

void printPageHead()
{
  printf(">>>>>>>>>>>>>>PageHead>>>>>>>>>>>>>>>\n");
  int i,j;
  //将页表以二进制形式输出
  for(i=0;i<128;i++)
  {
    unsigned char m=MyMemory->pageHead[i];
    int n[8];
    for(j=7;j>=0;j--)
    {
      n[j]=m%2;
      m=m/2;
    }
    printf("%d\t",n[0]);
    for(j=1;j<7;j++)
    {
        printf("%d",n[j]);
    }
    printf("%d\n",n[7]);
  }
}
void printFreePage()
{
    //将空闲页管理队列以二进制形式输出
    printf(">>>>>>>>>>>>>>FreePage>>>>>>>>>>>>>>>\n");
    int i,j;
  for(i=0;i<128;i++)
  {
    unsigned char m=MyMemory->FreePage->firstFree[i];
    int n[8];
    for(j=7;j>=0;j--)
    {
      n[j]=m%2;
      m=m/2;
    }
    printf("%d\t",n[0]);
    for(j=1;j<7;j++)
    {
        printf("%d",n[j]);
    }
    printf("%d\n",n[7]);
  }
}


void printMygister()
{
    int i,j=0;
   for(i=0;i<=31;i++)
   {
    printf("R%d:  ",i);
    unsigned m=myRegister[i];
    int n[32];
    for(j=31;j>=0;j--)
    {
      n[j]=m%2;
      m=m/2;
    }
    for(j=0;j<32;j++)
    {
        printf("%d",n[j]);
    }
    printf("%d\n",n[7]);
   }
   return;
}
void printFloatReg()
{
    int i=0;
    float* p_s=NULL;
    double* p_d=NULL;
    for(i=0;i<=31;i++)
    {
        printf("F%d(单精度):  ",i);
        p_s=myFloatReg+i;
        p_d=myFloatReg+i;
        printf("F%d:  ",i);
        printf("%f\tF%d（双精度）: %lf\n",*p_s,i,*p_d);
    }
    return;
}


int exe(FILE* program)  //program指向存有待执行程序机器码的文件
{

     char* tmp_instru=(char*)malloc(33*sizeof(char)); //读机器码
     programTail=programHead;
     while(fscanf(program,"%s",tmp_instru)!=EOF)
     {
         instru=0;
         int i=0;
         unsigned j=1;
         for(i=31;i>=0;i--)
         {
            if(tmp_instru[i]=='1')
            {
                instru+=j;
                j*=2;
            }
            else
            {
                j*=2;
            }
         }//将机器码转为unsi
         unsigned char* tmp_R=&instru;
         for(i=0;i<4;i++)
         {
             writeMymemory(programTail+i,tmp_R+i);//装载指令
         }
         programTail+=4;//最后一条指令的下一条指令的地址，用来判断程序是否执行完
     }
     pcShort=programHead;
     pc=pcShort;
     while(pcShort!=programTail)
    {
        instru=0;   //指令寄存器清零
    unsigned char* tmp_R=&instru;
    unsigned short addr=addrToMyAddr(pc);
    int i;
    for(i=0;i<4;i++)
    {
        readMymemory(addr+i,tmp_R+i);//取指令
    }
    unsigned tmp=instru>>26;//得到指令op

    //printf("the op is :  %u\n",tmp);

    unsigned numRs=0,numRt=0,numRd=0,numFs=0,numFt=0,numFd=0,tmp_fuc=0;
    switch(tmp)
    {
    case 0x00000023:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=lw(pc);
        break;
    case 0x0000002B:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=sw(pc);
        break;
    case 0x00000008:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=addi(pc);
        break;
    case 0x00000009:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=addiu(pc);
        break;
    case 0x0000000A:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=slti(pc);
        break;
    case 0x0000000B:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=sltiu(pc);
        break;
    case 0x0000000C:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=andi(pc);
        break;
    case 0x0000000D:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=ori(pc);
        break;
    case 0x0000000E:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=xori(pc);
        break;
    case 0x00000024:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=lbu(pc);
        break;
    case 0x00000020:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=lb(pc);
        break;
    case 0x00000028:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=sb(pc);
        break;
    case 0x0000000F:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=lui(pc);
        break;
    case 0x00000004:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=beq(pc);
        break;
    case 0x00000005:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        //printf("%u,%u,%u,%u\n",numRt,numRs,*RS1,*RS2);
        lig=instru<<16>>16;
       // printf("%u\n",lig);
        pc=bne(pc);
        break;
    case 0x00000006:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=blez(pc);
        break;
    case 0x00000007:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=bgtz(pc);
        break;
    case 0x00000001:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        lig=instru<<16>>16;
        pc=bltz(pc);
        break;
    case 0x00000002:
        pc=j(pc);
        break;
    case 0x00000003:
        pc=jal(pc);
        break;
    case 0x00000000:
        numRs=instru<<6>>27;
        numRt=instru<<11>>27;
        numRd=instru<<16>>27;
        RS1=myRegister+numRt;
        RS2=myRegister+numRs;
        RD=myRegister+numRd;
        tmp_fuc=instru%64;
        switch(tmp_fuc)
        {
        case 32:
            pc=add(pc);
            break;
        case 33:
            pc=addu(pc);
            break;
        case 34:
            pc=sub(pc);
            break;
        case 35:
            pc=subu(pc);
            break;
        case 24:
            pc=mul(pc);
            break;
        case 25:
            pc=mulu(pc);
            break;
        case 26:
            pc=myDiv(pc);
            break;
        case 27:
            pc=divu(pc);
            break;
        case 42:
            pc=slt(pc);
            break;
        case 43:
            pc=sltu(pc);
            break;
        case 36:
            pc=myAnd(pc);
            break;
        case 37:
            pc=myOr(pc);
            break;
        case 39:
            pc=nor(pc);
            break;
        case 40:
            pc=myXor(pc);
            break;
        case 8:
            pc=jr(pc);
            break;
        case 9:
            pc=jalr(pc);
            break;
        case 0:
            pc=nop(pc);
            break;
        case 16:
            pc=mfhi(pc);
            break;
        case 18:
            pc=mflo(pc);
            break;
        default:
            break;
        }
        break;
    case 0x00000010:
        numRt=instru<<11>>27;
        numRd=instru<<16>>27;
        RS1=myRegister+numRt;
        if(numRd==14)
        {
            pc=mfepc(pc);
        }
        else if(numRd==13)
        {
            pc=mfco(pc);
        }
        else return -1;
        break;
    case 0x00000031:
        numRs=instru<<6>>27;
        numFt=instru<<11>>27;
        RS2=myRegister+numRs;
        FS1=myFloatReg+numFt;
        lig=instru<<16>>16;
        pc=lwc1(pc);


            //printf("/********\nL.S %u %u\n****************/\n",numFt,numRs);

        break;
    case 0x0000001F:
        numRs=instru<<6>>27;
        numFt=instru<<11>>27;
        RS2=myRegister+numRs;
        FS1=myFloatReg+numFt;
        lig=instru<<16>>16;
        pc=S_D(pc);


            //printf("/********\nL.D %u %u\n****************/\n",numFt,numRs);

        break;
    case 0x0000001E:
        numRs=instru<<6>>27;
        numFt=instru<<11>>27;
        RS2=myRegister+numRs;
        FS1=myFloatReg+numFt;
        lig=instru<<16>>16;
        //printf("/********\nS.D %u %u\n****************/\n",numFt,numRs);
        pc=S_D(pc);
        break;

    case 0x00000039:
        numRs=instru<<6>>27;
        numFt=instru<<11>>27;
        RS2=myRegister+numRs;
        FS1=myFloatReg+numFt;
        lig=instru<<16>>16;
        //printf("/********\nS.S %u %u\n****************/\n",numFt,numRs);
        pc=swc1(pc);
        break;
    case 0x00000011:
        numFt=instru<<11>>27;
        numFs=instru<<16>>27;
        numFd=instru<<21>>27;
        FS1=myFloatReg+numFt;
        FS2=myFloatReg+numFs;
        FD=myFloatReg+numFd;
        numRs=instru<<6>>27;
        tmp_fuc=instru%64;
        //printf("%u %u\n",tmp_fuc,numRs);
        if(numRs==0)
        {
            switch(tmp_fuc)
            {
            case 0:
                pc=add_s(pc);
                break;
            case 1:
                pc=sub_s(pc);
                break;
            case 2:
                pc=mul_s(pc);
            case 3:
                pc=div_s(pc);
            default:
                break;
            }
        }
        else if(numRs==1)
        {
            switch(tmp_fuc)
            {
            case 0:
                pc=add_d(pc);
                //printf("/****************\nADD.D %u %u %u\n*****************/\n",numFd,numFt,numFs);
                break;
            case 1:
                pc=sub_d(pc);
                break;
            case 2:
                pc=mul_d(pc);
            case 3:
                pc=div_d(pc);
            default:
                break;
            }
        }
        default:break;
    }
    pcShort=pc%0x00010000;
    //printf("%u %u\n",pc,pcShort);
    //printf("%u %u\n",pcShort,programTail);
    }
    return 0;
}
/*test_main()
{
    init();
    unsigned int i;
    //对寄存器进行赋值
    for(i=1;i<=32;i++)
    {
      myRegister[i]=i;          //对寄存器进行赋值
    }
    //显示MyMemory剩余空间
    printf("the remaining storage of memory now is %dB\n",MyMemory->FreePage->freeCount*512);
    //将第26个寄存器的值写到MyMemory，逻辑地址为1258*4
    writeMymemory(1258*4,myRegister+25);
    printf("the remaining storage of memory now is %dB\n",MyMemory->FreePage->freeCount*512);
    //分别打印出页表和空闲页管理队列
    //printPageHead();
   // printFreePage();
    unsigned int a;
    //将逻辑地址1258*4的值读入a
    readMymemory(1258*4,&a);
    printf("read : %u\n",a);
    myFree(1258*4);
    //printFreePage();
    printf("the remaining storage of memory now is %dB\n",MyMemory->FreePage->freeCount*512);
    return 0;

}*/
/*
主函数基于课本151页的测试循环小程序进行编写
*/
int main()
{
    init();//初始化
    int i;
    FILE* program;
    program=fopen("input.txt","r+");//设置输入文件
  //设置R1和R2
    myRegister[1]=0x00008000;
    myRegister[2]=0x00007F00;
    double* test=myFloatReg+2;
    *test=1.5;//设置F2
    printf("/****************** 执行前*******************/\n从0(R1)到8(R2)的值向量(使用双精度浮点数型读)：\n");
    for(i=0;i<=31;i++)
    {
        unsigned long long tmp=0;
        double* p=&tmp;
        unsigned short addr=addrToMyAddr(32768-8*i);
        unsigned char* tmp_char=&tmp;
        int j;
        for(j=0;j<8;j++)
        {
           readMymemory(addr+j,tmp_char+j);
        }
        printf("%lf\n",*p);

    }
    printf("寄存器状态：\n");
    printMygister();  //显示通用寄存器
    printFloatReg();  //显示浮点数寄存器
    exe(program);//开始执行
    //显示存储器中的值向量
    printf("/****************** 执行后*******************/\n从0(R1)到8(R2)的值向量(使用双精度浮点数型读):\n");
    for(i=0;i<=31;i++)
    {
        unsigned long long tmp=0;
        double* p=&tmp;
        unsigned short addr=addrToMyAddr(32768-8*i);
        unsigned char* tmp_char=&tmp;
        int j;
        for(j=0;j<8;j++)
        {
           readMymemory(addr+j,tmp_char+j);
        }
        printf("%lf\n",*p);

    }
    printf("寄存器状态：\n");
    printMygister();  //显示通用寄存器
    printFloatReg();  //显示浮点数寄存器
    return 0;
}
