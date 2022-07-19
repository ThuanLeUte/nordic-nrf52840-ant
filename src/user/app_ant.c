/**
* @file       app_ant.c
* @copyright  Copyright (C) 2020 Fiot Co., Ltd. All rights reserved.
* @license    This project is released under the Fiot License.
* @version    1.0.0
* @date       2021-07-08
* @author     Hieu Doan
* @brief      App ANT+
*/

/* Includes ----------------------------------------------------------- */
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "nrf_log.h"
#include "app_error.h"
#include "app_ant.h"
#include "ant_key_manager.h"
#include "ant_state_indicator.h"

#include "ant_hrm.h"
#include "ant_bpwr.h"
#include "ant_bsc.h"
#include "app_usb.h"
#include "bsp_usb_ant.h"

/* Private defines ---------------------------------------------------- */
#define WHEEL_CIRCUMFERENCE         2070                                         /**< Bike wheel circumference [mm] */
#define BSC_EVT_TIME_FACTOR         1024                                         /**< Time unit factor for BSC events */
#define BSC_RPM_TIME_FACTOR         60                                           /**< Time unit factor for RPM unit */
#define BSC_MS_TO_KPH_NUM           36                                           /**< Numerator of [m/s] to [kph] ratio */
#define BSC_MS_TO_KPH_DEN           10                                           /**< Denominator of [m/s] to [kph] ratio */
#define BSC_MM_TO_M_FACTOR          1000                                         /**< Unit factor [m/s] to [mm/s] */
#define SPEED_COEFFICIENT           (WHEEL_CIRCUMFERENCE * BSC_EVT_TIME_FACTOR * BSC_MS_TO_KPH_NUM \
                                     / BSC_MS_TO_KPH_DEN / BSC_MM_TO_M_FACTOR)   /**< Coefficient for speed value calculation */
#define CADENCE_COEFFICIENT         (BSC_EVT_TIME_FACTOR * BSC_RPM_TIME_FACTOR)  /**< Coefficient for cadence value calculation */

/* Private macros ----------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
typedef struct
{
  int32_t acc_rev_cnt;
  int32_t prev_rev_cnt;
  int32_t prev_acc_rev_cnt;
  int32_t acc_evt_time;
  int32_t prev_evt_time;
  int32_t prev_acc_evt_time;
} 
bsc_disp_calc_data_t;

ant_evt_t ant_data;

/* Public variables --------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/*! EVT functions */
static void m_ant_hrm_evt_handler(ant_hrm_profile_t *p_profile, ant_hrm_evt_t event);
static void m_ant_bpwr_evt_handler(ant_bpwr_profile_t *p_profile, ant_bpwr_evt_t event);
static void m_ant_bsc_evt_handler(ant_bsc_profile_t *p_profile, ant_bsc_evt_t event);

/*! Display EVT functions */
static void m_ant_hrm_disp_evt_handler(ant_evt_t *p_ant_evt, void *p_context);

/*! Support functions */
static uint32_t m_calculate_speed(int32_t rev_cnt, int32_t evt_time);
static uint32_t m_calculate_cadence(int32_t rev_cnt, int32_t evt_time);

static char m_tx_buff[256] = { 0 };
/* Private variables -------------------------------------------------- */
static bsc_disp_calc_data_t m_speed_calc_data = {0};
static bsc_disp_calc_data_t m_cadence_calc_data = {0};

/*! HRM profile */
static ant_hrm_profile_t m_ant_hrm;

HRM_DISP_CHANNEL_CONFIG_DEF(m_ant_hrm,
                            HRM_CHANNEL_NUM,
                            CHAN_ID_TRANS_TYPE,
                            CHAN_ID_DEV_NUM,
                            ANTPLUS_NETWORK_NUM,
                            HRM_MSG_PERIOD_4Hz);

/*! BPWR profile */
static ant_bpwr_profile_t m_ant_bpwr;

BPWR_DISP_CHANNEL_CONFIG_DEF(m_ant_bpwr,
                             BPWR_CHANNEL_NUM,
                             CHAN_ID_TRANS_TYPE,
                             CHAN_ID_DEV_NUM,
                             ANTPLUS_NETWORK_NUM);

BPWR_DISP_PROFILE_CONFIG_DEF(m_ant_bpwr,
                             m_ant_bpwr_evt_handler);

/*! BSC profile */
static ant_bsc_profile_t m_ant_bsc;

