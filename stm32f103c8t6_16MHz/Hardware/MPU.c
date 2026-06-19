#include "MPU6050.h"
#include "delay.h"

// 私有变量
static GYRO_Range_t gyro_range = GYRO_RANGE_250;
static ACCEL_Range_t accel_range = ACCEL_RANGE_2G;

// 私有函数声明
static void MPU6050_I2C_Init(void);
static void MPU6050_I2C_Write(uint8_t devAddr, uint8_t regAddr, uint8_t data);
static void MPU6050_I2C_Read(uint8_t devAddr, uint8_t regAddr, uint8_t len, uint8_t *data);

/**
 * @brief  初始化MPU6050
 * @param  无
 * @retval 无
 */
void MPU6050_Init(void)
{
    uint8_t try_count = 0;
    uint8_t device_id = 0;

    // 初始化I2C
    MPU6050_I2C_Init();
    // 延时等待MPU6050上电稳定
    delay_ms(100);

    // 尝试读取设备ID，最多尝试3次
    for(try_count = 0; try_count < 3; try_count++) {
        device_id = MPU6050_ReadID();
        if(device_id == 0x68) {
            break; // 成功读取到正确的设备ID
        }
        delay_ms(50); // 等待一段时间后重试
    }

    // 如果无法读取正确的设备ID，可能是硬件连接问题或总线卡死
    if(device_id != 0x68) {
        return; // 初始化失败，直接返回
    }

    // 解除睡眠模式，选择PLL作为时钟源
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
    delay_ms(10); // 等待配置生效

    // 设置陀螺仪量程为±250°/s
    MPU6050_SetGyroRange(GYRO_RANGE_250);
    delay_ms(10); // 等待配置生效

    // 设置加速度计量程为±2g
    MPU6050_SetAccelRange(ACCEL_RANGE_2G);
    delay_ms(10); // 等待配置生效

    // 设置数字低通滤波器为20Hz
    MPU6050_SetDLPF(DLPF_21);
    delay_ms(10); // 等待配置生效

    // 设置采样率为50Hz
    MPU6050_SetSampleRate(50);
    delay_ms(10); // 等待配置生效

    // 延时等待所有配置生效
    delay_ms(100);
}

/**
 * @brief  初始化I2C1
 * @param  无
 * @retval 无
 */
static void MPU6050_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    uint16_t timeout = 0;

    // 使能GPIOB和I2C1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // I2C总线复位 - 如果SCL或SDA被拉低，尝试释放
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // 开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 尝试释放总线
    GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);  // 设置为高电平
    for(timeout = 0; timeout < 10; timeout++) {
        // 切换SCL至少9次以释放总线
        GPIO_ResetBits(GPIOB, GPIO_Pin_6);
        delay_ms(1);
        GPIO_SetBits(GPIOB, GPIO_Pin_6);
        delay_ms(1);
    }

    // 配置I2C1引脚：PB6(SCL), PB7(SDA)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;  // 复用开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // 配置I2C1
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;               // I2C模式
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;       // 占空比2:1
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;                // 主机地址
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;              // 使能应答
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7位地址
    I2C_InitStructure.I2C_ClockSpeed = 400000;               // 400kHz
    I2C_Init(I2C1, &I2C_InitStructure);
    // 使能I2C1
    I2C_Cmd(I2C1, ENABLE);
}

/**
 * @brief  向MPU6050写入一个字节数据
 * @param  devAddr: 设备地址
 * @param  regAddr: 寄存器地址
 * @param  data: 要写入的数据
 * @retval 无
 */
static void MPU6050_I2C_Write(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
    uint16_t timeout = 0;
    const uint16_t I2C_TIMEOUT = 1000;

    // 等待总线空闲
    timeout = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && timeout) timeout--;

    // 发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) && timeout) timeout--;

    // 如果超时，发送停止条件并返回
    if(timeout == 0) {
        I2C_GenerateSTOP(I2C1, ENABLE);
        return;
    }

    // 发送设备地址（写模式）
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && timeout) timeout--;

    // 如果超时，发送停止条件并返回
    if(timeout == 0) {
        I2C_GenerateSTOP(I2C1, ENABLE);
        return;
    }

    // 发送寄存器地址
    I2C_SendData(I2C1, regAddr);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && timeout) timeout--;

    // 如果超时，发送停止条件并返回
    if(timeout == 0) {
        I2C_GenerateSTOP(I2C1, ENABLE);
        return;
    }

    // 发送数据
    I2C_SendData(I2C1, data);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && timeout) timeout--;

    // 发送停止条件
    I2C_GenerateSTOP(I2C1, ENABLE);
}

