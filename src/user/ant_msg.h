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
/**
 * @brief ANT message ID enum
 */
typedef enum
{
    MSG_ID_CONFIG_UNASSIGN_CHANNEL             = 0x41
  , MSG_ID_CONFIG_ASSIGN_CHANNEL               = 0x42
  , MSG_ID_CONFIG_CHANNEL_ID                   = 0x51
  , MSG_ID_CONFIG_CHANNEL_PERIOD               = 0x43
  , MSG_ID_CONFIG_SEARCH_TIMEOUT               = 0x44
  , MSG_ID_CONFIG_CHANNEL_RF_FREQUENCY         = 0x45
  , MSG_ID_CONFIG_SET_NETWORK_KEY              = 0x46
  , MSG_ID_CONFIG_TRANSMIT_POWER               = 0x47
  , MSG_ID_CONFIG_SEARCH_WAVEFORM              = 0x49
  , MSG_ID_CONFIG_ADD_CHANNEL_ID               = 0x59
  , MSG_ID_CONFIG_ADD_ENCRYPTION_ID            = 0x59
  , MSG_ID_CONFIG_ID_LIST                      = 0x5A
  , MSG_ID_CONFIG_ENCRYTION_ID                 = 0x5A
  , MSG_ID_CONFIG_CHANNEL_TRANSMIT_POWER       = 0x60
  , MSG_ID_CONFIG_LOW_PRIORITY_SEARCH_TIMEOUT  = 0x63
  , MSG_ID_CONFIG_SERIAL_NUMBER_SET_CHANNEL_ID = 0x65
  , MSG_ID_NOTI_STARTUP_MSG                    = 0x6F
  , MSG_ID_NOTI_SERIAL_ERROR_MSG               = 0xAE
  , MSG_ID_CTRL_RESET_SYSTEM                   = 0x4A
  , MSG_ID_DATA_BROADCAST_DATA                 = 0x4E
  , MSG_ID_CHANNEL_EVENT                       = 0x40
  , MSG_ID_CHANNEL_RESPONSE                    = 0x40
  , MSG_ID_REQ_RES_CHANNEL_STATUS              = 0x52
  , MSG_ID_TEST_CW_INIT                        = 0x53
  , MSG_ID_CAPABILITIES                        = 0x54
}
ant_msg_id_t;

/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

#endif // __ANT_MSG_H

/* End of file -------------------------------------------------------- */
