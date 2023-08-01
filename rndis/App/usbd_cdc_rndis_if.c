/**
******************************************************************************
* @file    usbd_cdc_rndis_if.c
* @author  MCD Application Team
* @brief   Source file for USBD CDC_RNDIS interface template
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

/* Includes ------------------------------------------------------------------*/

/* Include TCP/IP stack header files */

#include "stm32g0xx_hal.h"
#include "main.h"
#include "usbd_cdc_rndis_if.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"
#include "tcp_server.h"
#include <intrinsics.h>
#include <assert.h>
#include <string.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Network interface name */
#define IFNAME0 'd'
#define IFNAME1 'm'
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t UserRxBuffer[CDC_RNDIS_ETH_MAX_SEGSZE + 100];
/* Received Data over USB are stored in this buffer */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif /* __ICCARM__ */
__ALIGN_BEGIN uint8_t UserRxBuffer[CDC_RNDIS_ETH_MAX_SEGSZE + 100] __ALIGN_END;

/* Transmitted Data over CDC_RNDIS (CDC_RNDIS interface) are stored in this buffer */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif /* __ICCARM__ */
__ALIGN_BEGIN static uint8_t UserTxBuffer[CDC_RNDIS_ETH_MAX_SEGSZE + 100] __ALIGN_END;

static uint8_t CDC_RNDISInitialized = 0U;

/* DHCP Variables initialization */
uint32_t DHCPfineTimer = 0;



typedef enum {DHCP_OFF, DHCP_START,
DHCP_WAIT_ADDRESS, DHCP_ADDRESS_ASSIGNED,
DHCP_TIMEOUT, DHCP_LINK_DOWN
} DHCP_State;

DHCP_State DHCP_state = DHCP_OFF;
#define MAX_DHCP_TRIES  4

static dhcp_entry_t entries[NUM_DHCP_ENTRY] =
{
  /* mac  unused	ip address        subnet mask        lease time */
  { {0},   {0}, DHCP_DEFAULT_IP, {255, 255, 255, 0}, 24 * 60 * 60 },
};

static dhcp_config_t dhcp_config =
{
  entries,               /* entries */
  "Digital module SHIO",/* dns suffix */
  {IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3},  /* server address */
  {IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3},    /* dns server */
  NUM_DHCP_ENTRY,       /* num entry */
  67		/* port */
};


/* USB handler declaration */
extern USBD_HandleTypeDef  USBD_Device;
extern struct netif gnetif;

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_RNDIS_Itf_Init(void);
static int8_t CDC_RNDIS_Itf_DeInit(void);
static int8_t CDC_RNDIS_Itf_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_RNDIS_Itf_Receive(uint8_t *pbuf, uint32_t *Len);
static int8_t CDC_RNDIS_Itf_TransmitCplt(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);
static int8_t CDC_RNDIS_Itf_Process(USBD_HandleTypeDef *pdev);
static void Netif_Config(void);
static void hwaddr_gen(struct netif *netif);
u32_t sys_now(void);

USBD_CDC_RNDIS_ItfTypeDef USBD_CDC_RNDIS_fops =
{
  CDC_RNDIS_Itf_Init,
  CDC_RNDIS_Itf_DeInit,
  CDC_RNDIS_Itf_Control,
  CDC_RNDIS_Itf_Receive,
  CDC_RNDIS_Itf_TransmitCplt,
  CDC_RNDIS_Itf_Process,
  (uint8_t *)CDC_RNDIS_MAC_STR_DESC,
};

/* Private functions ---------------------------------------------------------*/

