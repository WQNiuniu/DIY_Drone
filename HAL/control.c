/*******************************************************************
 *MPU6050
 *@brief 
 *@brief 
 *@time  2016.1.8
 *@editorС��&zin
 *�ɿذ���QQȺ551883670,����759421287@qq.com
 *����Ȩʹ����Ա����ֹʹ�á���ֹ���ģ�Υ��һ�����֣���Ȩ����
 ******************************************************************/
#include "ALL_DATA.h" 
#include "ALL_DEFINE.h" 
#include "control.h"
#include "pid.h"
//------------------------------------------------------------------------------
#undef NULL
#define NULL 0
#undef DISABLE 
#define DISABLE 0
#undef ENABLE 
#define ENABLE 1
#undef REST
#define REST 0
#undef SET 
#define SET 1 
#undef EMERGENT
#define EMERGENT 0
//------------------------------------------------------------------------------
PidObject *(pPidObject[])={&pidRateX,&pidRateY,&pidRateZ,&pidRoll,&pidPitch,&pidYaw   //�ṹ�����飬��ÿһ�������һ��pid�ṹ�壬�����Ϳ���������������PID��������  �������ʱ������λpid�������ݣ�����������仰�����þͿ�����
};
/**************************************************************
 *  flight control
 * @param[in] 
 * @param[out] 
 * @return     
 ***************************************************************/
void FlightPidControl(float dt)
{
	volatile static uint8_t status=WAITING_1;

	switch(status)
	{		
		case WAITING_1: //�ȴ�����
			if(ALL_flag.unlock)
			{
				status = READY_11;	
			}			
			break;
		case READY_11:  //׼���������
			pidRest(pPidObject,6); //������λPID���ݣ���ֹ�ϴ�����������Ӱ�챾�ο���

			Angle.yaw = pidYaw.desired =  pidYaw.measured = 0;   //����ƫ����
		
			status = PROCESS_31;
		
			break;			
		case PROCESS_31: //��ʽ�������
			if(Angle.pitch<-50||Angle.pitch>50||Angle.roll<-50||Angle.roll>50)//��б��⣬��Ƕ��ж�Ϊ������������������	
					if(Remote.thr>1200)//�����ŵĺܵ�ʱ������б���
						ALL_flag.unlock = EMERGENT;//����������
			
      pidRateX.measured = MPU6050.gyroX * Gyro_G; //�ڻ�����ֵ �Ƕ�/��
			pidRateY.measured = MPU6050.gyroY * Gyro_G;
			pidRateZ.measured = MPU6050.gyroZ * Gyro_G;
		
			pidPitch.measured = Angle.pitch; //�⻷����ֵ ��λ���Ƕ�
		  pidRoll.measured = Angle.roll;
			pidYaw.measured = Angle.yaw;
		
		 	pidUpdate(&pidRoll,dt);    //����PID�������������⻷	�����PID		
			pidRateX.desired = pidRoll.out; //���⻷��PID�����Ϊ�ڻ�PID������ֵ��Ϊ����PID
			pidUpdate(&pidRateX,dt);  //�ٵ����ڻ�

		 	pidUpdate(&pidPitch,dt);    //����PID�������������⻷	������PID	
			pidRateY.desired = pidPitch.out;  
			pidUpdate(&pidRateY,dt); //�ٵ����ڻ�

			CascadePID(&pidRateZ,&pidYaw,dt);	//Ҳ����ֱ�ӵ��ô���PID����������
			break;
		case EXIT_255:  //�˳�����
			pidRest(pPidObject,6);
			status = WAITING_1;//���صȴ�����
		  break;
		default:
			status = EXIT_255;
			break;
	}
	if(ALL_flag.unlock == EMERGENT) //�����������ʹ��ң�ؽ����������ɿؾͿ������κ�����½�����ֹ���У��������������˳�PID����
		status = EXIT_255;
}


