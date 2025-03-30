#include "ALL_DEFINE.h"
#include "scheduler.h"



void TIM1_UP_IRQHandler(void)   //TIM3�ж� 3ms
{
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		MpuGetData();
		GetAngle(&MPU6050,&Angle,0.003f);
		RC_Analy();			
		FlightPidControl(0.003f);
		MotorControl();
	
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
	}
}


