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
#include "bsp_usb_ant.h"
#include "app_usb.h"
#include "ant_hrm.h"

/* Private defines ---------------------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
ant_msg_transfer_t msg_transfer;

/* Private function prototypes ---------------------------------------- */
static void sens_message_encode(ant_hrm_profile_t *p_profile, uint8_t page, uint8_t *p_message_payload);

static void bsp_usb_ant_send_reponse_event_ant_msg_id(uint8_t channel, uint8_t msg_id, uint8_t err_code);
static void bsp_usb_ant_send_reponse_channel_id(void);

/* Function definitions ----------------------------------------------- */
void bsp_usb_ant_receive_msg_handler(char *p_buf)
{
  switch (p_buf[ANT_MSG_POS_ID])
  {
  case MESG_REQUEST_ID: // 0x4D
    // 0x00 – Request Advanced Burst Capabilities
    // 0x01 – Request Advanced Burst Current Configuration
    if (p_buf[ANT_MSG_POS_CONTENT] == 0x00) //
    {
      if (p_buf[ANT_MSG_POS_CONTENT + 1] == MESG_CHANNEL_ID_ID)
      {
        // A4 05 51 00 01 00 78 01 88
        bsp_usb_ant_send_reponse_channel_id();
      }
      else
      {
        bsp_usb_ant_send_capabilities();
      }

      return;
    }
    break;

  // Flow setup by ANT+ simulation {
  case MESG_ASSIGN_CHANNEL_ID: // 0x42 |  A4 03 42 00 00 00 E5 00 00
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], p_buf[ANT_MSG_POS_ID], RESPONSE_NO_ERROR);
    break;

  case MESG_CHANNEL_ID_ID: // 0x51, Set channel ID | A4 05 51 00 00 00 78 00 88 00 00
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], p_buf[ANT_MSG_POS_ID], RESPONSE_NO_ERROR);
    break;
  
  case MESG_CHANNEL_RADIO_FREQ_ID: // 0x45 |  A4 02 45 00 39 DA 00 00
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], p_buf[ANT_MSG_POS_ID], RESPONSE_NO_ERROR);
    break;

  case MESG_CHANNEL_MESG_PERIOD_ID: // 0x43 | A4 03 43 00 86 1F 7D 00 00
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], p_buf[ANT_MSG_POS_ID], RESPONSE_NO_ERROR);
    break;

  case MESG_PROX_SEARCH_CONFIG_ID: // 0x71 | A4 02 71 00 00 D7 00 00
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], p_buf[ANT_MSG_POS_ID], RESPONSE_NO_ERROR);
    break;

  case MESG_SET_LP_SEARCH_TIMEOUT_ID: // 0x63 | A4 02 63 00 0A CF 00 00
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], p_buf[ANT_MSG_POS_ID], RESPONSE_NO_ERROR);
    break;

  case MESG_CHANNEL_SEARCH_TIMEOUT_ID: // 0x44 | A4 02 44 00 02 E0 00 00
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], p_buf[ANT_MSG_POS_ID], RESPONSE_NO_ERROR);
    break;

  case MESG_OPEN_CHANNEL_ID: // 0x4B |  A4 01 4B 00 EE 00 00
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], p_buf[ANT_MSG_POS_ID], RESPONSE_NO_ERROR);
    break;
  // }

  case MESG_CLOSE_CHANNEL_ID: // 0x4C
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], p_buf[ANT_MSG_POS_ID], RESPONSE_NO_ERROR);
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], MESG_EVENT_ID, EVENT_CHANNEL_CLOSED);
    break;

    case MESG_ACKNOWLEDGED_DATA_ID: // 0x4F
    // A4 03 40 00 01 05 E3
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], MESG_EVENT_ID, EVENT_TRANSFER_TX_COMPLETED);
    break;

  default:
    bsp_usb_ant_send_reponse_event_ant_msg_id(p_buf[ANT_MSG_POS_CONTENT], p_buf[ANT_MSG_POS_ID], RESPONSE_NO_ERROR);
    break;
  }


  //  A4 02 6E 00 E0 28 00 00                           ¤.n.à(..
}

