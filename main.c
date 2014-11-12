/*************************************************
ʵ����ҳʽ����洢���ڴ�ķ�����ͷţ���ʾ�ڴ�ʣ���������������ȹ��ܡ�
MyMemory��512BΪһҳ����128ҳ����һҳ���ڴ��ҳ��Ϳ��п������С�
ҳ����128��item��ÿ��itemΪ8��bit����һ����̬���顣��һ��bitΪ��Чλ��
����λ��Ӧ��ҳ�������ַ��
����ҳ������й���128��item��ÿ��itemҲ��8��bit����һ����̬������һ��bit
��ʾ������ҳ�Ƿ���У�����λ��ָ����һ������ҳ��ָ�롣����ҳʱ���Ӷ���ȡ��
�ͷ��ڴ�ҳʱ�����ڶ�β��
**************************************************/
#include <stdio.h>
#include <stdlib.h>
#include"INSN_ADD.h"
#include "load.h"
#include "logic.h"
#include "jump.h"
#include "float_cal.h"

/*��̬����freePage���������������ҳ������ľ���Ĺ���*/
struct freePage
{
  unsigned char* firstFree;
  unsigned short head;
  unsigned short end;
  unsigned short count;
  int freeCount;
};
/*�ڴ�*/
struct myMemory
{
  unsigned char* myBuffer;//�ڴ��׵�ַ
  unsigned  vm_size; //�ڴ��С
  unsigned char* vm_start;
  unsigned char* vm_end;
  unsigned char* pageHead;//��ַӳ���
  struct freePage* FreePage;//����ҳ�������
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
  /*��ҳ��Ϳ���ҳ������ж�����myMemory�ĵ�һҳ��ҳ�����ʼ��ַ����myMemory����
  ʼ��ַ������ҳ����ģ�����ʼ��ַΪ256B����ʼ��ҳ��Ϳ���ҳ�������*/
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
��������myMalloc
���ܣ��������ҳ
**************************************************************/
unsigned short myMalloc(unsigned short offest)
{
  //printf("begin malloc\n");
  unsigned short p_addr=((MyMemory->FreePage->head)<<9)+offest;
  //printf("offset is %u\n",p_addr);
  unsigned short a=MyMemory->FreePage->head;
  if(MyMemory->FreePage->firstFree[a]<128)
    MyMemory->FreePage->firstFree[a]+=128; //���λ��1����ʾ�ǿ���
  //printf("%d",MyMemory->FreePage->firstFree[a]);
  MyMemory->FreePage->head=(unsigned short)(MyMemory->FreePage->firstFree[a]%128);//�޸�ͷָ��
  MyMemory->FreePage->freeCount--;
  //printf("finish malloc\n");
  //printf("the malloc p_addr is : %u\n",p_addr);
  return p_addr;
}
/**************************************************************
��������myFtl
���ܣ����߼���ַת���������ַ
**************************************************************/
unsigned short myFtl(unsigned short v_addr)
{
  //printf("begin ftl\n");
  unsigned short v_page=v_addr>>9;
  unsigned short p_addr=0;
  if(MyMemory->pageHead[v_page]/128==1)//���λΪ1����Ч��������ҳ
  {
    p_addr=myMalloc(v_addr%512);
    MyMemory->pageHead[v_page]=(unsigned char)(p_addr/512);
  }
  else//��Ч��ֱ�Ӳ��ת��
  {
    p_addr=((unsigned short)MyMemory->pageHead[v_page]<<9)+v_addr%512;
  }
  //printf("finish ftl\n");
  //printf("the p_addr is : %u\n",p_addr);
  return p_addr;
}
/**************************************************************
��������myFree
���ܣ��ͷ��ڴ�ռ�
**************************************************************/
int myFree(unsigned short v_addr)
{
  //printf("begin freeing\n");
  if(MyMemory->pageHead[v_addr/512]/128==1)
  {
      return 0;//�����߼���ַ�������ǿ��У�����
  }
  unsigned short p_addr=myFtl(v_addr);
  MyMemory->FreePage->firstFree[p_addr/512]-=128;//�޸���Чλ
  unsigned short a=MyMemory->FreePage->end;
  MyMemory->FreePage->firstFree[a]=(unsigned char)(p_addr/512);
  if(MyMemory->FreePage->firstFree[a]>=128)
    MyMemory->FreePage->firstFree[a]-=128;
  MyMemory->FreePage->freeCount++;
  MyMemory->FreePage->end=p_addr/512; //�޸�βָ��
  //printf("finish freeing\n");
  return 0;
}
/**************************************************************
��������readMymemory
���ܣ������ݴ�MyMemory���뵽�ҵļĴ���
**************************************************************/

int readMymemory(unsigned short addr,unsigned char* Register)
{
  //printf("begin read\n");
  //if((addr%4!=0)||(addr<512))
  //return -1;
  unsigned char* tmp=MyMemory->myBuffer+myFtl(addr);//��һ���ֽ�
  *Register=*tmp;
  //printf("finish read\n");
  return 0;
}
/**************************************************************
��������writeMymemory
���ܣ����Ĵ�������д��MyMemory
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
  //��ҳ���Զ�������ʽ���
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
    //������ҳ��������Զ�������ʽ���
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
        printf("F%d(������):  ",i);
        p_s=myFloatReg+i;
        p_d=myFloatReg+i;
        printf("F%d:  ",i);
        printf("%f\tF%d��˫���ȣ�: %lf\n",*p_s,i,*p_d);
    }
    return;
}


int exe(FILE* program)  //programָ����д�ִ�г����������ļ�
{

     char* tmp_instru=(char*)malloc(33*sizeof(char)); //��������
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
         }//��������תΪunsi
         unsigned char* tmp_R=&instru;
         for(i=0;i<4;i++)
         {
             writeMymemory(programTail+i,tmp_R+i);//װ��ָ��
         }
         programTail+=4;//���һ��ָ�����һ��ָ��ĵ�ַ�������жϳ����Ƿ�ִ����
     }
     pcShort=programHead;
     pc=pcShort;
     while(pcShort!=programTail)
    {
        instru=0;   //ָ��Ĵ�������
    unsigned char* tmp_R=&instru;
    unsigned short addr=addrToMyAddr(pc);
    int i;
    for(i=0;i<4;i++)
    {
        readMymemory(addr+i,tmp_R+i);//ȡָ��
    }
    unsigned tmp=instru>>26;//�õ�ָ��op

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
    //�ԼĴ������и�ֵ
    for(i=1;i<=32;i++)
    {
      myRegister[i]=i;          //�ԼĴ������и�ֵ
    }
    //��ʾMyMemoryʣ��ռ�
    printf("the remaining storage of memory now is %dB\n",MyMemory->FreePage->freeCount*512);
    //����26���Ĵ�����ֵд��MyMemory���߼���ַΪ1258*4
    writeMymemory(1258*4,myRegister+25);
    printf("the remaining storage of memory now is %dB\n",MyMemory->FreePage->freeCount*512);
    //�ֱ��ӡ��ҳ��Ϳ���ҳ�������
    //printPageHead();
   // printFreePage();
    unsigned int a;
    //���߼���ַ1258*4��ֵ����a
    readMymemory(1258*4,&a);
    printf("read : %u\n",a);
    myFree(1258*4);
    //printFreePage();
    printf("the remaining storage of memory now is %dB\n",MyMemory->FreePage->freeCount*512);
    return 0;

}*/
/*
���������ڿα�151ҳ�Ĳ���ѭ��С������б�д
*/
int main()
{
    init();//��ʼ��
    int i;
    FILE* program;
    program=fopen("input.txt","r+");//���������ļ�
  //����R1��R2
    myRegister[1]=0x00008000;
    myRegister[2]=0x00007F00;
    double* test=myFloatReg+2;
    *test=1.5;//����F2
    printf("/****************** ִ��ǰ*******************/\n��0(R1)��8(R2)��ֵ����(ʹ��˫���ȸ������Ͷ�)��\n");
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
    printf("�Ĵ���״̬��\n");
    printMygister();  //��ʾͨ�üĴ���
    printFloatReg();  //��ʾ�������Ĵ���
    exe(program);//��ʼִ��
    //��ʾ�洢���е�ֵ����
    printf("/****************** ִ�к�*******************/\n��0(R1)��8(R2)��ֵ����(ʹ��˫���ȸ������Ͷ�):\n");
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
    printf("�Ĵ���״̬��\n");
    printMygister();  //��ʾͨ�üĴ���
    printFloatReg();  //��ʾ�������Ĵ���
    return 0;
}
