/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{

  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }

}
//==========================================================
//	函数名称：	MPU6050_I2C1_Init()
//
//	函数功能：	初始化与MPU6050陀螺仪通信I2C1通道。设置通信速率为100000Hz。
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
I2C_HandleTypeDef hi2c1;
void MPU6050_I2C1_Init(void)
{
    

    __HAL_RCC_GPIOB_CLK_ENABLE();                         //使能GPIOB端口时钟，I2C1位于GPIOB端。
    
    hi2c1.Instance             = I2C1;                    //选择I2C1，芯片内部有两个I2C通道，I2C1、I2C2
    hi2c1.Init.ClockSpeed      = 200000;                  //设置I2C1,的通信速率为100000Hz(100K),最大速率不能超过400KHz。
    hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;         //设置I2C1的时钟的点空比为50%
    hi2c1.Init.OwnAddress1     = 0;                       //I2C1设置本身的地址为0，这个地址只有在做为从设备时才会使用，本次实验从设备是MPU6050
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT; //设置I2C1的地址模式
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE; //禁止双地址功能
    hi2c1.Init.OwnAddress2     = 0;                       //I2C1设置本身的地址为0，这个地址只有在做为从设备时才会使用，本次实验从设备是MPU6050
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE; 
    hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        while(1);
    }

}

