#include "NVIC.h"

void NVIC_Config(void)
{	
	/*定义一个NVIC_InitTypeDef 类型的结构体，名字叫NVIC_InitStructure*/ 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 配置中断使用组合0*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		
	/*配置选中的中断向量*/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;

	/*配置抢占优先级*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;

	/*配置响应优先级*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	/*使能中断向量*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	/*调用库函数，初始化中断向量*/
	NVIC_Init(&NVIC_InitStructure);
	
		 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //要用同一个Group
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3	溢出更新中断
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//	打断优先级为1，与上一个相同，不希望中断相互打断对方
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 	//	响应优先级1，低于上一个，当两个中断同时来时，上一个先执行
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
						 
	
}
void tim3()							  //配置TIM3为基本定时器模式 ，约100kA
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //定义格式为TIM_TimeBaseInitTypeDef的结构体的名字为TIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =9999;		  //配置计数阈值为9999，超过时，自动清零，并触发中断
		TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 时钟预分频值，定时器的计数的频率等于主时钟频率除以该预分频值
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // 时钟分频倍数(用于数字滤波，暂时无用)
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // 计数方式为向上计数

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 //	 初始化tim3
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除TIM3溢出中断标志
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //  使能TIM3的溢出更新中断
    TIM_Cmd(TIM3,ENABLE);					  //		   使能TIM3
}
void TIM3_IRQHandler(void)		//	  //TIM3的溢出更新中断响应函数
{
	    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 清空TIM3溢出中断响应函数标志位
	
			ADC_SoftwareStartConvCmd(ADC1, ENABLE); //开始单次模式数据转换
   
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
