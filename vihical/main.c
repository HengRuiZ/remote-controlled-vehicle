#include "stm32f10x.h"
#include "math.h"

//Servo:91to165,left to right,middle128
//Motor:0to255,back to forth,middle128
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
u8 speed=128;
u8 mode=1,mode2=0; //���״̬,0��ֹͣ,1��ת,2��ת
u16 counter=500;
u16 AD1_Value;
u16 r=2048,pwm1=0,pwm2=0; //����ֵ
u8 time=0;
int e1=0,e2=0; //e1�ǵ�ǰƫ��
u8 RxBuf[3]={0x80,0x80,0x00};

void RCC_Configuration(void);	 //ʱ�ӳ�ʼ������������ʱ��
void GPIO_Configuration(void);	 //IO�ڳ�ʼ���������书��
void tim3(void);				 //��ʱ��tim4��ʼ������
void tim4(void);				 //��ʱ��tim4��ʼ������
void nvic(void);				 //�ж����ȼ�������
void ADC(void);

void RCC_Configuration(void)				 //ʹ���κ�һ������ʱ����ؿ�������Ӧ��ʱ��
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO, ENABLE);	  //ʹ��APB2���������ʱ�ӣ�����GPIOA, ���ܸ���ʱ��AFIO�ȣ�
                                                                              //�������������裬����̼����ֲ������
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM3, ENABLE); //ʹ��APB1���������ʱ�ӣ���ʱ��tim4��������������ֲ�

                
}


void GPIO_Configuration(void)			 //ʹ��ĳio���������ʱ������ض����ʼ������
{
    GPIO_InitTypeDef GPIO_InitStructure;   //�����ʽΪGPIO_InitTypeDef�Ľṹ�������ΪGPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;
		 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	   GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//ʧ��STM32 JTAG��д���ܣ�ֻ����SWDģʽ��д����ų�PA15��PB�в���IO��
}


void tim3()							  //����TIM3Ϊ������ʱ��ģʽ ��Լ100kA
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //�����ʽΪTIM_TimeBaseInitTypeDef�Ľṹ�������ΪTIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =999;		  //���ü�����ֵΪ999������ʱ���Զ����㣬�������ж�
		TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 ʱ��Ԥ��Ƶֵ����ʱ���ļ�����Ƶ�ʵ�����ʱ��Ƶ�ʳ��Ը�Ԥ��Ƶֵ
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // ʱ�ӷ�Ƶ����(���������˲�����ʱ����)
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // ������ʽΪ���ϼ���

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 //	 ��ʼ��tim3
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //���TIM3����жϱ�־
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //  ʹ��TIM3����������ж�
    TIM_Cmd(TIM3,ENABLE);					  //		   ʹ��TIM3
}

void tim4()							  //����TIM4Ϊ������ʱ��ģʽ ��Լ100kA
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //�����ʽΪTIM_TimeBaseInitTypeDef�Ľṹ�������ΪTIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =1;		  //���ü�����ֵΪ9������ʱ���Զ����㣬�������ж�
		TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 ʱ��Ԥ��Ƶֵ����ʱ���ļ�����Ƶ�ʵ�����ʱ��Ƶ�ʳ��Ը�Ԥ��Ƶֵ
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // ʱ�ӷ�Ƶ����(���������˲�����ʱ����)
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // ������ʽΪ���ϼ���

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);		 //	 ��ʼ��tim4
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //���TIM4����жϱ�־
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //  ʹ��TIM4����������ж�
    TIM_Cmd(TIM4,ENABLE);					  //		   ʹ��TIM4
}

void nvic()									//�����ж����ȼ�
{	 
     NVIC_InitTypeDef NVIC_InitStructure;  //	 //	  ����һ���ȼ�����

	 
		 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //Ҫ��ͬһ��Group
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3	��������ж�
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//	������ȼ�Ϊ1������һ����ͬ����ϣ���ж��໥��϶Է�
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 	//	��Ӧ���ȼ�0
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
	
	   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //Ҫ��ͬһ��Group
     NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //TIM4	��������ж�
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//	������ȼ�Ϊ1������һ����ͬ����ϣ���ж��໥��϶Է�
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 	//	��Ӧ���ȼ�1��������һ�����������ж�ͬʱ��ʱ����һ����ִ��
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
 
}


