/*******************************************************************************
 * @file    simple_exchange.h
 * @author  GaROU (xgaroux@gmail.com)
 * @brief   Header file of simple serial exchange protocol
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SIMPLE_EXCHANGE_H
#define SIMPLE_EXCHANGE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
#define EXCH_SOH                    0x01    /*!< Message start of header flag */
#define EXCH_ACK                    0x06    /*!< Acknowledge command          */
#define EXCH_NAK                    0x15    /*!< Not acknowledge command      */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    EXCH_State_Idle,
    EXCH_State_Start,
    EXCH_State_Cmd,
    EXCH_State_Len,
    EXCH_State_Data,
    EXCH_State_Crc

} EXCH_StateTypedef;

typedef enum
{
    EXCH_Ack_Ok,
    EXCH_Ack_Error

} EXCH_AckTypedef;

/**
 * @brief Exchange message structure definition
 */
typedef struct
{
    uint8_t             cmd;        /*!< Command byte                         */
    uint8_t             *data;      /*!< Frame data array pointer             */
    uint8_t             length;     /*!< Frame data length                    */
    uint16_t            crc;        /*!< Frame checksum 2 bytes               */

} EXCH_MsgTypedef;

typedef struct
{
    /* Callbacks needs to setup from user code */
    void                (*write_function)(uint8_t);
    int                 (*read_function)();
    void                (*parse_function)(EXCH_MsgTypedef*);
    void                (*ack_function)(EXCH_AckTypedef);

    /* Private data of instance structure. Modified only internaly my module */
    EXCH_MsgTypedef     *msg;
    uint32_t            msg_size;
    EXCH_StateTypedef   state;

} EXCH_InstTypedef;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void EXCH_Init(EXCH_InstTypedef *exch, uint32_t size);
void EXCH_Write(EXCH_InstTypedef *exch,
                uint8_t cmd, const uint8_t *data, uint32_t length);
void EXCH_Ack(const EXCH_InstTypedef *exch);
void EXCH_Nak(const EXCH_InstTypedef *exch);
void EXCH_Dispatcher(EXCH_InstTypedef *exch);

uint16_t EXCH_Crc16(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_EXCHANGE_H */

/***************************** END OF FILE ************************************/
