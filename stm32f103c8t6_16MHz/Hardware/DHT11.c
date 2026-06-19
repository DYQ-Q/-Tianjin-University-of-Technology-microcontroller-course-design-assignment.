#include "DHT11.h"
#include "Delay.h"      // 里边有 delay_ms / delay_us

/* DHT11 接在 PA6 */
#define DHT11_GPIO_PORT      GPIOB
#define DHT11_GPIO_PIN       GPIO_Pin_5
#define DHT11_GPIO_CLK       RCC_APB2Periph_GPIOB

#define DHT11_DQ_OUT(x)      GPIO_WriteBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN, (BitAction)(x))
#define DHT11_DQ_IN()        GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN)

/* 配置为输出模式 */
static void DHT11_GPIO_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

/* 配置为输入上拉模式 */
static void DHT11_GPIO_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  DHT11 初始化
  * @retval 0 成功，其它 失败（一般可忽略）
  */
uint8_t DHT11_Init(void)
{
    RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);

    DHT11_GPIO_Output();
    DHT11_DQ_OUT(1);       // 数据线拉高

    delay_ms(1000);        // 上电延时，确保 DHT11 稳定

    /* 可选：读一次丢弃作为唤醒 */
    uint8_t t, h;
    return DHT11_ReadData(&t, &h);
}

/* 读取 1 字节（内部使用） */
static uint8_t DHT11_ReadByte(void)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++)
    {
        uint16_t timeout = 0;

        /* 等待低电平结束（50us 左右） */
        while (DHT11_DQ_IN() == 0)
        {
            if (++timeout > 1000) break;
            delay_us(1);
        }

        /* 等高电平 30us，然后采样 */
        delay_us(30);
        if (DHT11_DQ_IN() == 1)
        {
            data |= (1 << (7 - i));   // 1
        }

        /* 等待本 bit 的高电平结束 */
        timeout = 0;
        while (DHT11_DQ_IN() == 1)
        {
            if (++timeout > 1000) break;
            delay_us(1);
        }
    }

    return data;
}

/**
  * @brief  读取温湿度
  * @param  temp 温度（整数）
  * @param  humi 湿度（整数）
  * @retval 0 成功，其它为错误码
  */
uint8_t DHT11_ReadData(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5] = {0};
    uint16_t timeout = 0;
    uint8_t i;

    /* 主机开始信号：拉低至少 18ms */
    DHT11_GPIO_Output();
    DHT11_DQ_OUT(0);
    delay_ms(20);

    /* 拉高 20~40us */
    DHT11_DQ_OUT(1);
    delay_us(30);

    /* 切换为输入，等待 DHT11 响应 */
    DHT11_GPIO_Input();

    /* 等待 DHT11 拉低响应 */
    timeout = 0;
    while (DHT11_DQ_IN() == 1)
    {
        if (++timeout > 1000) return 1;    // 超时
        delay_us(1);
    }

    /* 等待 DHT11 拉高 */
    timeout = 0;
    while (DHT11_DQ_IN() == 0)
    {
        if (++timeout > 1000) return 2;
        delay_us(1);
    }

    /* 再等待拉低，准备开始传数据 */
    timeout = 0;
    while (DHT11_DQ_IN() == 1)
    {
        if (++timeout > 1000) return 3;
        delay_us(1);
    }

    /* 开始读取 5 字节，40bit 数据 */
    for (i = 0; i < 5; i++)
    {
        buf[i] = DHT11_ReadByte();
    }

    /* 释放总线 */
    DHT11_GPIO_Output();
    DHT11_DQ_OUT(1);

    /* 校验和 */
    if ((uint8_t)(buf[0] + buf[1] + buf[2] + buf[3]) != buf[4])
    {
        return 4;       // 校验错误
    }

    *humi = buf[0];     // 湿度整数
    *temp = buf[2];     // 温度整数

    return 0;
}