/**
* @brief  CDC_RNDIS_Itf_Init
*         Initializes the CDC_RNDIS media low layer
* @param  None
* @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
*/
static int8_t CDC_RNDIS_Itf_Init(void)
{
  if (CDC_RNDISInitialized == 0U)
  {
    /* Initialize the LwIP stack */
    lwip_init();
    
    /* Configure the Network interface */
    Netif_Config();
    
    /* cmp Init */
    //cmp_app_init();
    
    /* TCP Server Init */
    tcp_server_init();
    
    CDC_RNDISInitialized = 1U;
  }
  
  /* Set Application Buffers */
#ifdef USE_USBD_COMPOSITE
  (void)USBD_CDC_RNDIS_SetTxBuffer(&USBD_Device, UserTxBuffer, 0U, 0U);
#else
  (void)USBD_CDC_RNDIS_SetTxBuffer(&USBD_Device, UserTxBuffer, 0U);
#endif /* USE_USBD_COMPOSITE */
  (void)USBD_CDC_RNDIS_SetRxBuffer(&USBD_Device, UserRxBuffer);
  
  return (0);
}

/**
* @brief  CDC_RNDIS_Itf_DeInit
*         DeInitializes the CDC_RNDIS media low layer
* @param  None
* @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
*/
static int8_t CDC_RNDIS_Itf_DeInit(void)
{
#ifdef USE_USBD_COMPOSITE
  USBD_CDC_RNDIS_HandleTypeDef *hcdc_cdc_rndis = (USBD_CDC_RNDIS_HandleTypeDef *)(
                                                                                  USBD_Device.pClassDataCmsit[USBD_Device.classId]);
#else
  USBD_CDC_RNDIS_HandleTypeDef *hcdc_cdc_rndis = (USBD_CDC_RNDIS_HandleTypeDef *)(
                                                                                  USBD_Device.pClassData);
#endif /* USE_USBD_COMPOSITE */
  
  /*
  Add your code here
  */
  
  /* Notify application layer that link is down */
  hcdc_cdc_rndis->LinkStatus = 0U;
  
  return (0);
}

/**
* @brief  CDC_RNDIS_Itf_Control
*         Manage the CDC_RNDIS class requests
* @param  Cmd: Command code
* @param  Buf: Buffer containing command data (request parameters)
* @param  Len: Number of data to be sent (in bytes)
* @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
*/
static int8_t CDC_RNDIS_Itf_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
#ifdef USE_USBD_COMPOSITE
  USBD_CDC_RNDIS_HandleTypeDef *hcdc_cdc_rndis = (USBD_CDC_RNDIS_HandleTypeDef *)(
                                                                                  USBD_Device.pClassDataCmsit[USBD_Device.classId]);
#else
  USBD_CDC_RNDIS_HandleTypeDef *hcdc_cdc_rndis = (USBD_CDC_RNDIS_HandleTypeDef *)(
                                                                                  USBD_Device.pClassData);
#endif /* USE_USBD_COMPOSITE */
  
  switch (cmd)
  {
  case CDC_RNDIS_SEND_ENCAPSULATED_COMMAND:
    /* Add your code here */
    break;
    
  case CDC_RNDIS_GET_ENCAPSULATED_RESPONSE:
    
    /* Check if this is the first time we enter */
    if (hcdc_cdc_rndis->LinkStatus == 0U)
    {
      /* Setup the Link up at TCP/IP stack level */
      hcdc_cdc_rndis->LinkStatus = 1U;
      /* Setup the Link up at LwIP level */
      netif_set_link_up(&gnetif);
      /*
      Add your code here
      */
    }
    
    /* Add your code here */
    break;
    
  default:
    /* Add your code here */
    break;
  }
  
  UNUSED(length);
  UNUSED(pbuf);
  
  return (0);
}