void TIM3_IRQHandler(void)		//	  //TIM3����������ж���Ӧ����
{
	    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 ���TIM3����ж���Ӧ������־λ
	
			ADC_SoftwareStartConvCmd(ADC1, ENABLE); //��ʼ����ģʽ����ת��
	
	AD1_Value=ADC_GetConversionValue(ADC1);
	r=RxBuf[0]*16;
	e1=r-AD1_Value; 
	e2=speed-128;
	if(e2>0)
	{
		mode2=1;
		pwm2=e2;
	}
	else if(e2<0)
	{
		mode2=2;
		pwm2=-e2;
	}
	else pwm2=mode2=0;
	pwm2*=8;
	if(e1>100){
		mode=1;//����ʵ����������޸�
		pwm1=e1;
	}
	else if(e1<-100){
		mode=2;//����ʵ����������޸�
		pwm1=-e1;
	}
	else{
		mode=0;
	}
	if(mode==0){
		 GPIO_SetBits(GPIOB,GPIO_Pin_10);
		 GPIO_SetBits(GPIOB,GPIO_Pin_11);
	}
	if(mode==1)
	   GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	if(mode==2)
		 GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	if(mode2==0)
	{
			 GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		 GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	}
	if(mode2==1)
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	if(mode2==2)
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
}
void TIM4_IRQHandler(void){		//	  //TIM4����������ж���Ӧ����
      TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	//	 //	 ���TIM4����ж���Ӧ������־λ

	if(counter==999)
	  counter = 0;
	else 
	  counter +=1;
	
	 if(counter>pwm1){
		  if(mode==1)
		     GPIO_SetBits(GPIOB,GPIO_Pin_10); 
			if(mode==2)
				 GPIO_SetBits(GPIOB,GPIO_Pin_11);
	 }
    else{
			if(mode==1)
		     GPIO_ResetBits(GPIOB,GPIO_Pin_10);
			if(mode==2)
				 GPIO_ResetBits(GPIOB,GPIO_Pin_11); 
		}		
		
	if(mode2==2)
	{
		if(pwm2>counter)
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
		else
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	}
	if(mode2==1)
	{
		if(pwm2>counter)
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
		else
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	}
}
	
void delay_nus(u32 n)
{
  u8 i;
  while(n--)
  {
    i=4;
    while(i--);
  }
}

void delay_nms(u32 n)
{
    while(n--)
      delay_nus(1000);
}

void ADC()
{										 
	ADC_InitTypeDef ADC_InitStructure;
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   //����ADC1��ADC2�����ڶ�������ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;				// ����ADC������ɨ��ģʽ����ͨ���������ֻ��Ҫһ��ͨ��������Ϊ��ͨ��ģʽ DISABLE
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //����ADC�����ڵ���ģʽ��ʹ��һ�Σ�ת��һ�Σ�Ҳ������������ģʽ���Զ������Բ���
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //����ת������������������ⲿ����
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //ADC�����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1;                          //���ý���ת����ͨ����ĿΪ5
  ADC_Init(ADC1, &ADC_InitStructure);                              //��ʼ��ADC1

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_239Cycles5);  //����ADC1��ͨ��0��һ������ת��������ʱ������239.5��

  ADC_Cmd(ADC1, ENABLE);                           //ʹ��ADC1
  
  ADC_ResetCalibration(ADC1);                        //����ADC1��У׼�Ĵ���
  while(ADC_GetResetCalibrationStatus(ADC1));        //�ȴ�ADC1У׼�������

  ADC_StartCalibration(ADC1);                        //��ʼADC1У׼
  while(ADC_GetCalibrationStatus(ADC1));             //�ȴ�ADC1У׼���
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);            //ʹ��ADC1�����ʼת��
}


int main(void)
{
  RCC_Configuration();															   		
  GPIO_Configuration();	
	ADC();
	nvic(); 
	tim3();
	tim4();
	delay_nms(500);			  
	while(1)
	{
		if(RxBuf[1]>speed){speed++;delay_nms(100);}
		if(RxBuf[1]<speed){speed--;delay_nms(100);}
  }	
}	 
