#include "stm32f10x.h"
#include "math.h"

//Servo:91to165,left to right,middle128
//Motor:0to255,back to forth,middle128
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
u8 speed=128;
u8 mode=1,mode2=0; //电机状态,0是停止,1正转,2反转
u16 counter=500;
u16 AD1_Value;
u16 r=2048,pwm1=0,pwm2=0; //给定值
u8 time=0;
int e1=0,e2=0; //e1是当前偏差
u8 RxBuf[3]={0x80,0x80,0x00};

void RCC_Configuration(void);	 //时钟初始化，开启外设时钟
void GPIO_Configuration(void);	 //IO口初始化，配置其功能
void tim3(void);				 //定时器tim4初始化配置
void tim4(void);				 //定时器tim4初始化配置
void nvic(void);				 //中断优先级等配置
void ADC(void);

void RCC_Configuration(void)				 //使用任何一个外设时，务必开启其相应的时钟
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO, ENABLE);	  //使能APB2控制外设的时钟，包括GPIOA, 功能复用时钟AFIO等，
                                                                              //其他包括的外设，详见固件库手册等资料
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM3, ENABLE); //使能APB1控制外设的时钟，定时器tim4，其他外设详见手册

                
}


void GPIO_Configuration(void)			 //使用某io口输入输出时，请务必对其初始化配置
{
    GPIO_InitTypeDef GPIO_InitStructure;   //定义格式为GPIO_InitTypeDef的结构体的名字为GPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;
		 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	   GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//失能STM32 JTAG烧写功能，只能用SWD模式烧写，解放出PA15和PB中部分IO口
}


void tim3()							  //配置TIM3为基本定时器模式 ，约100kA
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //定义格式为TIM_TimeBaseInitTypeDef的结构体的名字为TIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =999;		  //配置计数阈值为999，超过时，自动清零，并触发中断
		TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 时钟预分频值，定时器的计数的频率等于主时钟频率除以该预分频值
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // 时钟分频倍数(用于数字滤波，暂时无用)
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // 计数方式为向上计数

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 //	 初始化tim3
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除TIM3溢出中断标志
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //  使能TIM3的溢出更新中断
    TIM_Cmd(TIM3,ENABLE);					  //		   使能TIM3
}

void tim4()							  //配置TIM4为基本定时器模式 ，约100kA
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //定义格式为TIM_TimeBaseInitTypeDef的结构体的名字为TIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =1;		  //配置计数阈值为9，超过时，自动清零，并触发中断
		TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 时钟预分频值，定时器的计数的频率等于主时钟频率除以该预分频值
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // 时钟分频倍数(用于数字滤波，暂时无用)
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // 计数方式为向上计数

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);		 //	 初始化tim4
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //清除TIM4溢出中断标志
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //  使能TIM4的溢出更新中断
    TIM_Cmd(TIM4,ENABLE);					  //		   使能TIM4
}

void nvic()									//配置中断优先级
{	 
     NVIC_InitTypeDef NVIC_InitStructure;  //	 //	  命名一优先级变量

	 
		 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //要用同一个Group
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3	溢出更新中断
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//	打断优先级为1，与上一个相同，不希望中断相互打断对方
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 	//	响应优先级0
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
	
	   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //要用同一个Group
     NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //TIM4	溢出更新中断
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//	打断优先级为1，与上一个相同，不希望中断相互打断对方
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 	//	响应优先级1，低于上一个，当两个中断同时来时，上一个先执行
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
 
}


void TIM3_IRQHandler(void)		//	  //TIM3的溢出更新中断响应函数
{
	    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 清空TIM3溢出中断响应函数标志位
	
			ADC_SoftwareStartConvCmd(ADC1, ENABLE); //开始单次模式数据转换
	
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
		mode=1;//根据实际情况作出修改
		pwm1=e1;
	}
	else if(e1<-100){
		mode=2;//根据实际情况作出修改
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
void TIM4_IRQHandler(void){		//	  //TIM4的溢出更新中断响应函数
      TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	//	 //	 清空TIM4溢出中断响应函数标志位

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
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   //设置ADC1和ADC2工作在独立工作模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;				// 设置ADC工作在扫描模式（多通道），如果只需要一个通道，配置为单通道模式 DISABLE
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //设置ADC工作在单次模式（使能一次，转换一次；也可以配置连续模式，自动周期性采样
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //设置转换由软件触发，不用外部触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //ADC数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;                          //设置进行转换的通道数目为5
  ADC_Init(ADC1, &ADC_InitStructure);                              //初始化ADC1

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_239Cycles5);  //设置ADC1的通道0第一个进行转换，采样时钟周期239.5个

  ADC_Cmd(ADC1, ENABLE);                           //使能ADC1
  
  ADC_ResetCalibration(ADC1);                        //重置ADC1的校准寄存器
  while(ADC_GetResetCalibrationStatus(ADC1));        //等待ADC1校准重置完成

  ADC_StartCalibration(ADC1);                        //开始ADC1校准
  while(ADC_GetCalibrationStatus(ADC1));             //等待ADC1校准完成
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);            //使能ADC1软件开始转换
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
