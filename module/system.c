/*******************************************************************************
 * @file    system.c
 * @author  GaROU (xgaroux@gmail.com)
 * @brief   System management
 * @version 0.3
 * @date    2019-06-01
 *
 * @note
 * Changelog:
 * v0.3 Added calculating of first memory page by start_address parameter.
 *     Added defines for two types of cpu: MDR1986VE1T and MDR1986VE9X.
 * v0.2 Added crc32 calculate functions.
 * v0.1 First release. Deinit and clock init functions with state control.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "mdr1986.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define EEPROM_Latency_0            ((uint32_t)0x00000000)  /*!< Up to  25 MHz*/
#define EEPROM_Latency_1            ((uint32_t)0x00000008)  /*!< Up to  50 MHz*/
#define EEPROM_Latency_2            ((uint32_t)0x00000010)  /*!< Up to  75 MHz*/
#define EEPROM_Latency_3            ((uint32_t)0x00000018)  /*!< Up to 100 MHz*/
#define EEPROM_Latency_4            ((uint32_t)0x00000020)  /*!< Up to 125 MHz*/
#define EEPROM_Latency_5            ((uint32_t)0x00000028)  /*!< Up to 150 MHz*/

/** HSE and PLL ready timeout */
#define SYS_INIT_TIMEOUT            ((uint32_t)0x00000600)

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static SYS_State_Type sys_state = SYS_State_Unknown;

/* Private function prototypes -----------------------------------------------*/
static uint32_t getLastProgramAddress(uint32_t start_page);
static uint32_t crc32(uint8_t *buf, uint32_t len);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Reset the clock configuration to the default state
 */
void SYS_DeInit(void)
{
    /* Reset all clock but RST_CLK & BKP_CLK bits */
    MDR_RST_CLK->PER_CLOCK = (uint32_t)0x8000010;
    MDR_RST_CLK->CPU_CLOCK &= (uint32_t)0x00000000;
    MDR_RST_CLK->PLL_CONTROL &= (uint32_t)0x00000000;
    MDR_RST_CLK->HS_CONTROL &= (uint32_t)0x00000000;
    MDR_RST_CLK->USB_CLOCK &= (uint32_t)0x00000000;
    MDR_RST_CLK->ADC_MCO_CLOCK &= (uint32_t)0x00000000;
    MDR_RST_CLK->TIM_CLOCK &= (uint32_t)0x00000000;

    sys_state = SYS_State_Reset;
}

/**
 * @brief Initialization of clock system
 * @details Perform initialization of system clocks by several steps:
 *          1) Enables quartz rezonator and wait for stabilization;
 *          2) If stabilization is success it takes as CPU_C1 clock, else
 *              CPU_C1 set for inner HSI oscillator (and signalize about error);
 *          3) Sets PLL multiplication factor, enables it and wait,
 *              while it stabilize;
 *          4) If stabilization is success PLL takes as CPU_C2 clock and then
 *              transfer for HCLK. In other way clocks roll back for HSE/HSI
 *              frequency only (and signalize about error).
 */
