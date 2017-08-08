#include "NVIC.h"

void NVIC_Config(void)
{	
	/*����һ��NVIC_InitTypeDef ���͵Ľṹ�壬���ֽ�NVIC_InitStructure*/ 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* �����ж�ʹ�����0*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		
	/*����ѡ�е��ж�����*/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;

	/*������ռ���ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;

	/*������Ӧ���ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	/*ʹ���ж�����*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	/*���ÿ⺯������ʼ���ж�����*/
	NVIC_Init(&NVIC_InitStructure);
	
		 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //Ҫ��ͬһ��Group
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3	��������ж�
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//	������ȼ�Ϊ1������һ����ͬ����ϣ���ж��໥��϶Է�
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 	//	��Ӧ���ȼ�1��������һ�����������ж�ͬʱ��ʱ����һ����ִ��
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
						 
	
}
void tim3()							  //����TIM3Ϊ������ʱ��ģʽ ��Լ100kA
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //�����ʽΪTIM_TimeBaseInitTypeDef�Ľṹ�������ΪTIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =9999;		  //���ü�����ֵΪ9999������ʱ���Զ����㣬�������ж�
		TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 ʱ��Ԥ��Ƶֵ����ʱ���ļ�����Ƶ�ʵ�����ʱ��Ƶ�ʳ��Ը�Ԥ��Ƶֵ
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // ʱ�ӷ�Ƶ����(���������˲�����ʱ����)
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // ������ʽΪ���ϼ���

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 //	 ��ʼ��tim3
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //���TIM3����жϱ�־
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //  ʹ��TIM3����������ж�
    TIM_Cmd(TIM3,ENABLE);					  //		   ʹ��TIM3
}
void TIM3_IRQHandler(void)		//	  //TIM3����������ж���Ӧ����
{
	    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 ���TIM3����ж���Ӧ������־λ
	
			ADC_SoftwareStartConvCmd(ADC1, ENABLE); //��ʼ����ģʽ����ת��
   
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
