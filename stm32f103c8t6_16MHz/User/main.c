/*
 *    _______      _   _    _   _______ 
 *   |__   __|    | | | |  | | |__   __|
 *      | |       | | | |  | |    | |   
 *      | |   _   | | | |  | |    | |   
 *      | |  | |__| | | |__| |    | |   
 *      |_|   \____/   \____/     |_|   
 *                                      
 *     _____   _______   __  __   ____    ___    ______   __    ___    ____     _____    ___    _______     __  
 *    / ____| |__   __| |  \/  | |___ \  |__ \  |  ____| /_ |  / _ \  |___ \   / ____|  / _ \  |__   __|   / /  
 *   | (___      | |    | \  / |   __) |    ) | | |__     | | | | | |   __) | | |      | (_) |    | |     / /_  
 *    \___ \     | |    | |\/| |  |__ <    / /  |  __|    | | | | | |  |__ <  | |       > _ <     | |    | '_ \ 
 *    ____) |    | |    | |  | |  ___) |  / /_  | |       | | | |_| |  ___) | | |____  | (_) |    | |    | (_) |
 *   |_____/     |_|    |_|  |_| |____/  |____| |_|       |_|  \___/  |____/   \_____|  \___/     |_|     \___/                                                                                                     
 */
#include "stm32f10x.h" // Device header
#include <stdio.h>
#include <string.h>

#include "delay.h"     //延时函数
#include "Key.h"       //按键
#include "OLED.h"      //OLED屏
#include "Motor.h"     //电机
#include "Serial.h"    //串口:串口屏、USB转TTL、ESP8266
#include "MPU6050.h"   //MPU6050陀螺仪
#include "Buzzer.h"	   //蜂鸣器
#include "DHT11.h"     //温湿度传感器
#include "HMI_TJC.h"     //陶晶驰串口屏
#include "timer.h"
#include "ESP8266.h"

#define shut_angle 45//关机角度
/*
------------------------------------------------------------------
| 设备模块         | 引脚配置                  | 功能说明         |
|------------------|--------------------------|-----------------|
| 串口屏           | PA9(TX1), PA10(RX1)      | 串口通信         |
| USB转TTL         | PA2, PA3                 | USB通信转换      |
| ESP8266          | PB10, PB11               | WiFi通信模块     |
| 按键(4个)        | PB12, PB13, PB14, PB15   | 输入控制         |
| 电机             | PA0, PA1, PB0(PWM)       | 电机驱动控制     |
| MPU6050陀螺仪    | SCL:PB6, SDA:PB7         | 姿态检测         |
| OLED屏           | SDA:PB9, SCL:PB8         | 显示输出         |
| 蜂鸣器           | PB1                      | 声音提示         |
| 温湿度传感器      | PB5                     | 环境监测          |
------------------------------------------------------------------
*/

void key_num_process(void);//按键处理函数
void TJC_RXdata_process(void);//串口屏数据接收处理函数
void motor_speed_limit(void);//电机速度限制函数
void smart_shutdown(void);//智能关机函数

//全局变量
uint8_t temp=0,humi=0; // 温湿度
int8_t motor_speed=0; // 电机速度
uint8_t key_num=0;    // 按键键值
uint8_t motor_dir=0; // 电机方向
int pwm[255]; // pwm占空比
MPU6050_Data_t mpu_data; // MPU6050数据

