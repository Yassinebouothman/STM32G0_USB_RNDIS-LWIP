/**
  * @file		tcp_server.h
  * @author		Comelit Group S.p.A.
  * @author     Houssemeddine Ben Mbarek (houssemeddine.benmbarek@comelit.it)
  * @brief          : Header for tcp_server.c file.
  *                   This file contains the common defines of the application.
  * @date		01/01/2023
  * @copyright	Comelit Group S.p.A.
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbd_cdc_rndis_if.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
//#include "sh_msg.h"

#if LWIP_TCP

/* structure for maintaining connection infos to be passed as argument
   to LwIP callbacks*/
struct tcp_server_struct
{
	u8_t state;             /* current connection state */
	u8_t retries;
	struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
	struct pbuf *p;         /* pointer on the received/to be transmitted pbuf */
};

/* TCP Port */
#define TCP_PORT		10011

void tcp_server_init(void);
void tcp_server_send_msg(uint8_t *msg);

#endif /* LWIP_TCP */
#endif /* _TCP_SERVER_H */
