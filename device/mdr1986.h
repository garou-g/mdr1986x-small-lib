/**
  ******************************************************************************
  * @file    mdr1986.h
  * @author  GaROU (xgaroux@gmail.com)
  * @brief   Header file for choosing mcu by defines.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MDR1986_MCU_H
#define MDR1986_MCU_H

#ifdef USE_MDR1986VE1T
    #include "mdr1986ve1t.h"
#elif USE_MDR1986VE9x
    #include "mdr32fx.h"
#else
    #error "Define proccesor type!"
#endif

#endif /* MDR1986_MCU_H */

/***************************** END OF FILE ************************************/