#define MOTOR1 motor_PWM_Value[0] 
#define MOTOR2 motor_PWM_Value[1] 
#define MOTOR3 motor_PWM_Value[2] 
#define MOTOR4 motor_PWM_Value[3] 
uint16_t low_thr_cnt_quiet;
uint16_t low_thr_cnt;
void MotorControl(void)
{	
	volatile static uint8_t status=WAITING_1;
	
	
	if(ALL_flag.unlock == EMERGENT) //�����������ʹ��ң�ؽ����������ɿؾͿ������κ�����½�����ֹ���У��������������˳�PID����
		status = EXIT_255;	
	switch(status)
	{		
		case WAITING_1: //�ȴ�����	
			MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;  //������������������Ϊ0
			if(ALL_flag.unlock)
			{
				status = WAITING_2;
			}
		case WAITING_2: //������ɺ��ж�ʹ�����Ƿ�ʼ����ң�˽��з��п���
			if(Remote.thr>1100)
			{
				low_thr_cnt_quiet=0;
				low_thr_cnt=0;
				pidRest(pPidObject,6);
				status = PROCESS_31;
			}
			break;
		case PROCESS_31:
			{
				int16_t temp,thr;
				temp = Remote.thr -1000; //����+�������ֵ
				//���ű����滮
				thr = 100+0.55f * temp;
			
				if(temp<10) //�Զ���ͣ�ж�
				{
					if(low_thr_cnt<1500)
							low_thr_cnt++;
					thr = thr-(low_thr_cnt*0.6);//����ҡ��ֵ������Ϊ0 
					if(MPU6050.accZ<8500&&MPU6050.accZ>7800)
					{
						low_thr_cnt++;
						if(low_thr_cnt>600)//1800ms
						{
							thr = 0;
							
							pidRest(pPidObject,6);
							MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 =0;
							status = WAITING_2;
							
							break;
						}
					}
				}
				else low_thr_cnt=0;
				MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = LIMIT(thr,0,700); //��100����̬����
					
//����������������ȡ���ڵ��PWM�ֲ���ɿ�������ϵ���뿴�ɿ�������ϵͼ�⣬���ĸ����PWM�ֲ��ֲ�	
//           ��ͷ      
//   PWM3     ��       PWM1
//      *           *
//      	*       *
//    		  *   *
//      			*  
//    		  *   *
//      	*       *
//      *           *
//    PWM4           PWM2			
//		pidRateX.out ����Ǵ���PID��� �������ң����Կ���1 2��3 4������������ͬ��ͬ��
//    pidRateY.out �����Ǵ���PID��� ����ǰ�󣬿��Կ���2 3��1 4��ǰ��������ͬ��ͬ��
//		pidRateZ.out ����Ǵ���PID��� ������ת�����Կ���2 4��1 3������Խ��ߵ��ͬ��ͬ��	

// ������ȡ�����㷨��� ������������Ļ�  ��ǰ�ɱ�Ȼ��β�������������,���ҷɱ�Ȼ����������������		

				MOTOR1 +=    + pidRateX.out + pidRateY.out + pidRateZ.out;//; ��̬����������������Ŀ�����
				MOTOR2 +=    + pidRateX.out - pidRateY.out - pidRateZ.out ;//;
				MOTOR3 +=    - pidRateX.out + pidRateY.out - pidRateZ.out;
				MOTOR4 +=    - pidRateX.out - pidRateY.out + pidRateZ.out;//;
			}	
			break;
		case EXIT_255:
			MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;  //������������������Ϊ0
			status = WAITING_1;	
			break;
		default:
			break;
	}
	
	
	TIM2->CCR1 = LIMIT(MOTOR1,0,1000);  //����PWM
	TIM2->CCR2 = LIMIT(MOTOR2,0,1000);
	TIM2->CCR3 = LIMIT(MOTOR3,0,1000);
	TIM2->CCR4 = LIMIT(MOTOR4,0,1000);
} 

/************************************END OF FILE********************************************/ 
