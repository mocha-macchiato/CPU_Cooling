#include "ds18b20.h"
#include "main.h"

#define SET_DS18B20_MODE_IN() {DQ_GPIO_Port->CRL &= 0xFFFFFFF0;}
#define SET_DS18B20_MODE_OUT() {DQ_GPIO_Port->CRL &= 0xFFFFFFF0; DQ_GPIO_Port->CRL |= 0x01;}


/**
  * @brief  This function provides delay (in microseconds) based on CPU cycles method.
  * @param  udelay: specifies the delay time length, in microseconds.
  * @retval None
  */
static void Micro_Delay(uint32_t udelay)
{
    __IO uint32_t Delay = udelay * (SystemCoreClock / 8U / 1000000U);
    do
    {
        __NOP();
    }
    while (Delay --);
}


/**
  * @brief  This function write a byte size of data to DS18B20.
  * @param  data: A byte size of date.
  * @retval None
  */
static void Ds18b20_Write_Byte(uint8_t data)
{
    uint8_t bit_data = 0;
    SET_DS18B20_MODE_OUT();
    for (uint8_t i = 0; i < 8; ++i) {
        bit_data = data & 0x01;
        data >>= 1;
        if (bit_data){
            HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_RESET);
            Micro_Delay(2);
            HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);
            Micro_Delay(60);
        } else {
            HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_RESET);
            Micro_Delay(60);
            HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);
            Micro_Delay(2);
        }
    }
}


/**
  * @brief  This function read a bit size of data from DS18B20.
  * @param  None
  * @return bit_data: A bit size of data read from DS18B20.
  * @retval None
  */
static uint8_t Ds18b20_Read_bit(void)
{
    uint8_t bit_data = 0;
    SET_DS18B20_MODE_OUT();
    HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_RESET);
    Micro_Delay(2);
    HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);
    SET_DS18B20_MODE_IN();
    Micro_Delay(12);
    bit_data = HAL_GPIO_ReadPin(DQ_GPIO_Port, DQ_Pin);
    Micro_Delay(50);
    return bit_data;
}


/**
  * @brief  This function read a byte size of data from DS18B20.
  * @param  None
  * @return byte_data: A byte size of data read from DS18B20.
  * @retval None
  */
static uint8_t Ds18b20_Read_Byte(void)
{
    uint8_t bit_data = 0, byte_data = 0;
    SET_DS18B20_MODE_IN();
    for (uint8_t i = 0; i < 8; ++i) {
        bit_data = Ds18b20_Read_bit();
        byte_data = byte_data >> 1 | bit_data << 7;
    }
    return byte_data;
}


/**
  * @brief  This function wait DS18B20 response.
  * @param  None
  * @return 0: Normal;
  *         1: Abnormal.
  * @retval None
  */
static uint8_t Ds18b20_Check(void)
{
    uint8_t retry_times = 0;
    SET_DS18B20_MODE_IN();
    while (HAL_GPIO_ReadPin(DQ_GPIO_Port, DQ_Pin) && retry_times < 200){
        retry_times ++;
        Micro_Delay(1);
    }
    if (retry_times >= 200) return 1;
    else retry_times = 0;

    while (!HAL_GPIO_ReadPin(DQ_GPIO_Port, DQ_Pin) && retry_times < 240){
        retry_times ++;
        Micro_Delay(1);
    }
    if (retry_times >= 240) return 1;
    else return 0;
}


/**
  * @brief  This function initialize DS18B20.
  * @param  None
  * @return TODO: The temperature gotten from DS18B20.
  * @retval None
  */
static uint8_t Ds18b20_Init(void)
{
    uint8_t response = 0;

    SET_DS18B20_MODE_OUT();
    HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);
    Micro_Delay(2);
    HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_RESET);
    Micro_Delay(500);
    HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);

    Micro_Delay(60);
    SET_DS18B20_MODE_IN();
    response = HAL_GPIO_ReadPin(DQ_GPIO_Port, DQ_Pin);

    Micro_Delay(500);
    SET_DS18B20_MODE_OUT();
    HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);

    return response;
}


/**
  * @brief  This function get temperature from DS18B20.
  * @param  None
  * @return temp: The temperature gotten from DS18B20.
  * @retval None
  */
uint16_t Ds18b20_Get_Temp(void)
{
    uint8_t TH = 0, TL = 0;
    uint16_t temp = 0;

    if(Ds18b20_Init()) return 0xFFFF;
    Ds18b20_Write_Byte(0xcc);
    Ds18b20_Write_Byte(0x44);

    if(Ds18b20_Init()) return 0xFFFF;
    Ds18b20_Write_Byte(0xcc);
    Ds18b20_Write_Byte(0xbe);

    TL = Ds18b20_Read_Byte();
    TH = Ds18b20_Read_Byte();
    temp = (uint16_t)TH << 8 | TL;
    return temp;
}























