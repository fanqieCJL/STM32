#include"stm32f10x.h"
#include"System_Configuration.h"
#include"LCD.h"
#include"Seg.h"


u8 Key_Pressed_Cnt=0;
u8 Key_F=1,Key_B=1;
u32 Delay_1ms_Cnt=0;
void Delay_1ms(u32 cnt);
//void Key_Scan(void);
void SerialPutChar(u8 c);

/* ���� */
// 4*4�������  
struct io_port {                                           
  GPIO_TypeDef *GPIO_x;                  
  unsigned short GPIO_pin; 
};           
static struct io_port key_output[4] = { 
{GPIOF, GPIO_Pin_6},      //0��
{GPIOC, GPIO_Pin_10},     //1��
{GPIOF, GPIO_Pin_7},      //2��
{GPIOC, GPIO_Pin_13}};    //3��
static struct io_port key_input[4] = { 
{GPIOC, GPIO_Pin_14},     //0��
{GPIOE, GPIO_Pin_5},      //1��
{GPIOC, GPIO_Pin_15},     //2��
{GPIOE, GPIO_Pin_6} };    //3��

char key[4][4]; 

void GPIO_Configuration(void);  
void CleanKeyNunber(void);      
void  Key_Scan(void);           
void Delay1ms(unsigned int cnt_1ms);
char TransferNumber(void);      

/* ------- */

int main(void)
{
  /* ���� */
  char Key_val,Disp_Key_val;
  CleanKeyNunber();         //�������λ����Ϣ key[i][j] = 0
  GPIO_Configuration();     //IO��ʼ��
  SEG_GPIO_Configuration()
  /* ------------ */
  
  int num = -1;
  RCC_Configuration(); //ʱ������
  //NVIC_Configuration();//�ж�
  //GPIO_Configuration();//IO
  //TIM2_Configuration();//10ms ��ʱ��
  SysTick_delay_init(); //�趨SysTich Ƶ��Ϊ72M/72000=1ms
  //USART_Configuration(); //���ô���
  LCM_IO_Init();
  Init_ST7567();
  ClearScreen();    //clear all dots
  //DisplayFont(1, 1, 17, FONT_8X16);
  //DisplayFont(2, 8, 18, FONT_8X16);
  
  
  while(1)
  {
    CleanKeyNunber();	      //�������λ����Ϣ key[i][j] = 0
    Key_Scan();               //����ɨ�裬���ذ���λ����Ϣ key[i][j] = 1
    Key_val=TransferNumber(); //key[i][j] = 1����Ϊ0-15��������0xff
    if(Key_val != Disp_Key_val & Key_val!=0xff)
      num++;
    if(Key_val!=0xff)
    {
      Disp_Key_val=Key_val;
    }    
   // ShowNunber(Disp_Key_val); //��ʾ��ֵ 0-15  
    CleanKeyNunber();	      //�������λ����Ϣ key[i][j] = 0
    
    int data[] = {17,18,19,20,21,22,23,24,25,26,34,35,36,37,5,4};
    DisplayFont(1, num*8+1, data[Disp_Key_val], FONT_8X16);
  }
}

