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

/* Exported types ------------------------------------------------------------*/
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
    void                (*write_function)(uint8_t);
    int                 (*read_function)();
    void                (*parse_function)(EXCH_MsgTypedef*);
    uint8_t             *buf;
    uint32_t            length;

} EXCH_InitTypedef;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void EXCH_Init(const EXCH_InitTypedef *exch_init);
void EXCH_Dispatcher();

uint8_t EXCH_Crc8(const uint8_t *data, uint8_t len);
uint16_t EXCH_Crc16(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_EXCHANGE_H */

/***************************** END OF FILE ************************************/
