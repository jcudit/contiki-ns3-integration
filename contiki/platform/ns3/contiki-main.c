#include <stdio.h>
#include <unistd.h>
#include "contiki.h"
#include "net/netstack.h"
#include "net/uip.h"
#include "dev/socketradio.h"
#include "dev/socket-sensor.h"

#include "net/rime/rimeaddr.h"
#include "net/rime.h"
#include "net/rime/rime-udp.h"

#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x ",lladdr.u8[0], lladdr.u8[1], lladdr.u8[2], lladdr.u8[3],lladdr.u8[4], lladdr.u8[5], lladdr.u8[6], lladdr.u8[7])

#define UIP_IP_BUF                ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_ICMP_BUF            ((struct uip_icmp_hdr *)&uip_buf[uip_l2_l3_hdr_len])


PROCINIT(&etimer_process, &tcpip_process);
SENSORS(&socket_sensor);

u8_t
convert(const char c, uint8_t lower)
{
  u8_t result;
  if (c > 0x39) {
    if (lower) {
     result = (c - 0x57); 
    }
    else {
     result = 0x10 * (c - 0x57);
    }
  }
  else {
    if (lower) {
      result = (c - 0x30);
    }
    else {
      result = 0x10 * (c - 0x30);
    }
  }
  return result;
}

/*  Sets uip_lladdr value so that uip can derive a full address from the link layer address */
void
assign_rimeaddr(const char * addr)
{
  uint8_t i,j = 0;
  union {
    u8_t u8[8];
  } lladdr;
 
  for (; i < 8; i++) {
    lladdr.u8[i] = 0;
    lladdr.u8[i] += convert(addr[j], 0);
    lladdr.u8[i] += convert(addr[j+1], 1);
    j += 3;
  }
  rimeaddr_set_node_addr((rimeaddr_t *) &lladdr);
  memcpy(&uip_lladdr.addr, &lladdr, sizeof(uip_lladdr.addr));
  PRINTLLADDR(lladdr);
}

/*---------------------------------------------------------------------------*/
int
main(int argc, char * argv[])
{
  /*  Make standard output unbuffered. */
  setvbuf(stdout, (char *)NULL, _IONBF, 0);

  printf("Starting Contiki with addresses:\n");

  char c;
  int port;

  /* Obtain networking information */
  while ((c = getopt (argc, argv, "a:")) != -1) {
    /* Assign Layer 2 Address */
    if (c == 'a') {
       assign_rimeaddr(optarg);
    }
    else if (c == 'p') {
       port = optarg; 
    }
  }

  /* Prepare process list */
  process_init();
  /* Prepares a list of timers; starts the ctimer process */
  ctimer_init();
  /* Run configured Radio, RDC, MAC and Network init functions */
  netstack_init();
  /* Start all processes listed in PROCINIT macro */ 
  procinit_init();
  /* Initialize socket to ns3 */
  socket_init();
  /*  Initialize sensor socket */
  socket_sensor_init(port);
  /* Run Autostart processes (Application Layer) */
  autostart_start(autostart_processes);
  while(1) {
    /* Run a single cycle of the scheduler (event and poll processing) */
    process_run();
    /* Synchronize etimer with real-time */
    etimer_request_poll();
  }
  
  return 0;
}
/*---------------------------------------------------------------------------*/