/*
void Key_Scan(void)
{
 
  Key_F=KEY2;
  if((Key_F==0)&&(Key_B==1))
  {
    Key_Pressed_Cnt++;//��⵽����
    if(Key_Pressed_Cnt==2)
      Key_Pressed_Cnt=0;
  }
  Key_B=Key_F;
}
*/
void Delay_1ms(u32 cnt)
{
  Delay_1ms_Cnt=cnt;
  while(Delay_1ms_Cnt); //�ȴ�����SysTick�ж�cnt�˺��˳�Delay_1ms����
}
/*******************************************************************************
* Function Name  : SerialPutChar
* Description    : Print a character on the HyperTerminal
* Input          : - c: The character to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void SerialPutChar(u8 c)
{
  USART_SendData(USART1, c);                                  //��Ҫ���͵�������䵽���ͻ�����
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//�ȴ����ͽ���
}

void CleanKeyNunber(void)
{
  uint8_t i,j;
  for(i = 0 ; i < 4; i ++)
    for(j = 0 ; j < 4; j ++)		
        key[i][j] = 0;
}

void  Key_Scan(void)  //������ʱ���İ���ɨ�����
{	
  char i, j;  
  for(i = 0; i < 4; i++)             //i������ڣ������õ͵�ƽ 
  {   
    GPIO_ResetBits(key_output[i].GPIO_x, key_output[i].GPIO_pin);       
    for(j = 0; j < 4; j++)            //j������ڣ���������ʱ��ͨ����Ϊ�͵�ƽ    
    {      
        if(GPIO_ReadInputDataBit(key_input[j].GPIO_x, key_input[j].GPIO_pin) == 0)
        {
          Delay1ms(10);            //��ʱȥ��
          if(GPIO_ReadInputDataBit(key_input[j].GPIO_x, key_input[j].GPIO_pin) == 0)
          {
            while(GPIO_ReadInputDataBit(key_input[j].GPIO_x, key_input[j].GPIO_pin) == 0);
            key[i][j] = 1; 
          }                
        }
        else
                key[i][j] = 0;
    }
    GPIO_SetBits(key_output[i].GPIO_x, key_output[i].GPIO_pin);
  }
}

 void Delay1ms(unsigned int cnt_1ms)
{
  unsigned int i;
  while(cnt_1ms--)   //Ĭ��ϵͳ��ʱ��72M�������Ч��Լ1/8,��һ������ԼΪ1ms
  {
    i=8000;             
    while(i--);
  }
}

char TransferNumber(void)//�������룺����ʵ�����Ӳ������,�Ѱ�����λ����Ϣkey[i][j] = 1����Ϊ0-15����
{
  char Key_Value;
    if(key[0][0] == 1)
            Key_Value = 14;
    else if(key[0][1] == 1)
            Key_Value = 7;
    else if(key[0][2] == 1)
            Key_Value = 4 ;
    else if(key[0][3] == 1)
            Key_Value = 1;
    else if(key[1][0] == 1)
            Key_Value = 0;
    else if(key[1][1] == 1)
            Key_Value = 8;
    else if(key[1][2] == 1)
            Key_Value = 5;
    else if(key[1][3] == 1)
            Key_Value = 2;
    else if(key[2][0] == 1)
            Key_Value = 15 ;
    else if(key[2][1] == 1)
            Key_Value = 9 ;
    else if(key[2][2] == 1)
            Key_Value = 6;
    else if(key[2][3] == 1)
            Key_Value = 3;
    else if(key[3][0] == 1)
            Key_Value = 13;
    else if(key[3][1] == 1)
            Key_Value = 12;
    else if(key[3][2] == 1)
            Key_Value = 11;
    else if(key[3][3] == 1)
            Key_Value = 10;
    else 
            Key_Value = 0xff;
    return Key_Value;	
}

void GPIO_Configuration(void)
{
  char i;
   GPIO_InitTypeDef  GPIO_InitStructure;
  //����������ʼ������  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOF| RCC_APB2Periph_GPIOG, ENABLE);//ʹ��PA,PB,PC,PG,PF�˿�ʱ��
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;//KEY0-KEY2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PC0���ó����룬 KEY3
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  //���а�����ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;				 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOF, &GPIO_InitStructure);					 //�����趨������ʼ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;				 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;				 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��
  
  
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE
  
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOE
  
  for(i = 0; i < 4; i++) 
     GPIO_SetBits(key_output[i].GPIO_x, key_output[i].GPIO_pin); 
  
  
 //LED IO��ʼ��
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);				//�����趨������ʼ��
 GPIO_ResetBits(GPIOA,GPIO_Pin_8);		                //LED0
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);				 //LED7
 GPIO_ResetBits(GPIOB,GPIO_Pin_0);						 

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 
 GPIO_ResetBits(GPIOC,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);//LED1,LED2,LED3,LED4,LED5

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOG, &GPIO_InitStructure);				 //�����趨������ʼ
 GPIO_ResetBits(GPIOG, GPIO_Pin_6|GPIO_Pin_7);                  //LED6,LED7		 

}