BSC_DISP_CHANNEL_CONFIG_DEF(m_ant_bsc,
                            BSC_CHANNEL_NUM,
                            CHAN_ID_TRANS_TYPE,
                            DISPLAY_TYPE,
                            CHAN_ID_DEV_NUM,
                            ANTPLUS_NETWORK_NUM,
                            BSC_MSG_PERIOD_4Hz);

BSC_DISP_PROFILE_CONFIG_DEF(m_ant_bsc,
                            m_ant_bsc_evt_handler);

/* Function definitions ----------------------------------------------- */
int app_ant_init(void)
{
  ret_code_t err_code = nrf_sdh_enable_request();
  APP_ERROR_CHECK(err_code);

  ASSERT(nrf_sdh_is_enabled());

  // Enable ANT stack.
  err_code = nrf_sdh_ant_enable();
  APP_ERROR_CHECK(err_code);

  err_code = ant_plus_key_set(ANTPLUS_NETWORK_NUM);
  APP_ERROR_CHECK(err_code);

  // Register a handler for ANT events.
  NRF_SDH_ANT_OBSERVER(m_ant_hrm_observer, ANT_HRM_ANT_OBSERVER_PRIO, m_ant_hrm_disp_evt_handler, &m_ant_hrm);

  NRF_SDH_ANT_OBSERVER(m_ant_bpwr_observer, ANT_BPWR_ANT_OBSERVER_PRIO, ant_bpwr_disp_evt_handler, &m_ant_bpwr);

  NRF_SDH_ANT_OBSERVER(m_ant_bsc_observer, ANT_BSC_ANT_OBSERVER_PRIO, ant_bsc_disp_evt_handler, &m_ant_bsc);

  /** @snippet [ANT Profile Setup] */

  err_code = ant_hrm_disp_init(&m_ant_hrm, HRM_DISP_CHANNEL_CONFIG(m_ant_hrm), m_ant_hrm_evt_handler);
  APP_ERROR_CHECK(err_code);

  err_code = ant_hrm_disp_open(&m_ant_hrm);
  APP_ERROR_CHECK(err_code);

  err_code = ant_bpwr_disp_init(&m_ant_bpwr,
                                BPWR_DISP_CHANNEL_CONFIG(m_ant_bpwr),
                                BPWR_DISP_PROFILE_CONFIG(m_ant_bpwr));
  APP_ERROR_CHECK(err_code);

  err_code = ant_bpwr_disp_open(&m_ant_bpwr);
  APP_ERROR_CHECK(err_code);

  err_code = ant_bsc_disp_init(&m_ant_bsc,
                               BSC_DISP_CHANNEL_CONFIG(m_ant_bsc),
                               BSC_DISP_PROFILE_CONFIG(m_ant_bsc));
  APP_ERROR_CHECK(err_code);

  err_code = ant_bsc_disp_open(&m_ant_bsc);
  APP_ERROR_CHECK(err_code);
  /** @snippet [ANT Profile Setup] */

  return NRF_SUCCESS;
}

/* Private function definitions --------------------------------------- */
/**@brief Function for handling HRM profile's events
 *
 */
static void m_ant_hrm_evt_handler(ant_hrm_profile_t *p_profile, ant_hrm_evt_t event)
{
  switch (event)
  {
  case ANT_HRM_PAGE_0_UPDATED:
  {
    sprintf(m_tx_buff, "=== HRM page 0 ===\nBeat count: %u\nHeart rate: %u\nBeat time: %u\r\n", 
    p_profile->HRM_PROFILE_beat_count, p_profile->HRM_PROFILE_computed_heart_rate, p_profile->HRM_PROFILE_beat_time);
    app_usb_send(m_tx_buff);
    
    NRF_LOG_INFO("Page 0 HRM was updated\r\n");
    break;
  }
  case ANT_HRM_PAGE_1_UPDATED:
  {
    sprintf(m_tx_buff, "=== HRM page 1 ===\nOper time: %u\r\n", p_profile->HRM_PROFILE_operating_time);
    app_usb_send(m_tx_buff);

    NRF_LOG_INFO("Page 1 HRM was updated\r\n");
    break;
  }
  case ANT_HRM_PAGE_2_UPDATED:
  {
    sprintf(m_tx_buff, "=== HRM page 2 ===\nManuf id: %u\nSerial num: %u\r\n",
            p_profile->HRM_PROFILE_manuf_id, p_profile->HRM_PROFILE_serial_num);
    app_usb_send(m_tx_buff);

    NRF_LOG_INFO("Page 2 HRM was updated\r\n");
    break;
  }
  case ANT_HRM_PAGE_3_UPDATED:
  {
    sprintf(m_tx_buff, "=== HRM page 3 ===\nHW version: %u\nSW version: %u\nModel num: %u\r\n",
            p_profile->HRM_PROFILE_hw_version, p_profile->HRM_PROFILE_sw_version, p_profile->HRM_PROFILE_model_num);
    app_usb_send(m_tx_buff);

    NRF_LOG_INFO("Page 3 HRM was updated\r\n");
    break;
  }
  case ANT_HRM_PAGE_4_UPDATED:
  {
    sprintf(m_tx_buff, "=== HRM page 4 ===\nManuf_spec: %u\nPrev_beat: %u\r\n",
            p_profile->HRM_PROFILE_manuf_spec, p_profile->HRM_PROFILE_prev_beat);
    app_usb_send(m_tx_buff);

    NRF_LOG_INFO("Page 4 HRM was updated\r\n");
    break;
  }
  default:
    break;
  }
}

