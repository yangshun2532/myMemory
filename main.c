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
  MyMemory->myBuffer=(char*)malloc(MyMemory->vm_size);
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
  printf("begin malloc\n");
  unsigned short p_addr=((MyMemory->FreePage->head)<<9)+offest;
  //printf("offset is %u\n",p_addr);
  unsigned short a=MyMemory->FreePage->head;
  if(MyMemory->FreePage->firstFree[a]<128)
    MyMemory->FreePage->firstFree[a]+=128; //最高位置1，表示非空闲
  //printf("%d",MyMemory->FreePage->firstFree[a]);
  MyMemory->FreePage->head=(unsigned short)(MyMemory->FreePage->firstFree[a]%128);//修改头指针
  MyMemory->FreePage->freeCount--;
  printf("finish malloc\n");
  printf("the malloc p_addr is : %u\n",p_addr);
  return p_addr;
}
/**************************************************************
函数名：myFtl
功能：将逻辑地址转换成物理地址
**************************************************************/
unsigned short myFtl(unsigned short v_addr)
{
  printf("begin ftl\n");
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
  printf("finish ftl\n");
  printf("the p_addr is : %u\n",p_addr);
  return p_addr;
}
/**************************************************************
函数名：myFree
功能：释放内存空间
**************************************************************/
int myFree(unsigned short v_addr)
{
  printf("begin freeing\n");
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
  printf("finish freeing\n");
  return 0;
}
/**************************************************************
函数名：readMymemory
功能：将数据从MyMemory读入到我的寄存器
**************************************************************/

int readMymemory(unsigned short addr,unsigned char* Register)
{
  printf("begin read\n");
  //if((addr%4!=0)||(addr<512))
  //return -1;
  unsigned char* tmp=MyMemory->myBuffer+myFtl(addr);//读一个字节
  *Register=*tmp;
  printf("finish read\n");
  return 0;
}
/**************************************************************
函数名：writeMymemory
功能：将寄存器数据写到MyMemory
**************************************************************/
int writeMymemory(unsigned short addr,unsigned* Register)
{
    printf("begin write\n");
    //if(addr%4!=0||addr<512)
       // return -1;
       unsigned* tmp=MyMemory->myBuffer+myFtl(addr);
    *tmp=*Register;
    printf("finish write\n");
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
int main()
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

}
test_DADDU_Demon()
{
    myRegister[1]=0x12345678;
    myRegister[2]=0x87654321;
    RS1=myRegister+1;
    RS2=myRegister+2;
    RD=myRegister+3;
    DADDU(0);
    printf("%X",myRegister[3]);
    return 0;
}