/**
* @brief  Setup the network interface
* @param  None
* @retval None
*/
static void Netif_Config(void)
{
  struct netif  *netif = &gnetif;
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gateway;
  
  /* Generate MAC Address*/
  hwaddr_gen(netif);
  
  IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
  IP_ADDR4(&gateway, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
  
  /* add the network interface */
  netif = netif_add(&gnetif, &ipaddr, &netmask, &gateway,
                    NULL, &rndis_ethernetif_init, &ethernet_input);
  
  assert(netif != 0);
  
  if (netif != 0)
  {
    netif_set_default(netif);
    
    if (dhserv_init(&dhcp_config) != ERR_OK)
    {
      assert(0);
      return;
    }
    
#if LWIP_DNS
    
    if (dnserv_init(PADDR(ipaddr), 53, dns_query_proc) != ERR_OK)
    {
      assert(0);
      return;
    }
    
#endif
  }
}

/**
* @brief  CDC_RNDIS_Itf_Receive
*         Data received over USB OUT endpoint are sent over CDC_RNDIS interface
*         through this function.
* @param  Buf: Buffer of data to be transmitted
* @param  Len: Number of data received (in bytes)
* @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
*/
static int8_t CDC_RNDIS_Itf_Receive(uint8_t *Buf, uint32_t *Len)
{
  /* Get the CDC_RNDIS handler pointer */
#ifdef USE_USBD_COMPOSITE
  USBD_CDC_RNDIS_HandleTypeDef *hcdc_cdc_rndis = (USBD_CDC_RNDIS_HandleTypeDef *)(
                                                                                  USBD_Device.pClassDataCmsit[hUsbDeviceFS.classId]);
#else
  USBD_CDC_RNDIS_HandleTypeDef *hcdc_cdc_rndis = (USBD_CDC_RNDIS_HandleTypeDef *)(
                                                                                  USBD_Device.pClassData);
#endif /* USE_USBD_COMPOSITE */
  
  /* Call Eth buffer processing */
  hcdc_cdc_rndis->RxState = 1U;
  
  UNUSED(Buf);
  UNUSED(Len);
  
  return (0);
}

/**
* @brief  CDC_RNDIS_Itf_TransmitCplt
*         Data transmitted callback
*
*         @note
*         This function is IN transfer complete callback used to inform user that
*         the submitted Data is successfully sent over USB.
*
* @param  Buf: Buffer of data to be received
* @param  Len: Number of data received (in bytes)
* @param  epnum: EP number
* @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
*/
static int8_t CDC_RNDIS_Itf_TransmitCplt(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  
  return (0);
}

/**
* @brief  CDC_RNDIS_Itf_Process
*         Data received over USB OUT endpoint are sent over CDC_RNDIS interface
*         through this function.
* @param  pdef: pointer to the USB Device Handle
* @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
*/
static int8_t CDC_RNDIS_Itf_Process(USBD_HandleTypeDef *pdev)
{
  USBD_CDC_RNDIS_HandleTypeDef *hcdc_cdc_rndis = (USBD_CDC_RNDIS_HandleTypeDef *)(pdev->pClassData);
  
  if (hcdc_cdc_rndis == NULL)
  {
    return (-1);
  }
  
  if (hcdc_cdc_rndis->LinkStatus != 0U)
  {
    /*
    Add your code here
    Read a received packet from the RNDIS buffers and send it
    to the lwIP for handling
    */
    
    /* Read a received packet from the RNDIS buffers and send it
    to the lwIP for handling */
    rndis_ethernetif_input(&gnetif);
    
    /* CMP RUN function */
    //cmp_app_run();
    
    /* Handle timeouts */
    sys_check_timeouts();
    
  }
  
  return (0);
}

#if LWIP_DNS
static bool dns_query_proc(const char *name, ip_addr_t *addr)
{
  if (strcmp(name, "porter.ultra") == 0 || strcmp(name, "www.porter.ultra") == 0)
  {
    addr->addr = *(uint32_t *)dhcp_config.addr;
    return true;
  }
  
  return false;
}
#endif

/*******************************************************************************
RNDIS MSP Routines
*******************************************************************************/

/*******************************************************************************
LL Driver Interface ( LwIP stack --> USB)
*******************************************************************************/
/**
* @brief In this function, the hardware should be initialized.
* Called from rndis_ethernetif_init().
*
* @param netif the already initialized lwip network interface structure
*        for this rndis_ethernetif
*/
static void low_level_init(struct netif *netif)
{
  netif->mtu = RNDIS_MTU; // maximum transfer unit
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  
  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an rndis one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_UP;
}

/**
* @brief This function should do the actual transmission of the packet. The packet is
* contained in the pbuf that is passed to the function. This pbuf
* might be chained.
*
* @param netif the lwip network interface structure for this rndis_ethernetif
* @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
* @return ERR_OK if the packet could be sent
*         an err_t value if the packet couldn't be sent
*
* @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
*       strange results. You might consider waiting for space in the DMA queue
*       to become available since the stack doesn't retry to send a packet
*       dropped because of memory failure (except for the TCP timers).
*/
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  uint8_t *pdata;
  uint32_t Trials = CDC_RNDIS_MAX_TX_WAIT_TRIALS;
  
  USBD_CDC_RNDIS_HandleTypeDef *hcdc = (USBD_CDC_RNDIS_HandleTypeDef *)(USBD_Device.pClassData);
  
  /* Check if the TX State is not busy */
  while ((hcdc->TxState != 0U) && (Trials > 0U))
  {
    Trials--;
    HAL_Delay(1);
  }
  
  /* If no success getting the TX state ready, return error */
  if (Trials == 0U)
  {
    return (err_t)ERR_USE;
  }
  
  pdata = hcdc->TxBuffer + sizeof(USBD_CDC_RNDIS_PacketMsgTypeDef);
  hcdc->TxLength = 0;
  
  for (q = p; q != NULL; q = q->next)
  {
    (void) memcpy(pdata, q->payload, q->len);
    pdata += q->len;
    hcdc->TxLength += q->len;
  }
  
  hcdc->TxLength += sizeof(USBD_CDC_RNDIS_PacketMsgTypeDef);
  
  /* Set the TX buffer information */
  (void)USBD_CDC_RNDIS_SetTxBuffer(&USBD_Device, hcdc->TxBuffer, hcdc->TxLength);
  
  /* Start transmitting the Tx buffer */
  if (USBD_CDC_RNDIS_TransmitPacket(&USBD_Device) == (uint8_t)USBD_OK)
  {
    return (err_t)ERR_OK;
  }
  
  UNUSED(netif);
  return (err_t)ERR_USE;
}