/**@brief Function for handling Bicycle Power profile's events
 *
 */
static void m_ant_bpwr_evt_handler(ant_bpwr_profile_t *p_profile, ant_bpwr_evt_t event)
{
  switch (event)
  {
  case ANT_BPWR_PAGE_1_UPDATED:
    // calibration data received from sensor
    NRF_LOG_DEBUG("Received calibration data");
    break;

  case ANT_BPWR_PAGE_16_UPDATED:
  {
    sprintf(m_tx_buff, "=== BPWR page 16 ===\nPower_evt_cnt: %u\nAccumulate power: %u W\nInstantaneous: %u rpm\r\n",
            p_profile->BPWR_PROFILE_power_update_event_count, p_profile->BPWR_PROFILE_accumulated_power, 
            p_profile->BPWR_PROFILE_instantaneous_power);
    app_usb_send(m_tx_buff);

    NRF_LOG_DEBUG("Page 16 BPWR was updated");
    break;
  }
  case ANT_BPWR_PAGE_17_UPDATED:
  {
    sprintf(m_tx_buff, "=== BPWR page 17 ===\nWheel_evt_cnt: %u\nWheel_tick: %u\nWheel_period: %u\nWheel_acc_torque: %u\r\n",
            p_profile->BPWR_PROFILE_wheel_update_event_count, p_profile->BPWR_PROFILE_wheel_tick, 
            p_profile->BPWR_PROFILE_wheel_period, p_profile->BPWR_PROFILE_wheel_accumulated_torque);
    app_usb_send(m_tx_buff);

    NRF_LOG_DEBUG("Page 17 BPWR was updated");
    break;
  }
  case ANT_BPWR_PAGE_18_UPDATED:
  {
    sprintf(m_tx_buff, "=== BPWR page 18 ===\nCrank_evt_cnt: %u\nCrank_tick: %u\nCrank_period: %u\nCrank_acc_torque: %u\r\n",
            p_profile->BPWR_PROFILE_crank_update_event_count, p_profile->BPWR_PROFILE_crank_tick, 
            p_profile->BPWR_PROFILE_crank_period, p_profile->BPWR_PROFILE_crank_accumulated_torque);
    app_usb_send(m_tx_buff);

    NRF_LOG_DEBUG("Page 18 BPWR was updated");
    break;
  }
  case ANT_BPWR_PAGE_80_UPDATED:
  {
    sprintf(m_tx_buff, "=== BPWR page 80 ===\nManuf_id: %u\nHW_version: %u\nmodel_number: %u\r\n",
            p_profile->BPWR_PROFILE_manufacturer_id, p_profile->BPWR_PROFILE_hw_revision, 
            p_profile->BPWR_PROFILE_model_number);
    app_usb_send(m_tx_buff);

    NRF_LOG_DEBUG("Page 80 BPWR was updated");
    break;
  }
  case ANT_BPWR_PAGE_81_UPDATED:
  {
    // data actualization
    sprintf(m_tx_buff, "=== BPWR page 81 ===\nsw_revision_minor: %u\nsw_revision_major: %u\nserial_number: %u\r\n",
            p_profile->BPWR_PROFILE_sw_revision_minor, p_profile->BPWR_PROFILE_sw_revision_major, 
            p_profile->BPWR_PROFILE_serial_number);
    app_usb_send(m_tx_buff);

    NRF_LOG_DEBUG("Page 81 BPWR was updated");
    break;
  }

  case ANT_BPWR_CALIB_TIMEOUT:
  {
    // calibration request time-out
    NRF_LOG_DEBUG("ANT_BPWR_CALIB_TIMEOUT");
    break;
  }

  case ANT_BPWR_CALIB_REQUEST_TX_FAILED:
  {
    // Please consider retrying the request.
    NRF_LOG_DEBUG("ANT_BPWR_CALIB_REQUEST_TX_FAILED");
    break;
  }

  default:
    // never occurred
    break;
  }
}

