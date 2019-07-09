/*******************************************************************************
 * @file    simple_exchange.c
 * @author  GaROU (xgaroux@gmail.com)
 * @brief   Simple queue realization
 * @version 0.1
 * @date    2019-07-09
 *
 * @note
 * Simple queue representation with external buffer definition.
 *
 * Changelog:
 * v0.1 First release.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "simple_queue.h"
#include <limits.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void QUEUE_Init(QueueTypedef *queue, int32_t *buf, uint32_t length)
{
    queue->data = buf;
    queue->length = length;
    queue->first = 0;
    queue->last  = length - 1;
    queue->size  = 0;
}

int32_t QUEUE_IsFull(const QueueTypedef* queue)
{
    return (queue->size == queue->length);
}

int32_t QUEUE_IsEmpty(const QueueTypedef* queue)
{
    return (queue->size == 0);
}

void QUEUE_Enqueue(QueueTypedef* queue, int32_t item)
{
    if (isFull(queue))
        return;

    queue->last = (queue->last + 1) % queue->length;
    queue->data[queue->last] = item;
    queue->size++;
}

int32_t QUEUE_Dequeue(QueueTypedef* queue)
{
    if (isEmpty(queue))
        return INT_MIN;

    int item = queue->data[queue->first];
    queue->first = (queue->first + 1) % queue->length;
    queue->size--;
    return item;
}

int32_t QUEUE_First(const QueueTypedef* queue)
{
    if (isEmpty(queue))
        return INT_MIN;

    return queue->data[queue->first];
}

int32_t QUEUE_Last(const QueueTypedef* queue)
{
    if (isEmpty(queue))
        return INT_MIN;

    return queue->data[queue->last];
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/
