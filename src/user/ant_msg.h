/**
* @file       ant_msg.h
* @copyright  Copyright (C) 2020 Hydratech. All rights reserved.
* @license    This project is released under the Hydratech.
* @version    1.0.0
* @date       2021-07-08
* @author     Thuan Le
* @brief      ANT+ message
*/

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __ANT_MSG_H
#define __ANT_MSG_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <string.h>
#include "ant_parameters.h"

/* Public defines ----------------------------------------------------- */
#define ANT_MSG_SYNC_TX_VALUE                 (0xA4)
#define ANT_MSG_SYNC_RX_VALUE                 (0xA5)
#define ANT_MSG_SYNC_SIZE                     (1)
#define ANT_MSG_SIZE_SIZE                     (1)
#define ANT_MSG_ID_SIZE                       (1)
#define ANT_MSG_CHECKSUM_SIZE                 (1)
#define ANT_MSG_MAX_DATA_SIZE                 (17)
#define ANT_MSG_HEADER_SIZE                   (ANT_MSG_SYNC_SIZE + ANT_MSG_SIZE_SIZE + ANT_MSG_ID_SIZE)
#define ANT_MSG_FRAME_SIZE                    (ANT_MSG_HEADER_SIZE + ANT_MSG_CHECKSUM_SIZE)
#define ANT_MSG_MAX_SIZE                      (ANT_MSG_MAX_DATA_SIZE + ANT_MSG_FRAME_SIZE)

/* Public macros ------------------------------------------------------ */
/* Public enumerate/structure ----------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

#endif // __ANT_MSG_H

/* End of file -------------------------------------------------------- */
