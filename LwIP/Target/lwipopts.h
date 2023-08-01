/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt
 *
 */
#ifndef LWIP_HDR_LWIPOPTS_H
#define LWIP_HDR_LWIPOPTS_H

//#include "dbg.h"

#ifdef NDEBUG
#define LWIP_NOASSERT
#endif

/* Prevent having to link sys_arch.c (we don't test the API layers in unit tests) */
#define NO_SYS                          1
#define MEM_ALIGNMENT                   4
#define SYS_LIGHTWEIGHT_PROT            0
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define LWIP_DNS						0

#define ARP_TABLE_SIZE                  50

/* All netif interface to the same pc. If we not disable this mode. lwip will send an arp request
   for each interface, and store the pc ip address foreach netif.
   This slows down UDP/CMP SCAN and cause multiple scan to be performed to see all devices.
 */
#define ETHARP_TABLE_MATCH_NETIF		0

//#define MEM_DEBUG						0x00//0x80
//#define PBUF_DEBUG 						0x00//0x80
//#define LWIP_DEBUG						1


/* Enable DHCP to test it, disable UDP checksum to easier inject packets */
#define LWIP_DHCP                       0
#define LWIP_UDP                        1
#define LWIP_TCP						1
#define LWIP_STATS 						1
#define LWIP_IP_ACCEPT_UDP_PORT(dst_port) ((dst_port) == PP_NTOHS(67))
/* Enable SNMP */
#define LWIP_SNMP 			0

/* Minimal changes to opt.h required for tcp unit tests: */
#define MEM_SIZE                        16384
#if LWIP_DNS
#define MEMP_NUM_UDP_PCB                5
#else
#define MEMP_NUM_UDP_PCB                6
#endif
#define TCP_SND_QUEUELEN                40
#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN
#define TCP_SND_BUF                     (12 * TCP_MSS)
#define TCP_WND                         (10 * TCP_MSS)
#define LWIP_WND_SCALE                  1
#define TCP_RCV_SCALE                   0
//#define PBUF_POOL_SIZE                  400 /* pbuf tests need ~200KByte */ //1 Giure deleted
#define PBUF_POOL_BUFSIZE 		1524

/* Enable IGMP and MDNS for MDNS tests */
#define LWIP_IGMP                       0
#define LWIP_MDNS_RESPONDER             0
#define LWIP_ICMP						0
#define LWIP_ICMP6						0
#define LWIP_NUM_NETIF_CLIENT_DATA      (LWIP_MDNS_RESPONDER)

/* Trick arp to simulate multiple hw interface */
#define LWIP_ARP_FILTER_NETIF	0

#define LWIP_NETIF_LINK_CALLBACK        1

/* Minimal changes to opt.h required for etharp unit tests: */
#define ETHARP_SUPPORT_STATIC_ENTRIES   1

//end added
#endif /* LWIP_HDR_LWIPOPTS_H */