int main(void)
{
	TJC_Init();//串口屏，引脚：PA9(TX1),PA10(RX1)
	Serial_Init(&Serial2);//USB转TTL，引脚：PA2,PA3
	ESP8266_Init();//ESP8266，引脚：PB10,PB11
	Key_Init();//按键(4个)，引脚：PB12,PB13,PB14,PB15
	Motor_Init();//电机,引脚：PA0,PA1,PB0(PWM)
	MPU6050_Init();//MPU6050陀螺仪,引脚：SCL:PB6,SDA:PB7
	int16_t gyro_offset_x, gyro_offset_y, gyro_offset_z;
	MPU6050_CalibrateGyro(&gyro_offset_x, &gyro_offset_y, &gyro_offset_z, 200);
	//MPU6050_Data_t mpu_data;
	OLED_Init();//OLED屏,引脚：SDA:PB9,SCL:PB8
	Buzzer_Init();//蜂鸣器,引脚：PB1
	DHT11_Init();//温湿度传感器,引脚：PB5

	OLED_ShowString(1, 1, "Hello C8T6!");
	OLED_ShowString(4, 1, "Over Limit:");

	Buzzer_Beep(100);
	delay_ms(500);
	Buzzer_Beep(100);//响两次作为开机提示音
	
	while(1)
	{
		
		if(DHT11_ReadData(&temp,&humi)==0)
		{
			TJC_ShowNumber("first.n0", temp);
			TJC_ShowNumber("first.n1", humi);
			OLED_ShowString(2, 1, "Read DHT11 OK!");
		}
		MPU6050_ReadProcessedData(&mpu_data);
		MPU6050_CalculateAngles(&mpu_data);
		
		TJC_ShowNumber("first.n3", mpu_data.temp_c);
		TJC_ShowFloat("first.x0", mpu_data.pitch);
		TJC_ShowFloat("first.x1", mpu_data.roll);
		TJC_ShowFloat("first.x2", mpu_data.yaw);
		OLED_ShowString(3, 1, "Read MPU6050 OK!");

		key_num=Key_GetNum();//获取按键键值
		key_num_process();
		TJC_RXdata_process();
		if(motor_dir)
		{
			TJC_ShowText("first.t5", "anticlockwise");//逆时针
		}
		else
		{
			TJC_ShowText("first.t5", "clockwise");//顺时针
		}

		motor_speed_limit();//电机速度限制函数
		
		smart_shutdown();//智能关机函数
		if(mpu_data.pitch<-shut_angle||mpu_data.pitch>shut_angle||mpu_data.roll<-shut_angle||mpu_data.roll>shut_angle||mpu_data.yaw<-shut_angle||mpu_data.yaw>shut_angle)
		{
			OLED_ShowString(4, 12, "YES");
		}
		else
		{
		    OLED_ShowString(4, 12, " NO");
		}
		
		Motor_SetSpeed(motor_speed);
		
		TJC_ShowNumber("motor.n0", motor_speed);
		TJC_ShowNumber("first.n2", motor_speed);
	}
}


// void TIM2_IRQHandler(void)//每1ms进一次TIM2的中断服务函数
// {
// 	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
// 	{
// 		key_num=Key_GetNum();//获取按键键值
// 		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
// 	}
// }

void key_num_process(void)
{
    if(key_num==1)
		{
		    motor_speed=-motor_speed;
			motor_dir=!motor_dir;
		}
	if(key_num==2)
		{
		    motor_speed+=20;
		}
	if(key_num==3)
		{
		    motor_speed-=20;
		}
	if(key_num==4)
		{
		    motor_speed=0;	   
		}
}

void TJC_RXdata_process(void)
{
	if(Serial1.RxData=='1')
	{
	    motor_speed=-motor_speed;
		motor_dir=!motor_dir;
		Serial1.RxData='0';
	}
	if(Serial1.RxData=='2')
	{
	    motor_speed+=20;
		Serial1.RxData='0';
	}
	if(Serial1.RxData=='3')
	{
	    motor_speed-=20;
		Serial1.RxData='0';
	}
	if(Serial1.RxData=='4')
	{
	    motor_speed=0;
		Serial1.RxData='0';
	}
	// if(Serial1.RxData=='5')
	// {
	// 	for(int i=0;i<motor_speed;i++)
	// 	{
	// 	    pwm[i]=1;
	// 	}
	// 	for(int i=motor_speed;i<255;i++)
	// 	{
	// 	    pwm[i]=0;
	// 	}
	// 	for(int i=0;i<255;i++)
	// 	{
	// 	    HMI_Wave_Clear("pwm.s0", pwm[i])
	// 	}
	// }
	// if(Serial1.RxData=='6')
	// {
	//     TJC_ClearWave("pwm.s0",0);
	// }
}

void motor_speed_limit(void)
{
	if(motor_speed>=100)
	{
		motor_speed=100;
	}
	if(motor_speed<=-100)
	{
		motor_speed=-100;
	}
}

void smart_shutdown(void)
{
    if(mpu_data.pitch<-shut_angle||mpu_data.pitch>shut_angle||mpu_data.roll<-shut_angle||mpu_data.roll>shut_angle||mpu_data.yaw<-shut_angle||mpu_data.yaw>shut_angle)
	{
		motor_speed=0;
		Buzzer_Beep(100);//响一声作为关机提示音
		OLED_ShowString(4, 12, "YES");
	}
}