static void m_ant_bsc_evt_handler(ant_bsc_profile_t *p_profile, ant_bsc_evt_t event)
{
  switch (event)
  {
  case ANT_BSC_PAGE_0_UPDATED:
  {
    sprintf(m_tx_buff, "=== BSC page 0 ===\nevent_time: %u\nrev_count: %u\r\n",
            p_profile->BSC_PROFILE_event_time, p_profile->BSC_PROFILE_rev_count);
    app_usb_send(m_tx_buff);

    NRF_LOG_DEBUG("Page 81 BSC was updated");
    break;
  }
  case ANT_BSC_PAGE_1_UPDATED:
  {
    sprintf(m_tx_buff, "=== BSC page 1 ===\noperating_time: %u\r\n",
            p_profile->BSC_PROFILE_operating_time);
    app_usb_send(m_tx_buff);

    NRF_LOG_DEBUG("Page 1 BSC was updated");
    break;
  }
  case ANT_BSC_PAGE_2_UPDATED:
  {
    sprintf(m_tx_buff, "=== BSC page 2 ===\nmanuf_id: %u\nserial_num: %u\r\n",
            p_profile->BSC_PROFILE_operating_time, p_profile->BSC_PROFILE_serial_num);
    app_usb_send(m_tx_buff);

    NRF_LOG_DEBUG("Page 2 BSC was updated");
    break;
  }
  case ANT_BSC_PAGE_3_UPDATED:
  {
    sprintf(m_tx_buff, "=== BSC page 3 ===\nhw_version: %u\nsw_version: %u\nmodel_num: %u\r\n",
            p_profile->BSC_PROFILE_hw_version, p_profile->BSC_PROFILE_sw_version, p_profile->BSC_PROFILE_model_num);
    app_usb_send(m_tx_buff);

    NRF_LOG_DEBUG("Page 3 BSC was updated");
    break;
  }
  case ANT_BSC_PAGE_4_UPDATED:
  {
    sprintf(m_tx_buff, "=== BSC page 4 ===\nfract_bat_volt: %u\ncoarse_bat_volt: %u\nbat_status: %u\r\n",
            p_profile->BSC_PROFILE_fract_bat_volt, p_profile->BSC_PROFILE_coarse_bat_volt, p_profile->BSC_PROFILE_bat_status);
    app_usb_send(m_tx_buff);

    NRF_LOG_DEBUG("Page 4 BSC was updated");
    break;
  }
  case ANT_BSC_PAGE_5_UPDATED:
    /* Log computed value */

    if (DISPLAY_TYPE == BSC_SPEED_DEVICE_TYPE)
    {
      NRF_LOG_INFO("Computed speed value:                 %u kph",
                   (unsigned int)m_calculate_speed(p_profile->BSC_PROFILE_rev_count,
                                                   p_profile->BSC_PROFILE_event_time));
    }
    else if (DISPLAY_TYPE == BSC_CADENCE_DEVICE_TYPE)
    {
      NRF_LOG_INFO("Computed cadence value:               %u rpm",
                   (unsigned int)m_calculate_cadence(p_profile->BSC_PROFILE_rev_count,
                                                     p_profile->BSC_PROFILE_event_time));
    }
    break;

  case ANT_BSC_COMB_PAGE_0_UPDATED:
  {
    uint32_t speed = m_calculate_speed(p_profile->BSC_PROFILE_speed_rev_count,
                                       p_profile->BSC_PROFILE_speed_event_time);

    uint32_t cadence = m_calculate_cadence(p_profile->BSC_PROFILE_cadence_rev_count,
                                          p_profile->BSC_PROFILE_cadence_event_time);

    NRF_LOG_INFO("Computed speed value:                         %u kph", speed);
    NRF_LOG_INFO("Computed cadence value:                       %u rpms",cadence);

    sprintf(m_tx_buff, "=== BSC page 0 ===\nSpeed: %u kph\nCandence: %u rpm\r\n", speed, cadence);
    app_usb_send(m_tx_buff);
    
    break;
  }
  default:
    break;
  }
}

