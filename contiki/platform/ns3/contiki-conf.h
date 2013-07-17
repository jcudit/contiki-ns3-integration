#ifndef __CONTIKI_CONF_H__
#define __CONTIKI_CONF_H__

#include <inttypes.h>

/* C Compiler Config */
#define CC_CONF_REGISTER_ARGS          1
#define CC_CONF_FUNCTION_POINTER_ARGS  1
#define CC_CONF_FASTCALL
#define CC_CONF_VA_ARGS                1
/*#define CC_CONF_INLINE                 inline*/
#define CCIF
#define CLIF
/* Not part of C99 but actually present */
int strcasecmp(const char*, const char*);

/* Type Definitions */
/* ---------------- */

typedef uint8_t   u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef  int32_t s32_t;
typedef unsigned short uip_stats_t;

/* Network Stack Config */

#define NETSTACK_CONF_NETWORK           sicslowpan_driver
#define NETSTACK_CONF_MAC               nullmac_driver
#define NETSTACK_CONF_RDC               sicslowmac_driver
#define NETSTACK_CONF_FRAMER            framer_802154
#define NETSTACK_CONF_RADIO		socketradio_driver


/* UIP Config */
/* ---------- */
#define UIP_CONF_UDP             	1
//#define UIP_CONF_MAX_CONNECTIONS 	40
//#define UIP_CONF_MAX_LISTENPORTS 	40
#define UIP_CONF_BUFFER_SIZE     	420
#define UIP_CONF_BYTE_ORDER      	UIP_LITTLE_ENDIAN
#define UIP_CONF_TCP             	1
#define UIP_CONF_TCP_SPLIT       	1
/* #define UIP_CONF_LOGGING         	1 */
#define UIP_CONF_UDP_CHECKSUMS   	1

#ifdef UIP_CONF_IPV6
#define UIP_CONF_IPV6_CHECKS     	1
#define UIP_CONF_IPV6_QUEUE_PKT  	1
#define UIP_CONF_IPV6_REASSEMBLY 	0
#define UIP_CONF_NETIF_MAX_ADDRESSES  	3
#define UIP_CONF_ND6_MAX_PREFIXES     	3
#define UIP_CONF_ND6_MAX_NEIGHBORS    	4
#define UIP_CONF_ND6_MAX_DEFROUTERS   	2
#define UIP_CONF_ICMP6           	1
#endif /* UIP_CONF_IPV6 */

/* #define UIP_CONF_IP_FORWARD                     1 */ 
//#define UIP_CONF_BUFFER_SIZE        	140
#define UIP_CONF_MAX_CONNECTIONS    	6
#define UIP_CONF_MAX_LISTENPORTS      	6
#define UIP_CONF_UDP_CONNS              3 
#define RIMEADDR_CONF_SIZE              8
#define UIP_CONF_LL_802154              1
#define IEEE802154_CONF_PANID           0x1234
#define UIP_CONF_LLH_LEN                0

/* SICSLOWPAN Config */
/* ------------ */

#define SICSLOWPAN_CONF_COMPRESSION_IPV6        0
#define SICSLOWPAN_CONF_COMPRESSION_HC1         1
#define SICSLOWPAN_CONF_COMPRESSION_HC06        2
#define SICSLOWPAN_CONF_COMPRESSION             SICSLOWPAN_CONF_COMPRESSION_HC06
#define SICSLOWPAN_CONF_FRAG                    1
#define SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS       2
#define SICSLOWPAN_CONF_MAXAGE                  2

/* Clock Config */
/* ------------ */

typedef unsigned long clock_time_t;

#define CLOCK_CONF_SECOND 1000

/* Logging  */
/* ------- */

#define LOG_CONF_ENABLED 1


#endif /* __CONTIKI_CONF_H__ */
