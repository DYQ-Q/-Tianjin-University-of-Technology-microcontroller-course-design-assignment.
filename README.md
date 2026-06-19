# -Tianjin-University-of-Technology-microcontroller-course-design-assignment.
基于STM32F103C8T6的一款智能风扇控制系统，外设有：DHT11温湿度传感器，MPU6050陀螺仪，蜂鸣器，结合陶晶驰触摸屏实现触屏控制，同时还有OLED显示屏显示一些info，PCB上画了ESP8266的接口，但是功能来不及加了（当时课设时间有点紧）/A smart fan control system based on the STM32F103C8T6 microcontroller, with the following peripherals: DHT11 temperature and humidity sensor, MPU6050 gyroscope, and a buzzer. It incorporates a Tao Jingchi touchscreen for touch control, and also features an OLED display to show some information. An ESP8266 interface was designed on the PCB, but the functionality was not implemented in time due to the tight schedule for the course project
# 接线表

|设备模块|引脚配置|功能说明|
|:---:|:---:|:---:|
|串口屏|PA9(TX1),PA10(RX1)|串口通信|
| USB转TTL| PA2, PA3| USB通信转换|
| ESP8266| PB10, PB11| WiFi通信模块|
| 按键(4个)| PB12, PB13, PB14, PB15| 输入控制|
| 电机| PA0, PA1, PB0(PWM)|电机驱动控制|
| MPU6050陀螺仪| SCL:PB6, SDA:PB7| 姿态检测|
| OLED屏| SDA:PB9, SCL:PB8|显示输出|
| 蜂鸣器| PB1| 声音提示|
| 温湿度传感器| PB5| 环境监测|