void bsp_usb_ant_send_capabilities(void)
{
  msg_transfer.msg_id = MESG_CAPABILITIES_ID;
  msg_transfer.in_len = 6;

  msg_transfer.in_data[0] = 0x08;   // Number of ANT channels available
  msg_transfer.in_data[1] = 0x03;   // Number of networks available
  msg_transfer.in_data[2] = 0x00;   // Standard Option
  msg_transfer.in_data[3] = 0xBA;   // Advanced Option
  msg_transfer.in_data[4] = 0x36;   // Advanded Option 2
  msg_transfer.in_data[5] = 0x00;   // Number of SensRcore channels available

  ant_msg_builder(&msg_transfer);

  app_usb_send_epin1(msg_transfer.out_data, msg_transfer.out_len);
}

void bsp_usb_ant_send_reponse_channel_id(void)
{
  msg_transfer.msg_id = MESG_CHANNEL_ID_ID;
  msg_transfer.in_len = 5;

  // A4 05 51 00 01 00 78 01 88
  msg_transfer.in_data[0] = 0x00; // Channel
  msg_transfer.in_data[1] = 0x01; // Device number
  msg_transfer.in_data[2] = 0x00; // Device type
  msg_transfer.in_data[3] = 0x78; // Device type
  msg_transfer.in_data[4] = 0x01; // Transmission type

  ant_msg_builder(&msg_transfer);

  app_usb_send_epin1(msg_transfer.out_data, msg_transfer.out_len);
}

void bsp_usb_ant_send_reponse_event_ant_msg_id(uint8_t channel, uint8_t msg_id, uint8_t err_code)
{
  msg_transfer.msg_id = MESG_RESPONSE_EVENT_ID;
  msg_transfer.in_len = 3;

  msg_transfer.in_data[0] = channel;   // Channel
  msg_transfer.in_data[1] = msg_id;    // Msg ID
  msg_transfer.in_data[2] = err_code;  // Msg code

  ant_msg_builder(&msg_transfer);

  app_usb_send_epin1(msg_transfer.out_data, msg_transfer.out_len);
}

void bsp_usb_ant_send_broadcast_data(uint8_t *p_data, uint16_t len)
{
  msg_transfer.msg_id = MESG_BROADCAST_DATA_ID;
  msg_transfer.in_len = len;

  memcpy(msg_transfer.in_data, p_data, msg_transfer.in_len);

  ant_msg_builder(&msg_transfer);

  app_usb_send_epin1(msg_transfer.out_data, msg_transfer.out_len);
}

void bsp_usb_ant_send_hrm_msg(ant_hrm_profile_t *p_profile, uint8_t page)
{
  uint8_t p_message_payload[ANT_STANDARD_DATA_PAYLOAD_SIZE];

  sens_message_encode(p_profile, page, p_message_payload);

  bsp_usb_ant_send_broadcast_data(p_message_payload, 8);
}

/* Private function definitions --------------------------------------- */
static void sens_message_encode(ant_hrm_profile_t *p_profile, uint8_t page, uint8_t *p_message_payload)
{
//  ant_hrm_message_layout_t *p_hrm_message_payload = (ant_hrm_message_layout_t *)p_message_payload;
//  ant_hrm_sens_cb_t        *p_hrm_cb              = p_profile->_cb.p_sens_cb;

//  p_hrm_message_payload->page_number = page;
//  p_hrm_message_payload->toggle_bit  = p_hrm_cb->toggle_bit;

//  NRF_LOG_INFO("HRM TX Page number: %u", p_hrm_message_payload->page_number);

//  ant_hrm_page_0_encode(p_hrm_message_payload->page_payload, &(p_profile->page_0)); // Page 0 is present in each message

//  switch (p_hrm_message_payload->page_number)
//  {
//  case ANT_HRM_PAGE_0:
//    // No implementation needed
//    break;

//  case ANT_HRM_PAGE_1:
//    ant_hrm_page_1_encode(p_hrm_message_payload->page_payload, &(p_profile->page_1));
//    break;

//  case ANT_HRM_PAGE_2:
//    ant_hrm_page_2_encode(p_hrm_message_payload->page_payload, &(p_profile->page_2));
//    break;

//  case ANT_HRM_PAGE_3:
//    ant_hrm_page_3_encode(p_hrm_message_payload->page_payload, &(p_profile->page_3));
//    break;

//  case ANT_HRM_PAGE_4:
//    ant_hrm_page_4_encode(p_hrm_message_payload->page_payload, &(p_profile->page_4));
//    break;

//  default:
//    return;
//  }

//  p_profile->evt_handler(p_profile, (ant_hrm_evt_t)p_hrm_message_payload->page_number);
}

/* End of file -------------------------------------------------------- */