/**
* @brief Should allocate a pbuf and transfer the bytes of the incoming
*        packet from the interface into the pbuf.
*
* @param pnetif the lwip network interface structure for this rndis_ethernetif
* @return a pbuf filled with the received packet (including MAC header)
*         NULL on memory error
*/
static struct pbuf *low_level_input(struct netif *pnetif)
{
  struct pbuf *p = NULL;
  
  USBD_CDC_RNDIS_HandleTypeDef *hcdc = (USBD_CDC_RNDIS_HandleTypeDef *)(USBD_Device.pClassData);
  uint32_t temp = hcdc->RxState;
  
  /* Get the length of the current buffer */
  if ((hcdc->RxLength > 0U) && (temp == 1U))
  {
    /* Allocate a pbuf chain of pbufs from the Lwip buffer pool */
    p = pbuf_alloc(PBUF_RAW, (uint16_t)hcdc->RxLength, PBUF_POOL);
  }
  
  /* Check that allocation was done correctly */
  if (p != NULL)
  {
    /** @note
    * In case that the packet length exceeds the maximum size of the packet pool
    * (defined in PBUF_POOL_BUFSIZE) the copy proceess will be splited depends
    * on the total length "hcdc->RxLength" value.
    **/
    
    uint32_t len;					//Amount to be copied once
    int RxLength = hcdc->RxLength;	//remaining length
    uint32_t idx = 0;				//index of RxBuffer
    struct pbuf *p_work = p;		//save an image og the original Packet
    
    do
    {
      if (p_work == NULL)
      {
        UNUSED(pnetif);
        return p_work;
      }
      
      /* Calculate the payload length to be copied */
      len = (RxLength < p_work->len) ? RxLength : p_work->len;
      
      /* Copy an amount "len" of payload to RxBuffer in the index "idx" */
      (void) memcpy(p_work->payload, (uint8_t *)&(hcdc->RxBuffer[idx]), len);
      
      /* Point to the next packet */
      p_work = p_work->next;
      
      /* calculate the remaining packet lenght */
      RxLength -= len;
      
      /* Increment the RxBuffer index */
      idx += len;
    }
    while (RxLength > 0);
  }
  
