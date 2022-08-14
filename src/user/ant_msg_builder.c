/**
* @file       ant_msg_builder.c
* @copyright  Copyright (C) 2020 Hydratech. All rights reserved.
* @license    This project is released under the Hydratech.
* @version    1.0.0
* @date       2021-07-08
* @author     Thuan Le
* @brief      ANT+ build message
*/

/* Includes ----------------------------------------------------------- */
#include "ant_msg_builder.h"

/* Private defines ---------------------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static uint8_t ant_msg_buffer[2 * ANT_MSG_MAX_SIZE];

/* Private function prototypes ---------------------------------------- */
static uint8_t ant_msg_builder_calculate_crc(uint8_t *p_data, uint16_t len);

/* Function definitions ----------------------------------------------- */
//  +------+------------+--------+-----------------+-----------+
//  | Sync | Msg Length | Msg ID | Message Content | Check sum |
//  |      |            |        | (Byte 0-(N-1))  |           |
//  +------+------------+--------+-----------------+-----------+
void ant_msg_builder(ant_msg_transfer_t *p_transfer)
{
  uint8_t crc;

  // Clear ANT msg buffer
  memset(ant_msg_buffer, 0, sizeof(ant_msg_buffer));

  // Create ANT message structure ======= {
  ant_msg_buffer[ANT_MSG_POS_SYNC]   = ANT_MSG_SYNC_TX_VALUE;  // Sync
  ant_msg_buffer[ANT_MSG_POS_LENGTH] = p_transfer->in_len;     // Msg Length
  ant_msg_buffer[ANT_MSG_POS_ID]     = p_transfer->msg_id;     // Msg ID
  
  // Message content
  memcpy(&ant_msg_buffer[ANT_MSG_POS_CONTENT], p_transfer->in_data, p_transfer->in_len);

  // Check sum
  crc = ant_msg_builder_calculate_crc(ant_msg_buffer, 3 + p_transfer->in_len);
  ant_msg_buffer[3 + p_transfer->in_len] = crc;
  // ===================================== }

  // Get output data for sending
  p_transfer->out_len = p_transfer->in_len + 4;
  memcpy(p_transfer->out_data, ant_msg_buffer, p_transfer->out_len);
}

/* Private function definitions --------------------------------------- */
static uint8_t ant_msg_builder_calculate_crc(uint8_t *p_data, uint16_t len)
{
  uint8_t crc = 0;

  for (int i = 0; i < len; i++)
    crc ^= p_data[i];

  return crc;
}

/* End of file -------------------------------------------------------- */
