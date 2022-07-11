/**
* @file       ant_msg_builder.h
* @copyright  Copyright (C) 2020 Hydratech. All rights reserved.
* @license    This project is released under the Hydratech.
* @version    1.0.0
* @date       2021-07-08
* @author     Thuan Le
* @brief      ANT+ build message
*/

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __ANT_MSG_BUILDER_H
#define __ANT_MSG_BUILDER_H

/* Includes ----------------------------------------------------------- */
#include "ant_msg.h"

/* Public defines ----------------------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public enumerate/structure ----------------------------------------- */
typedef enum
{
   ANT_MSG_POS_SYNC
  ,ANT_MSG_POS_LENGTH
  ,ANT_MSG_POS_ID
  ,ANT_MSG_POS_CONTENT
}
ant_msg_pos_t;

typedef struct
{
  uint8_t msg_id;
  uint8_t in_data[ANT_MSG_MAX_DATA_SIZE];
  uint8_t in_len;
  uint8_t out_data[ANT_MSG_MAX_SIZE];
  uint16_t out_len;
}
ant_msg_transfer_t;

/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief         ANT message builder
 *                +------+------------+--------+-----------------+-----------+
 *                | Sync | Msg Length | Msg ID | Message Content | Check sum |
 *                |      |            |        | (Byte 0-(N-1))  |           |
 *                +------+------------+--------+-----------------+-----------+
 *
 * @param[in]     p_transfer      Pointer to transfer params
 *
 * @return        None
 */
void ant_msg_builder(ant_msg_transfer_t *p_transfer);

#endif // __ANT_MSG_BUILDER_H

/* End of file -------------------------------------------------------- */
