/*******************************************************************************
 * @file    simple_uart.c
 * @author  GaROU (xgaroux@gmail.com)
 * @brief   UART exchange
 * @version 0.2
 * @date    2019-11-03
 *
 * @note
 * UART control and exchange. It uses simple_queue module for process
 * stream of input and output bytes.
 *
 * Changelog:
 * v0.2 Added handler structure with init callback for low-level hardware setup.
 * v0.1 Inital release and basic communication function with queue.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "simple_uart.h"
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define UART_FRACT_MASK             ((uint16_t)0x003F)  /*!< Fract div mask   */
#define UART_DEFAULT_BUF_SIZE       ((uint32_t)64)      /*!< Max queue size   */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

__STATIC_INLINE void read(UART_HandleTypeDef *huart);
__STATIC_INLINE void write(UART_HandleTypeDef *huart);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief UART module initalization function
 * @details Perform setup of gpio for UART and queue setup.
 *
 * @param freq of hclk clock to calculate UART baudrate registers
 * @param baudrate speed of exchange
 */
void UART_Init(UART_HandleTypeDef *huart)
{
    uint32_t tmpreg;
    IRQn_Type irq;

    /* Check the UART handle allocation */
    if (huart == 0)
        return;

    /* Init the low level hardware */
    huart->initCallback(huart);

    /* Baudrate calculation (from MDR32F9Qx_uart library) */
    tmpreg = huart->hclkFreq / (huart->baudrate >> 2);
    huart->instance->IBRD = tmpreg >> 6;
    huart->instance->FBRD = (tmpreg & UART_FRACT_MASK);

    /* Init UART */
    huart->instance->ILPR = 0;
    huart->instance->DMACR = 0;
    huart->instance->RSR_ECR = 0;
    huart->instance->LCR_H = (UART_LCR_H_WLEN_8_BITS << UART_LCR_H_WLEN_Pos) |
                        UART_LCR_H_FEN;
    huart->instance->IMSC = UART_IMSC_RXIM | UART_IMSC_TXIM | UART_IMSC_RTIM;
    huart->instance->IFLS = (UART_IFLS_IFLSEL_7_DIV_8 << UART_IFLS_RXIFLSEL_Pos) |
                      (UART_IFLS_IFLSEL_1_DIV_8 << UART_IFLS_TXIFLSEL_Pos);
    huart->instance->CR = UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE;

    /* Init UART IRQ */
    if (huart->instance == MDR_UART1)
        irq = UART1_IRQn;
    else
        irq = UART2_IRQn;

    NVIC_EnableIRQ(irq);
    NVIC_SetPriority(irq, 0);

    /* Init queues */
    huart->txQueue = malloc(sizeof(QueueTypedef));
    huart->rxQueue = malloc(sizeof(QueueTypedef));
    QUEUE_Init(huart->txQueue, huart->txBufSize != 0 ? huart->txBufSize : UART_DEFAULT_BUF_SIZE);
    QUEUE_Init(huart->rxQueue, huart->rxBufSize != 0 ? huart->rxBufSize : UART_DEFAULT_BUF_SIZE);
}

/**
 * @brief Sends byte by UART
 *
 * @param data to send
 */
void UART_WriteByte(UART_HandleTypeDef *huart, uint8_t data)
{
    /* Add data to tx queue */
    if (!QUEUE_IsFull(huart->txQueue))
        QUEUE_Enqueue(huart->txQueue, data);

    /* Init send process */
    write(huart);
}

/**
 * @brief Sends array of bytes by UART
 *
 * @param data array to send
 * @param len of data array
 */
void UART_WriteArray(UART_HandleTypeDef *huart, const uint8_t *data, uint32_t len)
{
    uint32_t i = 0;

    /* Add all data from array to queue until data ends or queue fills */
    while (len-- && !QUEUE_IsFull(huart->txQueue))
        QUEUE_Enqueue(huart->txQueue, data[i++]);

    /* Init send process */
    write(huart);
}

/**
 * @brief Returns received byte
 *
 * @return data byte or -1 if no data exist
 */
int32_t UART_ReadByte(UART_HandleTypeDef *huart)
{
    if (!QUEUE_IsEmpty(huart->rxQueue))
        return QUEUE_Dequeue(huart->rxQueue);
    else
        return -1;
}

/**
 * @brief UART interrupt service routine
 */
void UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if (huart->instance->MIS & UART_MIS_RXMIS)  /* Check rx irq status */
    {
        huart->instance->ICR |= UART_ICR_RXIC;  /* Reset irq request */
        read(huart);                            /* Read data to queue */
    }

    if (huart->instance->MIS & UART_MIS_RTMIS)  /* Check rx timeout irq status */
    {
        huart->instance->ICR |= UART_ICR_RTIC;  /* Reset irq request */
        read(huart);                            /* Read data to queue */
    }

    if (huart->instance->MIS & UART_MIS_TXMIS)  /* Check tx irq status */
    {
        huart->instance->ICR |= UART_ICR_TXIC;  /* Reset irq request */
        write(huart);                           /* Send data from queue */
    }
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Reads data from UART FIFO register to rx queue
 */
__STATIC_INLINE void read(UART_HandleTypeDef *huart)
{
    while (!QUEUE_IsFull(huart->rxQueue) && !(huart->instance->FR & UART_FR_RXFE))
        QUEUE_Enqueue(huart->rxQueue, huart->instance->DR);
}

/**
 * @brief Writes data from tx queue to UART FIFO register
 */
__STATIC_INLINE void write(UART_HandleTypeDef *huart)
{
    while (!QUEUE_IsEmpty(huart->txQueue) && !(huart->instance->FR & UART_FR_TXFF))
        huart->instance->DR = QUEUE_Dequeue(huart->txQueue);
}

/***************************** END OF FILE ************************************/
