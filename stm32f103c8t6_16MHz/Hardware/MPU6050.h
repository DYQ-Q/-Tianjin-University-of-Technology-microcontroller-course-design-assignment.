#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"

// MPU6050 I2C地址
#define MPU6050_ADDRESS     0xD0    // AD0连接到GND，地址为0xD0

// MPU6050寄存器定义
#define MPU6050_SELF_TEST_X     0x0D    // X轴自测试寄存器
#define MPU6050_SELF_TEST_Y     0x0E    // Y轴自测试寄存器
#define MPU6050_SELF_TEST_Z     0x0F    // Z轴自测试寄存器
#define MPU6050_SELF_TEST_A     0x10    // 加速度计自测试寄存器
#define MPU6050_SMPLRT_DIV      0x19    // 采样率分频器
#define MPU6050_CONFIG          0x1A    // 配置寄存器
#define MPU6050_GYRO_CONFIG     0x1B    // 陀螺仪配置寄存器
#define MPU6050_ACCEL_CONFIG    0x1C    // 加速度计配置寄存器
#define MPU6050_FIFO_EN         0x23    // FIFO使能寄存器
#define MPU6050_I2C_MST_CTRL    0x24    // I2C主控制器
#define MPU6050_INT_PIN_CFG     0x37    // 中断引脚配置
#define MPU6050_INT_ENABLE      0x38    // 中断使能寄存器
#define MPU6050_INT_STATUS      0x3A    // 中断状态寄存器
#define MPU6050_ACCEL_XOUT_H    0x3B    // 加速度计X轴高字节
#define MPU6050_ACCEL_XOUT_L    0x3C    // 加速度计X轴低字节
#define MPU6050_ACCEL_YOUT_H    0x3D    // 加速度计Y轴高字节
#define MPU6050_ACCEL_YOUT_L    0x3E    // 加速度计Y轴低字节
#define MPU6050_ACCEL_ZOUT_H    0x3F    // 加速度计Z轴高字节
#define MPU6050_ACCEL_ZOUT_L    0x40    // 加速度计Z轴低字节
#define MPU6050_TEMP_OUT_H      0x41    // 温度高字节
#define MPU6050_TEMP_OUT_L      0x42    // 温度低字节
#define MPU6050_GYRO_XOUT_H     0x43    // 陀螺仪X轴高字节
#define MPU6050_GYRO_XOUT_L     0x44    // 陀螺仪X轴低字节
#define MPU6050_GYRO_YOUT_H     0x45    // 陀螺仪Y轴高字节
#define MPU6050_GYRO_YOUT_L     0x46    // 陀螺仪Y轴低字节
#define MPU6050_GYRO_ZOUT_H     0x47    // 陀螺仪Z轴高字节
#define MPU6050_GYRO_ZOUT_L     0x48    // 陀螺仪Z轴低字节
#define MPU6050_USER_CTRL       0x6A    // 用户控制寄存器
#define MPU6050_PWR_MGMT_1      0x6B    // 电源管理寄存器1
#define MPU6050_PWR_MGMT_2      0x6C    // 电源管理寄存器2
#define MPU6050_FIFO_COUNTH     0x72    // FIFO计数高字节
#define MPU6050_FIFO_COUNTL     0x73    // FIFO计数低字节
#define MPU6050_FIFO_R_W        0x74    // FIFO读写寄存器
#define MPU6050_WHO_AM_I        0x75    // 设备ID寄存器

// 陀螺仪量程选择
typedef enum {
    GYRO_RANGE_250 = 0,   // ±250°/s
    GYRO_RANGE_500,       // ±500°/s
    GYRO_RANGE_1000,      // ±1000°/s
    GYRO_RANGE_2000       // ±2000°/s
} GYRO_Range_t;

// 加速度计量程选择
typedef enum {
    ACCEL_RANGE_2G = 0,   // ±2g
    ACCEL_RANGE_4G,       // ±4g
    ACCEL_RANGE_8G,       // ±8g
    ACCEL_RANGE_16G       // ±16g
} ACCEL_Range_t;

// 数字低通滤波器配置
typedef enum {
    DLPF_260 = 0,         // 260Hz
    DLPF_184,             // 184Hz
    DLPF_94,              // 94Hz
    DLPF_44,              // 44Hz
    DLPF_21,              // 21Hz
    DLPF_10,              // 10Hz
    DLPF_5                // 5Hz
} DLPF_Bandwidth_t;

// MPU6050数据结构
typedef struct {
    int16_t accel_x;      // 加速度计X轴原始数据
    int16_t accel_y;      // 加速度计Y轴原始数据
    int16_t accel_z;      // 加速度计Z轴原始数据
    int16_t gyro_x;       // 陀螺仪X轴原始数据
    int16_t gyro_y;       // 陀螺仪Y轴原始数据
    int16_t gyro_z;       // 陀螺仪Z轴原始数据
    int16_t temp;         // 温度原始数据

    float accel_x_g;      // 加速度计X轴(g)
    float accel_y_g;      // 加速度计Y轴(g)
    float accel_z_g;      // 加速度计Z轴(g)
    float gyro_x_dps;     // 陀螺仪X轴(°/s)
    float gyro_y_dps;     // 陀螺仪Y轴(°/s)
    float gyro_z_dps;     // 陀螺仪Z轴(°/s)
    float temp_c;         // 温度(°C)

    // 姿态角度
    float pitch;          // 俯仰角(°)
    float roll;           // 横滚角(°)
    float yaw;            // 偏航角(°)
} MPU6050_Data_t;

// 函数声明
void MPU6050_Init(void);
uint8_t MPU6050_ReadID(void);
void MPU6050_WriteReg(uint8_t reg, uint8_t data);
uint8_t MPU6050_ReadReg(uint8_t reg);
void MPU6050_ReadRegs(uint8_t reg, uint8_t len, uint8_t *data);
void MPU6050_SetGyroRange(GYRO_Range_t range);
void MPU6050_SetAccelRange(ACCEL_Range_t range);
void MPU6050_SetDLPF(DLPF_Bandwidth_t bandwidth);
void MPU6050_SetSampleRate(uint16_t rate);
void MPU6050_ReadRawData(MPU6050_Data_t *data);
void MPU6050_ReadProcessedData(MPU6050_Data_t *data);
void MPU6050_CalibrateGyro(int16_t *offset_x, int16_t *offset_y, int16_t *offset_z, uint16_t samples);

// 姿态角度计算函数
void MPU6050_CalculateAngles(MPU6050_Data_t *data);
void MPU6050_CalculateAnglesWithComplementaryFilter(MPU6050_Data_t *data, float dt, float filter_alpha);

#endif /* __MPU6050_H */