static uint32_t m_calculate_speed(int32_t rev_cnt, int32_t evt_time)
{
  static uint32_t computed_speed = 0;

  if (rev_cnt != m_speed_calc_data.prev_rev_cnt)
  {
    m_speed_calc_data.acc_rev_cnt += rev_cnt - m_speed_calc_data.prev_rev_cnt;
    m_speed_calc_data.acc_evt_time += evt_time - m_speed_calc_data.prev_evt_time;

    /* Process rollover */
    if (m_speed_calc_data.prev_rev_cnt > rev_cnt)
    {
      m_speed_calc_data.acc_rev_cnt += UINT16_MAX + 1;
    }
    if (m_speed_calc_data.prev_evt_time > evt_time)
    {
      m_speed_calc_data.acc_evt_time += UINT16_MAX + 1;
    }

    m_speed_calc_data.prev_rev_cnt = rev_cnt;
    m_speed_calc_data.prev_evt_time = evt_time;

    computed_speed = SPEED_COEFFICIENT *
                     (m_speed_calc_data.acc_rev_cnt - m_speed_calc_data.prev_acc_rev_cnt) /
                     (m_speed_calc_data.acc_evt_time - m_speed_calc_data.prev_acc_evt_time);

    m_speed_calc_data.prev_acc_rev_cnt = m_speed_calc_data.acc_rev_cnt;
    m_speed_calc_data.prev_acc_evt_time = m_speed_calc_data.acc_evt_time;
  }

  return (uint32_t)computed_speed;
}

static uint32_t m_calculate_cadence(int32_t rev_cnt, int32_t evt_time)
{
  static uint32_t computed_cadence = 0;

  if (rev_cnt != m_cadence_calc_data.prev_rev_cnt)
  {
    m_cadence_calc_data.acc_rev_cnt += rev_cnt - m_cadence_calc_data.prev_rev_cnt;
    m_cadence_calc_data.acc_evt_time += evt_time - m_cadence_calc_data.prev_evt_time;

    /* Process rollover */
    if (m_cadence_calc_data.prev_rev_cnt > rev_cnt)
    {
      m_cadence_calc_data.acc_rev_cnt += UINT16_MAX + 1;
    }
    if (m_cadence_calc_data.prev_evt_time > evt_time)
    {
      m_cadence_calc_data.acc_evt_time += UINT16_MAX + 1;
    }

    m_cadence_calc_data.prev_rev_cnt = rev_cnt;
    m_cadence_calc_data.prev_evt_time = evt_time;

    computed_cadence = CADENCE_COEFFICIENT *
                       (m_cadence_calc_data.acc_rev_cnt - m_cadence_calc_data.prev_acc_rev_cnt) /
                       (m_cadence_calc_data.acc_evt_time - m_cadence_calc_data.prev_acc_evt_time);

    m_cadence_calc_data.prev_acc_rev_cnt = m_cadence_calc_data.acc_rev_cnt;
    m_cadence_calc_data.prev_acc_evt_time = m_cadence_calc_data.acc_evt_time;
  }

  return (uint32_t)computed_cadence;
}

/* Display event handler----------------------------------------------- */
static void m_ant_hrm_disp_evt_handler(ant_evt_t *p_ant_evt, void *p_context)
{
  ant_hrm_profile_t *p_profile = (ant_hrm_profile_t *)p_context;

  memcpy((void *)&ant_data, p_ant_evt, sizeof(ant_data));

  if (p_ant_evt->channel == p_profile->channel_number)
  {
    switch (p_ant_evt->event)
    {
    case EVENT_RX:
      if (p_ant_evt->message.ANT_MESSAGE_ucMesgID == MESG_BROADCAST_DATA_ID ||
          p_ant_evt->message.ANT_MESSAGE_ucMesgID == MESG_ACKNOWLEDGED_DATA_ID ||
          p_ant_evt->message.ANT_MESSAGE_ucMesgID == MESG_BURST_DATA_ID)
      {
        // disp_message_decode(p_profile, p_ant_evt->message.ANT_MESSAGE_aucPayload);

        // Forward heart rate msg
        bsp_usb_ant_send_broadcast_data(p_ant_evt->message.ANT_MESSAGE_aucMesgData, 9);
      }
      break;

    default:
      break;
    }
  }
}

/* End of file -------------------------------------------------------- */
