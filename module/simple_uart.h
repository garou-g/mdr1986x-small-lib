/*******************************************************************************
 * @file    simple_uart.h
 * @author  GaROU (xgaroux@gmail.com)
 * @brief   Header file of simple UART exchange module
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SIMPLE_UART_H
#define SIMPLE_UART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mdr1986.h"
#include "simple_queue.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  UART handle Structure definition
  */
typedef struct __UART_HandleTypeDef
{
    MDR_UART_TypeDef    *instance;      /*!< UART registers base address      */
    
    void (* initCallback)(struct __UART_HandleTypeDef *huart);
    uint32_t            hclkFreq;       /*!< HCLK frequency for init          */
    uint32_t            baudrate;       /*!< UART baudrate value              */
    uint16_t            txBufSize;      /*!< UART Tx Transfer size            */
    uint16_t            rxBufSize;      /*!< UART Rx Transfer size            */

    QueueTypedef        *txQueue;
    QueueTypedef        *rxQueue;
} UART_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void UART_Init(UART_HandleTypeDef *huart);
void UART_WriteByte(UART_HandleTypeDef *huart, uint8_t data);
void UART_WriteArray(UART_HandleTypeDef *huart, const uint8_t *data, uint32_t len);
int32_t UART_ReadByte(UART_HandleTypeDef *huart);

void UART_IRQHandler(UART_HandleTypeDef *huart);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_UART_H */

/***************************** END OF FILE ************************************/
