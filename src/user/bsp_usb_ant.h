/**
* @file       bsp_ant.h
* @copyright  Copyright (C) 2020 Hydratech. All rights reserved.
* @license    This project is released under the Hydratech.
* @version    1.0.0
* @date       2021-07-08
* @author     Thuan Le
* @brief      ANT+ BSP
*/

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_ANT_H
#define __BSP_ANT_H

/* Includes ----------------------------------------------------------- */
#include "ant_msg_builder.h"

/* Public defines ----------------------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public enumerate/structure ----------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief Send capabilities
 */
void bsp_usb_ant_send_capabilities(void);

/**
 * @brief Send broadcast data
 */
void bsp_usb_ant_send_broadcast_data(uint8_t *p_data, uint16_t len);

#endif // __BSP_ANT_H

/* End of file -------------------------------------------------------- */
