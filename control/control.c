#include "control.h"
static __IO uint32_t TimingDelay;
static int Vx, Vy, Vz;

void SysTick_Init(void){
	if(SysTick_Config(SystemCoreClock / 10000)) //多少个时钟周期产生一次中断, 这里配置为20ms
	{
		while(1);
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}


//void Delay_us(__IO uint32_t nTime){
//	TimingDelay = nTime;
//	
//	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
//	
//	while(TimingDelay!=0);
//}



int Incremental_PI (int Encoder,int Target, int Pwm, int* Last_bias)
{ 	
	int Bias;
	 //static int Bias,Pwm,Last_bias;
	 Bias = Target- Encoder;                
	 Pwm += KP*(Bias-(*Last_bias))+KI*Bias;   
	 (*Last_bias) = Bias;	                   
	 return Pwm;                         
}


void control(void){
	
	int i;
	int Encoder[4],Pwm[4],Last_bias[4],Target[4];
	while(1){
		TimingDelay = 200;
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		for(i = 0;i < 4; i++){
			Encoder[i] = Read_Encoder(i);
		}
		if(!stop_motor)
			{
				Mecanum_WheelSpeedUpdate(Vx, Vy, Vz, Target);
				for(i = 0;i<4;i++)
				Pwm[i] = Incremental_PI(Encoder[i], Target[i], Pwm[i], Last_bias+i);
				
				Mecanum_SteelSpeedApply( Pwm[0],Pwm[1],Pwm[2],Pwm[3]);
			}
		
		while(TimingDelay!=0);
	}
}

void SysTick_Handler(void)
{
		if(TimingDelay != 0x00){
		TimingDelay--;
	}
}

void USART3_IRQHandler(void){
	static unsigned char Receive[10], length;
	unsigned char ch;
	
	if(USART_GetFlagStatus(USART1,USART_IT_RXNE)!=RESET){
		ch = USART_ReceiveData(USART3);
		
		if(ch == 128){ //0x81
			if((*Receive) == 129){	//0x82
				if(length == 4){
					Vx = Receive[1] - 20;
					Vy = Receive[2] - 20;
					Vz = Receive[3] - 20;
					printf("%d-%d-%d\n", Vx,Vy,Vz);
				}  
				else{
					length = 0;
				}
			}
			length = 0;
		}
		else{
			if(length == 0){
				if (ch == 129) {	//0x82
						*Receive = ch;
						length++;
					}
			}
			  else{
				*(Receive+length) = ch;
				length++;
			}
		}
		//printf("%c\n", ch);
	}
}




