#include "encoder.h"

static int count;

//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 		   
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}


//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;//取低四位  
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//使能中断位(要清除的话,相反操作就OK) 
	NVIC->IP[NVIC_Channel]|=temp<<4;//设置响应优先级和抢断优先级   	    	  				   
} 


//外部中断配置函数
//只针对GPIOA~G;不包括PVD,RTC和USB唤醒这三个
//参数:
//GPIOx:0~6,代表GPIOA~G
//BITx:需要使能的位;
//TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线   	    
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//得到中断寄存器组的编号
	EXTOFFSET=(BITx%4)*4; 
	RCC->APB2ENR|=0x01;//使能io复用时钟			 
	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITx映射到GPIOx.BITx 
	//自动设置
	EXTI->IMR|=1<<BITx;//  开启line BITx上的中断
	//EXTI->EMR|=1<<BITx;//不屏蔽line BITx上的事件 (如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
 	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx上事件下降沿触发
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx上事件上升降沿触发
} 	  
//不能在这里执行所有外设复位!否则至少引起串口不工作.		    
//把所有时钟寄存器复位		 


//函数功能：外部中断采集编码器
//入口参数：无
//返回  值：无
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
	RCC->APB2ENR|=1<<0;     	//使能AFIO时钟	
	RCC->APB2ENR|=1<<3;    //使能PORTB时钟	   	
	GPIOB->CRL&=0XFFFFFF00; 
	GPIOB->CRL|=0X00000088;//输入	
	Ex_NVIC_Config(1,0,3);		//跳变沿触发
	Ex_NVIC_Config(1,1,3);		//跳变沿触发
	MY_NVIC_Init(1,1,EXTI0_IRQn,2);  	//组2
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
		EXTI->PR=1<<1;  //清除LINE上的中断标志位
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
		EXTI->PR=1<<0;  //清除LINE上的中断标志位
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



