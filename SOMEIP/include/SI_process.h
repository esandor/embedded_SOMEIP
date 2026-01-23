// Include guard starts here
#ifndef SI_PROCESS_H_
#define SI_PROCESS_H_

/**
 * @file    SI_process.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Main modul of SOMEIP.
 *           Provides SOMEIP Rx handler function."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "SI_types.h"

#include "SomeIP_udp.h"
#include "SI_dispatcher.h"
#include "SI_parser.h"
#include "SI_header.h"
#include "SI_service_manager.h"
#include "SI_message.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_PROCESS_unicast(struct udp_pcb *rx_udp_pcb, struct pbuf *rx_pbuf, const ip_addr_t *src_addr, u16_t src_port);

// Include guard stops here
#endif /* SI_PROCESS_H_ */