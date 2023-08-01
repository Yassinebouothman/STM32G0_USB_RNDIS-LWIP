/**
  ******************************************************************************
  * @file    USB_Device/CDC_RNDIS_Server/USB_Device/App/usbd_cdc_rndis_interface.h
  * @author  MCD Application Team
  * @brief   Header for usbd_cdc_rndis_interface.c file.
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
#ifndef __USBD_CDC_RNDIS_IF_H
#define __USBD_CDC_RNDIS_IF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_rndis.h"
#include <stdbool.h>
#include "dhserver.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "usbd_desc.h"
//#include "cmp_app.h"
/* Exported types ------------------------------------------------------------*/
err_t rndis_ethernetif_init(struct netif *netif);
void rndis_ethernetif_input(struct netif *netif);
void rndis_ethernetif_update_config(struct netif *netif);
/* Exported constants --------------------------------------------------------*/
/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0                                192U
#define IP_ADDR1                                168U
#define IP_ADDR2                                1U
#define IP_ADDR3                                10U

/*NETMASK*/
#define NETMASK_ADDR0                           255U
#define NETMASK_ADDR1                           255U
#define NETMASK_ADDR2                           255U
#define NETMASK_ADDR3                           0U

/*Gateway Address*/
#define GW_ADDR0                                192U
#define GW_ADDR1                                168U
#define GW_ADDR2                                1U
#define GW_ADDR3                                1U

/* Ensure this MAC address value is same as MAC_ADDRx declared in STM32xxx_conf.h */
//#define CDC_RNDIS_MAC_STR_DESC                 (uint8_t *)"000202030000"
#define CDC_RNDIS_MAC_STR_DESC                 (uint8_t *)"0289846A96AB"
#define CDC_RNDIS_MAC_ADDR0                    0x02 /* 01 */
#define CDC_RNDIS_MAC_ADDR1                    0x89 /* 02 */
#define CDC_RNDIS_MAC_ADDR2                    0x84 /* 03 */
#define CDC_RNDIS_MAC_ADDR3                    0x6A /* 00 */
#define CDC_RNDIS_MAC_ADDR4                    0x96 /* 00 */
#define CDC_RNDIS_MAC_ADDR5                    0xAB /* 00 */


//#define CDC_RNDIS_MAC_STR_DESC                 (uint8_t *)"0289846A96AB"
#define RNDIS_HWADDR     0x02,0x89,0x84,0x6A,0x96,0xAB	/* MAC-address to set to client host interface */
#define RNDIS_MTU        				1500   	/* MTU value */
#define USBD_CDC_RNDIS_VENDOR_DESC      "Comelit"
#define USBD_CDC_RNDIS_LINK_SPEED       120000U /* Link baudrate (12Mbit/s for USB-FS) */
#define USBD_CDC_RNDIS_VID              USBD_VID

#define STM32_UID_ADDR ((uint32_t*)0x1FFF7590)

/* Max Number of Trials waiting for Tx ready */
#define CDC_RNDIS_MAX_TX_WAIT_TRIALS           200U /* 200 ms */

/* RNDIS Maximum Segment size, typically 1514 bytes */
#define CDC_RNDIS_ETH_MAX_SEGSZE                            1514U

#define CDC_RNDIS_CONNECT_SPEED_UPSTREAM                    0x1E000000U
#define CDC_RNDIS_CONNECT_SPEED_DOWNSTREAM                  0x1E000000U

extern USBD_CDC_RNDIS_ItfTypeDef                    USBD_CDC_RNDIS_fops;

void DHCP_Process(struct netif *netif);
/* Exported macro ------------------------------------------------------------*/
#define PADDR(ptr) ((ip_addr_t *)ptr)
#define NUM_DHCP_ENTRY 1

#define DHCP_DEFAULT_IP	{IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3+1}
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CDC_RNDIS_IF_H */

