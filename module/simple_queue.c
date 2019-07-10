/*******************************************************************************
 * @file    simple_exchange.c
 * @author  GaROU (xgaroux@gmail.com)
 * @brief   Simple queue realization
 * @version 0.3
 * @date    2019-07-10
 *
 * @note
 * Simple queue representation with external buffer definition.
 *
 * Changelog:
 * v0.3 Added using of malloc to allocate memory for queue buffer.
 * v0.2 Change type of data to uint8_t except of return values, because
 *      functions returns -1 when queue has no data.
 * v0.1 First release.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "simple_queue.h"
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void QUEUE_Init(QueueTypedef *queue, uint32_t size)
{
    queue->data = malloc(size);
    queue->length = size;
    queue->first = 0;
    queue->last  = size - 1;
    queue->size  = 0;
}

uint8_t QUEUE_IsFull(const QueueTypedef* queue)
{
    return (queue->size == queue->length);
}

uint8_t QUEUE_IsEmpty(const QueueTypedef* queue)
{
    return (queue->size == 0);
}

void QUEUE_Enqueue(QueueTypedef* queue, uint8_t item)
{
    if (QUEUE_IsFull(queue))
        return;

    queue->last = (queue->last + 1) % queue->length;
    queue->data[queue->last] = item;
    queue->size++;
}

int32_t QUEUE_Dequeue(QueueTypedef* queue)
{
    if (QUEUE_IsEmpty(queue))
        return -1;

    int item = queue->data[queue->first];
    queue->first = (queue->first + 1) % queue->length;
    queue->size--;
    return item;
}

int32_t QUEUE_First(const QueueTypedef* queue)
{
    if (QUEUE_IsEmpty(queue))
        return -1;

    return queue->data[queue->first];
}

int32_t QUEUE_Last(const QueueTypedef* queue)
{
    if (QUEUE_IsEmpty(queue))
        return -1;

    return queue->data[queue->last];
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/