/**
 * @brief  从MPU6050读取多个字节数据
 * @param  devAddr: 设备地址
 * @param  regAddr: 寄存器地址
 * @param  len: 要读取的字节数
 * @param  data: 存储读取数据的缓冲区
 * @retval 无
 */
static void MPU6050_I2C_Read(uint8_t devAddr, uint8_t regAddr, uint8_t len, uint8_t *data)
{
    uint8_t i;
    uint16_t timeout = 0;
    const uint16_t I2C_TIMEOUT = 1000;

    // 等待总线空闲
    timeout = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && timeout) timeout--;

    // 发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) && timeout) timeout--;

    // 如果超时，发送停止条件并返回
    if(timeout == 0) {
        I2C_GenerateSTOP(I2C1, ENABLE);
        return;
    }

    // 发送设备地址（写模式）
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && timeout) timeout--;

    // 如果超时，发送停止条件并返回
    if(timeout == 0) {
        I2C_GenerateSTOP(I2C1, ENABLE);
        return;
    }

    // 发送寄存器地址
    I2C_SendData(I2C1, regAddr);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && timeout) timeout--;

    // 如果超时，发送停止条件并返回
    if(timeout == 0) {
        I2C_GenerateSTOP(I2C1, ENABLE);
        return;
    }

    // 发送重复起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) && timeout) timeout--;

    // 如果超时，发送停止条件并返回
    if(timeout == 0) {
        I2C_GenerateSTOP(I2C1, ENABLE);
        return;
    }

    // 发送设备地址（读模式）
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Receiver);
    timeout = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && timeout) timeout--;

    // 如果超时，发送停止条件并返回
    if(timeout == 0) {
        I2C_GenerateSTOP(I2C1, ENABLE);
        I2C_AcknowledgeConfig(I2C1, ENABLE); // 确保重新启用应答
        return;
    }

    // 读取数据
    for(i = 0; i < len; i++)
    {
        if(i == len - 1)  // 最后一个字节
        {
            // 禁用应答
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            // 发送停止条件
            I2C_GenerateSTOP(I2C1, ENABLE);
        }

        // 等待接收到数据
        timeout = I2C_TIMEOUT;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) && timeout) timeout--;

        // 如果超时，发送停止条件并返回
        if(timeout == 0) {
            I2C_GenerateSTOP(I2C1, ENABLE);
            I2C_AcknowledgeConfig(I2C1, ENABLE); // 确保重新启用应答
            return;
        }

        // 读取数据
        data[i] = I2C_ReceiveData(I2C1);
    }

    // 重新启用应答
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}

/**
 * @brief  读取MPU6050设备ID
 * @param  无
 * @retval 设备ID (0x68)
 */
uint8_t MPU6050_ReadID(void)
{
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

/**
 * @brief  向MPU6050写入寄存器
 * @param  reg: 寄存器地址
 * @param  data: 要写入的数据
 * @retval 无
 */
void MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    MPU6050_I2C_Write(MPU6050_ADDRESS, reg, data);
}

/**
 * @brief  从MPU6050读取寄存器
 * @param  reg: 寄存器地址
 * @retval 读取到的数据
 */
uint8_t MPU6050_ReadReg(uint8_t reg)
{
    uint8_t data;
    MPU6050_I2C_Read(MPU6050_ADDRESS, reg, 1, &data);
    return data;
}

/**
 * @brief  从MPU6050读取多个寄存器
 * @param  reg: 起始寄存器地址
 * @param  len: 要读取的字节数
 * @param  data: 存储读取数据的缓冲区
 * @retval 无
 */
void MPU6050_ReadRegs(uint8_t reg, uint8_t len, uint8_t *data)
{
    MPU6050_I2C_Read(MPU6050_ADDRESS, reg, len, data);
}

/**
 * @brief  设置陀螺仪量程
 * @param  range: 陀螺仪量程
 * @retval 无
 */
void MPU6050_SetGyroRange(GYRO_Range_t range)
{
    uint8_t config;

    gyro_range = range;

    // 读取当前配置
    config = MPU6050_ReadReg(MPU6050_GYRO_CONFIG);

    // 清除量程位
    config &= ~(0x03 << 3);

    // 设置新的量程
    config |= (range << 3);

    // 写入新配置
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, config);
}

/**
 * @brief  设置加速度计量程
 * @param  range: 加速度计量程
 * @retval 无
 */
void MPU6050_SetAccelRange(ACCEL_Range_t range)
{
    uint8_t config;

    accel_range = range;

    // 读取当前配置
    config = MPU6050_ReadReg(MPU6050_ACCEL_CONFIG);

    // 清除量程位
    config &= ~(0x03 << 3);

    // 设置新的量程
    config |= (range << 3);

    // 写入新配置
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, config);
}

