/*******************************************************************************
 * @file    crc.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of different CRC types calculation.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CRC_H
#define __CRC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

uint8_t crc8(const uint8_t *buf, uint32_t len);
uint16_t crc16(const uint8_t *buf, uint32_t len);
uint32_t crc32(const uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __CRC_H */

/***************************** END OF FILE ************************************/
