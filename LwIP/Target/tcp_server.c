/**
  * @file		tcp_server.c
  * @author		Comelit Group S.p.A.
  * @author     Houssemeddine Ben Mbarek (houssemeddine.benmbarek@comelit.it)
  * @brief		This file implements the TCP Server
  * @date		01/01/2023
  * @copyright	Comelit Group S.p.A.
  */
/* Includes ------------------------------------------------------------------*/
#include "tcp_server.h"

#if LWIP_TCP
//static uint8_t msg_rx_flag = 0;	//Received msg through TCP flag
struct tcp_pcb *tcp_server_pcb;
struct tcp_pcb *pcb;
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
//static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_server_error(void *arg, err_t err);
//static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);

/**
  * @brief  Initializes the tcp server
  * @param  None
  * @retval None
  */
void tcp_server_init(void)
{
	/* create new tcp pcb */
	tcp_server_pcb = tcp_new();
	
	if (tcp_server_pcb == NULL)
	{
		/* Nothing to do */
	}
	else
	{
		err_t err;
		/* bind tcp_pcb to TCP_PORT */
		err = tcp_bind(tcp_server_pcb, IP_ADDR_ANY, TCP_PORT);
		
		if (err == ERR_OK)
		{
			/* start tcp listening for pcb */
			tcp_server_pcb = tcp_listen(tcp_server_pcb);
			
			/* initialize LwIP tcp_accept callback function */
			tcp_accept(tcp_server_pcb, tcp_server_accept);
		}
		else
		{
			/* deallocate the pcb */
			tcp_close(tcp_server_pcb);
		}
	}
}

/**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @param  arg: not used
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used
  * @retval err_t: ERR_OK
  */
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);
	
	/* set priority for the newly accepted tcp connection newpcb */
	tcp_setprio(newpcb, TCP_PRIO_MIN);
	
	pcb = newpcb;
	tcp_arg(pcb, NULL);
	//tcp_recv(pcb, tcp_server_recv);
	tcp_err(pcb, tcp_server_error);
	return ERR_OK;
}

/**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  p: pointer on the received pbuf
  * @param  err: not used
  * @retval err_t: ERR_OK
  */
//static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
//{
//  struct tcp_server_struct *es;
//  err_t ret_err;
//  uint8_t rec_msg[SH_PAYLOAD_LEN];
//  
//  es = (struct tcp_server_struct *)arg;
//  
//  /* if we receive an empty tcp frame from client => close connection */
//  if (p == NULL)
//  {
//    /* remote host closed connection */
//    tcp_server_connection_close(tpcb, es);
//  }
//  else if (err != ERR_OK)
//  {
//    /* A non empty frame was received from client but for some reason err != ERR_OK */
//    
//    /* free received pbuf*/
//    if (p != NULL)
//    {
//      es->p = NULL;
//      pbuf_free(p);
//    }
//    
//    ret_err = err;
//  }
//  else
//  {
//    /* Set received msg through TCP flag */
//    msg_rx_flag = 1;
//    
//    /* Reset rec_msg buffer */
//    memset(rec_msg, 0x00, sizeof(rec_msg));
//    /* Copy only SH_PAYLOAD_LEN lenght from the packet */
//    memcpy(rec_msg, p->payload, SH_PAYLOAD_LEN);
//    
//    /* Bridge received msg from TCP port to SH Bus */
//    sh_bridge_msg(rec_msg);
//    
//    /* Send ACK Received packet */
//    tcp_recved(tpcb, p->tot_len);
//    
//    /* Free p buffer */
//    pbuf_free(p);
//    
//    ret_err = ERR_OK;
//  }
//  
//  return ret_err;
//}

/**
  * @brief  This function implements the tcp_err callback function (called
  *         when a fatal tcp_connection error occurs.
  * @param  arg: pointer on argument parameter
  * @param  err: not used
  * @retval None
  */
static void tcp_server_error(void *arg, err_t err)
{
	LWIP_UNUSED_ARG(err);
	
	static struct tcp_server_struct *es;
	pcb = NULL;
	
	es = (struct tcp_server_struct *)arg;
	
	if (es != NULL)
	{
		/*  free es structure */
		mem_free(es);
	}
}

//void tcp_server_send_msg(uint8_t *msg)
//{
//	if (pcb != NULL)
//	{
//		/* if received msg from TCP Client (not SH Bus), */
//		/* do not send it through TCP to avoid Echo packet */
//		if (msg_rx_flag)
//		{
//			msg_rx_flag = 0;
//		}
//		else
//		{
//			tcp_write(pcb, msg, SH_PAYLOAD_LEN, TCP_WRITE_FLAG_COPY);
//			tcp_output(pcb);
//		}
//	}
//	else
//	{
//		/* No TCP Communication ongoing, ignore send message */
//	}
//}

/**
  * @brief  This functions closes the tcp connection
  * @param  tcp_pcb: pointer on the tcp connection
  * @param  es: pointer on state structure
  * @retval None
  */
//static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
//{
//	/* remove all callbacks */
//	tcp_arg(tpcb, NULL);
//	tcp_sent(tpcb, NULL);
//	tcp_recv(tpcb, NULL);
//	tcp_err(tpcb, NULL);
//	tcp_poll(tpcb, NULL, 0);
//	
//	/* delete es structure */
//	if (es != NULL)
//	{
//		mem_free(es);
//	}
//	
//	/* close tcp connection */
//	tcp_close(tpcb);
//}

#endif /* LWIP_TCP */
