/**
  ******************************************************************************
  * @file    USB_Device/CDC_RNDIS_Server/USB_Device/App/usbd_desc.h
  * @author  MCD Application Team
  * @brief   Header for usbd_desc.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_DESC_H
#define __USBD_DESC_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define  USB_SIZ_STRING_SERIAL          0x1A
//#if defined (_5416)
//#define USBD_PID                      0x0E64U
//#elif defined (_5416MIS)
//#define USBD_PID                      0x1213U
//#elif defined (_9810)
//#define USBD_PID                      0x1214U
//#elif defined (_9816)
//#define USBD_PID                      0x1215U
//#else
//#error "One module ID should be defined"
//#endif

#define  USBD_PID                       0x5740                     
#define  USBD_VID                       0x483

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern USBD_DescriptorsTypeDef CDC_RNDIS_Desc;

#endif /* __USBD_DESC_H */

