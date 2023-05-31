/*******************************************************************************
 * @file    simple_exchange.c
 * @author  GaROU (xgaroux@gmail.com)
 * @brief   Simple serial exchange protocol
 * @version 0.7
 * @date    2019-11-03
 *
 * @note
 * Exchange uses fixed size messages with start and end markers. It has
 * one byte for commands and array for data bytes.
 *
 * Changelog:
 * v0.7 Added typedefs for callbacks.
 * v0.6 Changed behavior of module: all inner variables moved into
 *      EXCH_InstTypedef. It allows to create multiply instances of module
 *      with different physical layers.
 * v0.5 Added EXCH_Write function for send messages.
 *      Added acknowledge callback and parsing of acknowledge bytes.
 *      Added acknowledge and not acknowledge send functions.
 * v0.4 Added init function for set up callbacks and inner buffer.
 *      Added dispatcher function for proccess message and callbacks
 *      for byte write, read and message parse.
 *      Changed message structure.
 *      Deleted EXCH_PrepareMsg function as obsolete.
 *      Deleted EXCH_Crc8 function as obsolete.
 *      Added using of malloc to allocate memory for message buffer.
 * v0.3 Added function for calculating crc16 checksum.
 * v0.2 Added function for prepare message structure to transmit.
 * v0.1 First release. Function for calculating crc8 checksum.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "simple_exchange.h"
#include "crc.h"
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void dummy_write(uint8_t d) {}
static int32_t dummy_read() { return -1; }
static void dummy_parse(EXCH_MsgTypedef* d) {}
static void dummy_ack(EXCH_AckTypedef d) {}

/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void EXCH_Init(EXCH_InstTypedef *exch, uint32_t size)
{
    exch->write_function = dummy_write;
    exch->read_function = dummy_read;
    exch->parse_function = dummy_parse;
    exch->ack_function = dummy_ack;
    exch->msg = malloc(sizeof(EXCH_MsgTypedef));
    exch->msg->data = malloc(size);
    exch->msg_size = size;
    exch->state = EXCH_State_Idle;
}

void EXCH_Write(EXCH_InstTypedef *exch,
                uint8_t cmd, const uint8_t *data, uint32_t length)
{
    uint16_t crc;
    uint32_t i;

    /* Check data length and limit it by maximum message size */
    if (length > exch->msg_size)
        length = exch->msg_size;

    /* Prepare checksum before exchange start */
    crc = crc16(data, length);

    /* Send frame with all service data */
    exch->write_function(EXCH_SOH);
    exch->write_function(cmd);
    exch->write_function(length);
    for (i = 0; i < length; ++i)
        exch->write_function(data[i]);
    exch->write_function(crc & 0xFF);
    exch->write_function((crc >> 8) & 0xFF);
}

void EXCH_Ack(const EXCH_InstTypedef *exch)
{
    exch->write_function(EXCH_ACK);
}

void EXCH_Nak(const EXCH_InstTypedef *exch)
{
    exch->write_function(EXCH_NAK);
}

void EXCH_Dispatcher(EXCH_InstTypedef *exch)
{
    static uint32_t cnt = 0, crc_cnt = 0;
    int32_t byte;
    EXCH_MsgTypedef *msg;

    byte = exch->read_function();
    if (byte == -1)
        return;

    msg = exch->msg;
    switch (exch->state)
    {
        case EXCH_State_Idle:
            switch (byte)
            {
                case EXCH_SOH:
                    msg->cmd = msg->length = msg->crc = 0;
                    cnt = crc_cnt = 0;
                    exch->state = EXCH_State_Cmd;
                    break;
                case EXCH_ACK:
                    exch->ack_function(EXCH_Ack_Ok);
                    break;
                case EXCH_NAK:
                    exch->ack_function(EXCH_Ack_Error);
                    break;
                default:
                    break;
            }
            break;
        case EXCH_State_Cmd:
            msg->cmd = byte;
            exch->state = EXCH_State_Len;
            break;
        case EXCH_State_Len:
            msg->length = byte;
            exch->state = EXCH_State_Data;
            break;
        case EXCH_State_Data:
            msg->data[cnt++] = byte;
            if (cnt == msg->length)
                exch->state = EXCH_State_Crc;
            break;
        case EXCH_State_Crc:
            if (crc_cnt == 0)
            {
                msg->crc = byte & 0xFF;
                crc_cnt++;
            }
            else if (crc_cnt == 1)
            {
                msg->crc |= (byte & 0xFF) << 8;
                if (msg->crc == crc16(msg->data, msg->length))
                {
                    exch->parse_function(msg);
                    EXCH_Ack(exch);
                }
                else
                {
                    EXCH_Nak(exch);
                }
                exch->state = EXCH_State_Idle;
            }
            break;
        default:
            break;
    }
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/
