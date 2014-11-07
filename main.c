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
  MyMemory->myBuffer=(char*)malloc(MyMemory->vm_size);
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
  printf("begin malloc\n");
  unsigned short p_addr=((MyMemory->FreePage->head)<<9)+offest;
  //printf("offset is %u\n",p_addr);
  unsigned short a=MyMemory->FreePage->head;
  if(MyMemory->FreePage->firstFree[a]<128)
    MyMemory->FreePage->firstFree[a]+=128; //���λ��1����ʾ�ǿ���
  //printf("%d",MyMemory->FreePage->firstFree[a]);
  MyMemory->FreePage->head=(unsigned short)(MyMemory->FreePage->firstFree[a]%128);//�޸�ͷָ��
  MyMemory->FreePage->freeCount--;
  printf("finish malloc\n");
  printf("the malloc p_addr is : %u\n",p_addr);
  return p_addr;
}
/**************************************************************
��������myFtl
���ܣ����߼���ַת���������ַ
**************************************************************/
unsigned short myFtl(unsigned short v_addr)
{
  printf("begin ftl\n");
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
  printf("finish ftl\n");
  printf("the p_addr is : %u\n",p_addr);
  return p_addr;
}
/**************************************************************
��������myFree
���ܣ��ͷ��ڴ�ռ�
**************************************************************/
int myFree(unsigned short v_addr)
{
  printf("begin freeing\n");
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
  printf("finish freeing\n");
  return 0;
}
/**************************************************************
��������readMymemory
���ܣ������ݴ�MyMemory���뵽�ҵļĴ���
**************************************************************/

int readMymemory(unsigned short addr,unsigned char* Register)
{
  printf("begin read\n");
  //if((addr%4!=0)||(addr<512))
  //return -1;
  unsigned char* tmp=MyMemory->myBuffer+myFtl(addr);//��һ���ֽ�
  *Register=*tmp;
  printf("finish read\n");
  return 0;
}
/**************************************************************
��������writeMymemory
���ܣ����Ĵ�������д��MyMemory
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
int main()
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
