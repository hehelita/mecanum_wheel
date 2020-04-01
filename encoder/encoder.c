#include "encoder.h"

static int count;

//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5�� 		   
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}


//����NVIC 
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;//ȡ����λ  
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//ʹ���ж�λ(Ҫ����Ļ�,�෴������OK) 
	NVIC->IP[NVIC_Channel]|=temp<<4;//������Ӧ���ȼ����������ȼ�   	    	  				   
} 


//�ⲿ�ж����ú���
//ֻ���GPIOA~G;������PVD,RTC��USB����������
//����:
//GPIOx:0~6,����GPIOA~G
//BITx:��Ҫʹ�ܵ�λ;
//TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
//�ú���һ��ֻ������1��IO��,���IO��,���ε���
//�ú������Զ�������Ӧ�ж�,�Լ�������   	    
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//�õ��жϼĴ�����ı��
	EXTOFFSET=(BITx%4)*4; 
	RCC->APB2ENR|=0x01;//ʹ��io����ʱ��			 
	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//���ԭ�����ã�����
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITxӳ�䵽GPIOx.BITx 
	//�Զ�����
	EXTI->IMR|=1<<BITx;//  ����line BITx�ϵ��ж�
	//EXTI->EMR|=1<<BITx;//������line BITx�ϵ��¼� (������������,��Ӳ�����ǿ��Ե�,��������������ʱ���޷������ж�!)
 	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx���¼��½��ش���
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx���¼��������ش���
} 	  
//����������ִ���������踴λ!�����������𴮿ڲ�����.		    
//������ʱ�ӼĴ�����λ		 


//�������ܣ��ⲿ�жϲɼ�������
//��ڲ�������
//����  ֵ����
//**************************************************************************/


void Encoder0_Init() //PA6(TIM3 CH1),PA7(TIM3 CH2)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//????
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler=0x0;
    TIM_TimeBaseStructure.TIM_Period=ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);//???????3

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter=10;
    TIM_ICInit(TIM3,&TIM_ICInitStructure);
    TIM_ClearFlag(TIM3,TIM_FLAG_Update);
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

    TIM_SetCounter(TIM3,0);
    TIM_Cmd(TIM3,ENABLE);
}
void Encoder1_Init() //PB6(TIM4 CH1),PB7(TIM4 CH2)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//????
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler=0x0;
    TIM_TimeBaseStructure.TIM_Period=ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);//???????3

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter=10;
    TIM_ICInit(TIM4,&TIM_ICInitStructure);
    TIM_ClearFlag(TIM4,TIM_FLAG_Update);
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);

    TIM_SetCounter(TIM4,0);
    TIM_Cmd(TIM4,ENABLE);
}
void Encoder2_Init() //PA8(TIM1 CH1),PA9(TIM1 CH2)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler=0x0;
    TIM_TimeBaseStructure.TIM_Period=ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	  TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
    TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM1,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);//???????3

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter=10;
    TIM_ICInit(TIM1,&TIM_ICInitStructure);
    TIM_ClearFlag(TIM1,TIM_FLAG_Update);
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);

    TIM_SetCounter(TIM1,0);
    TIM_Cmd(TIM1,ENABLE);
}

void Encoder3_Init(void)     //B0 B1
{
	RCC->APB2ENR|=1<<0;     	//ʹ��AFIOʱ��	
	RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��	   	
	GPIOB->CRL&=0XFFFFFF00; 
	GPIOB->CRL|=0X00000088;//����	
	Ex_NVIC_Config(1,0,3);		//�����ش���
	Ex_NVIC_Config(1,1,3);		//�����ش���
	MY_NVIC_Init(1,1,EXTI0_IRQn,2);  	//��2
	MY_NVIC_Init(1,1,EXTI1_IRQn,2);  
}

void Encoder_Init()
{
	Encoder0_Init();
	Encoder1_Init();
	Encoder2_Init();
	Encoder3_Init();
}




int Read_Encoder(u8 wheel)
{
    int Encoder_TIM;    
   switch(wheel)
	 {
	   case 0:  Encoder_TIM= (short)TIM3 -> CNT;  TIM3 -> CNT=0;break;		//  
		 case 1:  Encoder_TIM= (short)TIM4 -> CNT;  TIM4 -> CNT=0;break;	
		 case 2:  Encoder_TIM= (short)TIM1 -> CNT;  TIM1 -> CNT=0;break;	
		 case 3:  Encoder_TIM= (short)count;  count=0; break;	//  
		 default: Encoder_TIM=0;
	 }
		return  Encoder_TIM;
}



void TIM3_IRQHandler(void)
{                                   
    if(TIM3->SR&0X0001)
    {                                                   
    }                  
    TIM3->SR&=~(1<<0);
}

void TIM4_IRQHandler(void)
{                                   
    if(TIM4->SR&0X0001)
    {                                                   
    }                  
    TIM4->SR&=~(1<<0);
}
void TIM1_IRQHandler(void)
{                                   
    if(TIM1->SR&0X0001)
    {                                                   
    }                  
    TIM1->SR&=~(1<<0);
}

void EXTI1_IRQHandler(void)
{			
		EXTI->PR=1<<1;  //���LINE�ϵ��жϱ�־λ
	if(PBin(1)==1)
	{
	if(PBin(0)==0) count++;   
	else           count--;
	}
	else
	{
	if(PBin(0)==0)  count--;
	else            count++;
	}		
}
void EXTI0_IRQHandler(void)  //
{
		EXTI->PR=1<<0;  //���LINE�ϵ��жϱ�־λ
		if(PBin(0)==0)
	{
	if(PBin(1)==0)  count++;
	else            count--;
	}
	else
	{
	if(PBin(1)==0)  count--;
	else            count++;
	}		
}



