/**
* @file       app_usb.c
* @copyright  Copyright (C) 2020 Fiot Co., Ltd. All rights reserved.
* @license    This project is released under the Fiot License.
* @version    1.0.0
* @date       2021-07-08
* @author     Hieu Doan
* @brief      App USB
*/

/* Includes ----------------------------------------------------------- */
#include "app_usb.h"
#include "app_error.h"
#include "app_util.h"
#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_string_desc.h"
#include "app_usbd_serial_num.h"
#include "nrf_log.h"
#include "app_ant.h"

/* Private defines ---------------------------------------------------- */
#define CDC_ACM_COMM_INTERFACE  1
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN2

#define CDC_ACM_DATA_INTERFACE  0
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT1

#define TX_BUFFER               256

/* Private macros ----------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
static void m_cdc_acm_user_ev_handler(app_usbd_class_inst_t const *p_inst,
                                    app_usbd_cdc_acm_user_event_t event);

static void m_usbd_user_ev_handler(app_usbd_event_type_t event);

static void m_usb_init(const app_usbd_config_t* usb_cfg, const app_usbd_cdc_acm_t* cdc_cfg);

/* Private variables -------------------------------------------------- */
static char m_tx_buffer[TX_BUFFER];
static const app_usbd_cdc_acm_t* m_cdc_cfg;

/** @brief CDC_ACM class instance */
APP_USBD_CDC_ACM_GLOBAL_DEF(m_app_cdc_acm,
                            m_cdc_acm_user_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_AT_V250);

/* Function definitions ----------------------------------------------- */
int app_usb_init(void)
{
  static const app_usbd_config_t usbd_config =
  {
    .ev_state_proc = m_usbd_user_ev_handler
  };
  m_usb_init(&usbd_config, &m_app_cdc_acm);
  
  return NRF_SUCCESS;
}

int app_usb_send(const char* data)
{
  size_t size = sprintf(m_tx_buffer, "%s\r\n", data);
  app_usbd_cdc_acm_write(m_cdc_cfg, m_tx_buffer, size);
  
  return NRF_SUCCESS;
}

/* Private function definitions --------------------------------------- */
/** @brief User event handler @ref app_usbd_m_cdc_acm_user_ev_handler_t */
static void m_cdc_acm_user_ev_handler(app_usbd_class_inst_t const *p_inst,
                                    app_usbd_cdc_acm_user_event_t event)
{
  app_usbd_cdc_acm_t const *p_cdc_acm = app_usbd_cdc_acm_class_get(p_inst);

  switch (event)
  {
  case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
  {
    /*Set up the first transfer*/
    NRF_LOG_INFO("APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN");
    break;
  }

  case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
    NRF_LOG_INFO("APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE");
    break;

  case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:

    break;

  case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
  {
    NRF_LOG_INFO("APP_USBD_CDC_ACM_USER_EVT_RX_DONE");
    break;
  }
  default:
    break;
  }
}

static void m_usbd_user_ev_handler(app_usbd_event_type_t event)
{
  switch (event)
  {
  case APP_USBD_EVT_DRV_SUSPEND:
    NRF_LOG_INFO("APP_USBD_EVT_DRV_SUSPEND");
    break;

  case APP_USBD_EVT_DRV_RESUME:
    NRF_LOG_INFO("APP_USBD_EVT_DRV_RESUME");
    break;

  case APP_USBD_EVT_STARTED:
    NRF_LOG_INFO("APP_USBD_EVT_STARTED");
    app_ant_init();
    break;

  case APP_USBD_EVT_STOPPED:
    NRF_LOG_INFO("APP_USBD_EVT_STOPPED");
    app_usbd_disable();
    break;

  case APP_USBD_EVT_POWER_DETECTED:
    NRF_LOG_INFO("APP_USBD_EVT_POWER_DETECTED");

    if (!nrf_drv_usbd_is_enabled())
    {
      app_usbd_enable();
    }
    break;

  case APP_USBD_EVT_POWER_REMOVED:
  {
    NRF_LOG_INFO("APP_USBD_EVT_POWER_REMOVED");
    app_usbd_stop();
  }
  break;

  case APP_USBD_EVT_POWER_READY:
  {
    NRF_LOG_INFO("APP_USBD_EVT_POWER_READY");
    app_usbd_start();
  }
  break;

  default:
    break;
  }
}

static void m_usb_init(const app_usbd_config_t* usb_cfg, const app_usbd_cdc_acm_t* cdc_cfg)
{
  ret_code_t ret;
  app_usbd_serial_num_generate();
  
  m_cdc_cfg = cdc_cfg;
  ret = app_usbd_init(usb_cfg);
  APP_ERROR_CHECK(ret);

  app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(m_cdc_cfg);
  ret = app_usbd_class_append(class_cdc_acm);
  APP_ERROR_CHECK(ret);

  ret = app_usbd_power_events_enable();
  APP_ERROR_CHECK(ret);
}

/* End of file -------------------------------------------------------- */