  UNUSED(pnetif);
  return p;
}

/** Generate local mac address
* @netif interface to fille with mac
*/
static void hwaddr_gen(struct netif *netif)
{
  uint32_t uid;
  
  /* Use the unique stm32 device id */
  uid = *STM32_UID_ADDR;
  
  /* set MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;
  
  netif->hwaddr[0] = (0xA2 & 0xFE) | 0x02; // unicast local administrated bits
  netif->hwaddr[1] = 0xBB;
  netif->hwaddr[2] = uid & 0xFF;
  netif->hwaddr[3] = (uid >> 8) & 0xFF;
  netif->hwaddr[4] = (uid >> 16) & 0xFF;
  netif->hwaddr[5] = (uid >> 24) & 0xFF;
}

/**
* @brief This function should be called when a packet is ready to be read
* from the interface. It uses the function low_level_input() that
* should handle the actual reception of bytes from the network
* interface. Then the type of the received packet is determined and
* the appropriate input function is called.
*
* @param pnetif the lwip network interface structure for this rndis_ethernetif
*/
void rndis_ethernetif_input(struct netif *pnetif)
{
  err_t err;
  struct pbuf *p;
  
  USBD_CDC_RNDIS_HandleTypeDef *hcdc = (USBD_CDC_RNDIS_HandleTypeDef *)(USBD_Device.pClassData);
  
  /* move received packet into a new pbuf */
  p = low_level_input(pnetif);
  
  /* no packet could be read, silently ignore this */
  if (p == NULL)
  {
    return;
  }
  
  
  /* entry point to the LwIP stack */
  err = pnetif->input(p, pnetif);
  
  if (err != (err_t)ERR_OK)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("rndis_ethernetif_input: IP input error\n"));
    (void) pbuf_free(p);
    p = NULL;
  }
  
  /* Reset the Received buffer length to zero for next transfer */
  hcdc->RxLength = 0;
  hcdc->RxState = 0;
  
  /* Reset the Rx buffer pointer to origin */
  (void) USBD_CDC_RNDIS_SetRxBuffer(&USBD_Device, UserRxBuffer);
  /* Prepare Out endpoint to receive next packet in current/new frame */
  (void) USBD_LL_PrepareReceive(&USBD_Device,
                                CDC_RNDIS_OUT_EP,
                                (uint8_t *)(hcdc->RxBuffer),
                                (uint16_t)hcdc->MaxPcktLen);
   
   /* Free the allocated buffer :
   The allocated buffer is freed by low layer rndis functions.
   */
   UNUSED(p);
}

/**
* @brief Should be called at the beginning of the program to set up the
* network interface. It calls the function low_level_init() to do the
* actual setup of the hardware.
*
* This function should be passed as a parameter to netif_add().
*
* @param netif the lwip network interface structure for this rndis_ethernetif
* @return ERR_OK if the loopif is initialized
*         ERR_MEM if private data couldn't be allocated
*         any other err_t on error
*/
err_t rndis_ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  
  /* We directly use etharp_output() here to save a function call.
  * You can instead declare your own function an call etharp_output()
  * from it if you have to do some checks before sending (e.g. if link
  * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  
  /* initialize the hardware */
  low_level_init(netif);
  
  return (err_t)ERR_OK;
}

/**
* @brief  Returns the current time in milliseconds
*         when LWIP_TIMERS == 1 and NO_SYS == 1
* @param  None
* @retval Current Time value
*/
u32_t sys_now();