void SYS_ClkInit(SYS_Freq_Type freq)
{
    uint32_t timeout, flag;
    uint32_t cpu_c1, cpu_hclk;

    sys_state = SYS_State_Ready;

    /* Enable peripheral clock */
    MDR_RST_CLK->PER_CLOCK |= RST_CLK_PER_CLOCK_PCLK_EN_RST_CLK |
                                RST_CLK_PER_CLOCK_PCLK_EN_EEPROM_CNTRL |
                                RST_CLK_PER_CLOCK_PCLK_EN_BKP;

    /* Choose EEPROM latency */
    if (freq < SYS_Freq_32Mhz)
        MDR_EEPROM->CMD = EEPROM_Latency_0;
    else if (freq < SYS_Freq_56Mhz)
        MDR_EEPROM->CMD = EEPROM_Latency_1;
    else if (freq < SYS_Freq_80Mhz)
        MDR_EEPROM->CMD = EEPROM_Latency_2;
    else if (freq < SYS_Freq_104Mhz)
        MDR_EEPROM->CMD = EEPROM_Latency_3;
    else if (freq < SYS_Freq_128Mhz)
        MDR_EEPROM->CMD = EEPROM_Latency_4;
    else
        MDR_EEPROM->CMD = EEPROM_Latency_5;

    /* Enable HSE and wait for stabilization */
    MDR_RST_CLK->HS_CONTROL = RST_CLK_HS_CONTROL_HSE_ON;
    timeout = flag = 0;
    while (timeout < SYS_INIT_TIMEOUT && flag != RST_CLK_CLOCK_STATUS_HSE_RDY)
    {
        flag = MDR_RST_CLK->CLOCK_STATUS & RST_CLK_CLOCK_STATUS_HSE_RDY;
        timeout++;
    }

    if (flag == 0)  /* HSE fails -> set CPU_C1 from internal HSI */
    {
        cpu_c1 = RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSI;
        sys_state = SYS_State_Err_HSE;
    }
    else            /* HSE success -> set CPU_C1 from HSE */
    {
        cpu_c1 = SYS_HSE_DIV == 1 ? RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSE :
                                    RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSE_DIV_2;
    }

    /* Set CPU_C1 from choosen source */
    MDR_RST_CLK->CPU_CLOCK = cpu_c1;

    /* Prepare HCLK: CPU_C3 from CPU_C2, HCLK from CPU_C3 */
    cpu_hclk = (RST_CLK_CPU_CLOCK_CPU_C3_SEL_CPU_C2 << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos) |
                (RST_CLK_CPU_CLOCK_HCLK_SEL_CPU_C3 << RST_CLK_CPU_CLOCK_HCLK_SEL_Pos);

    /* Start PLL and switch HCLK to it */
    MDR_RST_CLK->PLL_CONTROL = RST_CLK_PLL_CONTROL_PLL_CPU_ON |
        ((freq / SYS_HSE_FREQ) - 1) << RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos;
    timeout = flag = 0;
    while (timeout < SYS_INIT_TIMEOUT && flag != RST_CLK_CLOCK_STATUS_PLL_CPU_RDY)
    {
        flag = MDR_RST_CLK->CLOCK_STATUS & RST_CLK_CLOCK_STATUS_PLL_CPU_RDY;
        timeout++;
    }

    if (flag == 0)  /* PLL fails -> set HCLK from HSE */
    {
        MDR_RST_CLK->CPU_CLOCK = cpu_c1 | cpu_hclk;
        sys_state = SYS_State_Err_PLL;
    }
    else            /* PLL success -> set HCLK from PLL */
    {
        MDR_RST_CLK->CPU_CLOCK = cpu_c1 | cpu_hclk |
            (RST_CLK_CPU_CLOCK_CPU_C2_SEL_PLL_CPU << RST_CLK_CPU_CLOCK_CPU_C2_SEL_Pos);
    }
}

/**
 * @brief Returns error state of system module
 * @return state of system module
 */
SYS_State_Type SYS_State()
{
    return sys_state;
}

/**
 * @brief Get firmware checksum from last address and compare with calculated
 *
 * @return uint32_t checksum value if ok, otherwise 0
 */
uint32_t SYS_ChecksumVerify(uint32_t start_address)
{
    uint8_t *memory = (uint8_t *)(start_address);
    uint32_t last_address, start_page, flash_checksum, checksum;

    start_page = (start_address - EEPROM_ADDRESS) / EEPROM_PAGE_SIZE;
    last_address = getLastProgramAddress(start_page);
    if (last_address == 0)
        return 0;

    flash_checksum = *((unsigned int *)(last_address));
    checksum = crc32(memory, (last_address - 4) - start_address);

    return flash_checksum == checksum ? checksum : 0;
}

/* Private functions ---------------------------------------------------------*/
static uint32_t getLastProgramAddress(uint32_t start_page)
{
    int i;
    const uint32_t words_page_size = (EEPROM_PAGE_SIZE >> 2);
    uint32_t *address = (uint32_t *)(EEPROM_ADDRESS);
    uint32_t found_page = 0, found_address = 0;

    /* Find last used memory page */
    for (i = start_page; i < 32; i++)
    {
        /* First page has bootloader - don't check it */
        address += words_page_size;
        if (*address == 0xFFFFFFFF)
        {
            /* Current page is empty, take previous */
            found_page = i - 1;
            address -= words_page_size;
            break;
        }
    }

    /* If second page is empty it means no user firmware */
    if (found_page != 0)
    {
        /* Find last used memory address in page */
        for (i = 0; i < words_page_size; i++)
        {
            if (*((unsigned int *)(address)) == 0xFFFFFFFF)
            {
                /* Current address is empty, take previous */
                found_address = (uint32_t)(address - 1);
                break;
            }
            address++;
        }
    }

    return found_address;
}

/**
 * @brief Name: CRC-32
 *      Poly  : 0x04C11DB7    x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11
 *                            + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
 *      Init  : 0xFFFFFFFF
 *      Revert: true
 *      XorOut: 0xFFFFFFFF
 *      Check : 0xCBF43926 ("123456789")
 *      MaxLen: 268 435 455 bytes (2 147 483 647 bits)
 * @param  data for calculating
 * @param  len of data
 * @retval CRC-32 checksum byte
 */
static uint32_t crc32(uint8_t *data, uint32_t len)
{
    const int CRC_TABLE_SIZE = 256;
    uint32_t crc_table[CRC_TABLE_SIZE];
    uint32_t crc;
    int i, j;

    for (i = 0; i < CRC_TABLE_SIZE; i++)
    {
        crc = i;
        for (j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320U : crc >> 1;

        crc_table[i] = crc;
    };

    crc = 0xFFFFFFFFU;

    while (len--)
        crc = crc_table[(crc ^ *data++) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFFU;
}

/***************************** END OF FILE ************************************/
