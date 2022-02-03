/**
* @file       app_usb.h
* @copyright  Copyright (C) 2020 Fiot Co., Ltd. All rights reserved.
* @license    This project is released under the Fiot License.
* @version    1.0.0
* @date       2021-07-08
* @author     Hieu Doan
*             
* @brief      App USB
*/

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __APP_USB_H
#define __APP_USB_H

/* Includes ----------------------------------------------------------- */
#include "app_usbd.h"
#include "app_usbd_cdc_acm.h"

/* Public defines ----------------------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public enumerate/structure ----------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
int app_usb_init(void);
int app_usb_send(const char* data);

#endif // __APP_USB_H
/* End of file -------------------------------------------------------- */
