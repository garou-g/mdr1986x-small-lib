/*******************************************************************************
 * @file    simple_queue.h
 * @author  GaROU (xgaroux@gmail.com)
 * @brief   Header file of simple queue realization
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SIMPLE_QUEUE_H
#define SIMPLE_QUEUE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief Queue representation structure definition
 */
typedef struct
{
    uint8_t* data;                  /*!< Pointer to queue data array          */
    uint32_t length;                /*!< Length of queue data array           */
    uint8_t first;                  /*!< Index of first queue item            */
    uint8_t last;                   /*!< Index of last queue item             */
    uint8_t size;                   /*!< Current count of queue data          */

} QueueTypedef;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void QUEUE_Init(QueueTypedef *queue, uint8_t *buf, uint32_t length);
uint8_t QUEUE_IsFull(const QueueTypedef* queue);
uint8_t QUEUE_IsEmpty(const QueueTypedef* queue);
void QUEUE_Enqueue(QueueTypedef* queue, uint8_t item);
int32_t QUEUE_Dequeue(QueueTypedef* queue);
int32_t QUEUE_First(const QueueTypedef* queue);
int32_t QUEUE_Last(const QueueTypedef* queue);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_QUEUE_H */

/***************************** END OF FILE ************************************/
