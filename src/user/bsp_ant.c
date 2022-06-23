/**
* @file       bsp.c
* @copyright  Copyright (C) 2020 Hydratech. All rights reserved.
* @license    This project is released under the Hydratech.
* @version    1.0.0
* @date       2021-07-08
* @author     Thuan Le
* @brief      ANT+ BSP
*/

/* Includes ----------------------------------------------------------- */
#include "bsp_ant.h"
#include "app_usb.h"

/* Private defines ---------------------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
void bsp_ant_send_capabilities(void)
{
  ant_msg_transfer_t msg_transfer;

  msg_transfer.msg_id = MSG_ID_CAPABILITIES;
  msg_transfer.in_len = 6;

  msg_transfer.in_data[0] = 0x08;   // Number of ANT channels available
  msg_transfer.in_data[1] = 0x03;   // Number of networks available
  msg_transfer.in_data[2] = 0x00;   // Standard Option
  msg_transfer.in_data[3] = 0xBA;   // Advanced Option
  msg_transfer.in_data[4] = 0x36;   // Advanded Option 2
  msg_transfer.in_data[5] = 0x00;   // NUmber of SensRcore channels available

  ant_msg_builder(&msg_transfer);

  app_usb_send_epin1(msg_transfer.out_data, msg_transfer.out_len);
}

/* Private function definitions --------------------------------------- */
/* End of file -------------------------------------------------------- */