/**
 * @brief  设置数字低通滤波器带宽
 * @param  bandwidth: DLPF带宽
 * @retval 无
 */
void MPU6050_SetDLPF(DLPF_Bandwidth_t bandwidth)
{
    uint8_t config;

    // 读取当前配置
    config = MPU6050_ReadReg(MPU6050_CONFIG);

    // 清除DLPF位
    config &= 0xF8;

    // 设置新的DLPF
    config |= bandwidth;

    // 写入新配置
    MPU6050_WriteReg(MPU6050_CONFIG, config);
}

/**
 * @brief  设置采样率
 * @param  rate: 采样率(Hz)
 * @retval 无
 */
void MPU6050_SetSampleRate(uint16_t rate)
{
    uint8_t div;

    // 计算分频值，采样率 = 1000 / (1 + div)
    if(rate > 1000) rate = 1000;
    div = 1000 / rate - 1;

    // 写入分频值
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, div);
}

/**
 * @brief  读取MPU6050原始数据
 * @param  data: 存储读取数据的结构体指针
 * @retval 无
 */
void MPU6050_ReadRawData(MPU6050_Data_t *data)
{
    uint8_t buffer[14];

    // 从ACCEL_XOUT_H开始连续读取14个字节
    MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H, 14, buffer);

    // 组合加速度计数据
    data->accel_x = (int16_t)((buffer[0] << 8) | buffer[1]);
    data->accel_y = (int16_t)((buffer[2] << 8) | buffer[3]);
    data->accel_z = (int16_t)((buffer[4] << 8) | buffer[5]);

    // 组合温度数据
    data->temp = (int16_t)((buffer[6] << 8) | buffer[7]);

    // 组合陀螺仪数据
    data->gyro_x = (int16_t)((buffer[8] << 8) | buffer[9]);
    data->gyro_y = (int16_t)((buffer[10] << 8) | buffer[11]);
    data->gyro_z = (int16_t)((buffer[12] << 8) | buffer[13]);
}

/**
 * @brief  读取MPU6050处理后的数据
 * @param  data: 存储读取数据的结构体指针
 * @retval 无
 */
void MPU6050_ReadProcessedData(MPU6050_Data_t *data)
{
    // 读取原始数据
    MPU6050_ReadRawData(data);

    // 转换加速度计数据为g
    float accel_scale;
    switch(accel_range)
    {
        case ACCEL_RANGE_2G:  accel_scale = 16384.0f; break;
        case ACCEL_RANGE_4G:  accel_scale = 8192.0f; break;
        case ACCEL_RANGE_8G:  accel_scale = 4096.0f; break;
        case ACCEL_RANGE_16G: accel_scale = 2048.0f; break;
        default:               accel_scale = 16384.0f; break;
    }

    data->accel_x_g = (float)data->accel_x / accel_scale;
    data->accel_y_g = (float)data->accel_y / accel_scale;
    data->accel_z_g = (float)data->accel_z / accel_scale;

    // 转换陀螺仪数据为°/s
    float gyro_scale;
    switch(gyro_range)
    {
        case GYRO_RANGE_250:  gyro_scale = 131.0f; break;
        case GYRO_RANGE_500:  gyro_scale = 65.5f; break;
        case GYRO_RANGE_1000: gyro_scale = 32.8f; break;
        case GYRO_RANGE_2000: gyro_scale = 16.4f; break;
        default:               gyro_scale = 131.0f; break;
    }

    data->gyro_x_dps = (float)data->gyro_x / gyro_scale;
    data->gyro_y_dps = (float)data->gyro_y / gyro_scale;
    data->gyro_z_dps = (float)data->gyro_z / gyro_scale;

    // 转换温度数据为°C
    data->temp_c = (float)data->temp / 340.0f + 36.53f;
}

/**
 * @brief  校准陀螺仪零偏
 * @param  offset_x: X轴零偏指针
 * @param  offset_y: Y轴零偏指针
 * @param  offset_z: Z轴零偏指针
 * @param  samples: 采样次数
 * @retval 无
 */
void MPU6050_CalibrateGyro(int16_t *offset_x, int16_t *offset_y, int16_t *offset_z, uint16_t samples)
{
    MPU6050_Data_t data;
    int32_t sum_x = 0, sum_y = 0, sum_z = 0;
    uint16_t i;

    // 采集多组数据并求和
    for(i = 0; i < samples; i++)
    {
        MPU6050_ReadRawData(&data);
        sum_x += data.gyro_x;
        sum_y += data.gyro_y;
        sum_z += data.gyro_z;
        delay_ms(10);  // 适当延时
    }

    // 计算平均值作为零偏
    *offset_x = sum_x / samples;
    *offset_y = sum_y / samples;
    *offset_z = sum_z / samples;
}
