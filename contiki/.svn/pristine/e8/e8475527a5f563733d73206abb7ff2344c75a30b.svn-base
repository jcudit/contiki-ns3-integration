#include "dev/socketradio.h"

/* Socket Operations */
#include <sys/un.h>
#include <sys/select.h>
//#include <stdio.h>
//#include <inttypes.h>


/* Contiki Process functions */

PROCESS(socketradio_process, "IPC Socket Radio");

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(socketradio_process, ev, data)
{ 
  PROCESS_POLLHANDLER(pollhandler());
  PROCESS_BEGIN();

  process_poll(&socketradio_process);

  PROCESS_WAIT_UNTIL(ev == PROCESS_EVENT_EXIT);

  socket_exit();

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

static void
pollhandler(void)
{
  process_poll(&socketradio_process);

  packetbuf_clear();

  int len;
  /* Check if there is incoming data via the network stack */
  len = radio_read(packetbuf_dataptr(), PACKETBUF_SIZE);
  if (len > 0) {
    packetbuf_set_datalen(len);
    NETSTACK_RDC.input();
  }
}

/*---------------------------------------------------------------------------*/

#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",lladdr->addr[0], lladdr->addr[1], lladdr->addr[2], lladdr->addr[3],lladdr->addr[4], lladdr->addr[5])

#define PING6_NB 5
#define PING6_DATALEN 16

#define UIP_IP_BUF                ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_ICMP_BUF            ((struct uip_icmp_hdr *)&uip_buf[uip_l2_l3_hdr_len])


void
socket_init()
{
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 1;

  ns3sock = dup(STDIN_FILENO); // Copy pre-created socket FD stored as Standard Input
  if (setsockopt(ns3sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
    perror("socket timeout failed");  // Set recv() timeout
  close(STDIN_FILENO);
}

/*---------------------------------------------------------------------------*/

void
socket_exit(void)
{
  close(ns3sock);
}

/*---------------------------------------------------------------------------*/

/* Radio functions */

/*---------------------------------------------------------------------------*/
static int
init(void)
{
  process_start(&socketradio_process, NULL);
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
prepare(const void *payload, unsigned short payload_len)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
transmit(unsigned short transmit_len)
{
  return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
static int
radio_send(const void *payload, unsigned short payload_len)
{
  prepare(payload, payload_len);
  /*  Write to socket using payload/payload_len.
   *  In PHY Overlay mode, payload contains the compressed IPv6 header and the payload. 
   *  In MACPHY Overlay mode, payload contains the layer 3/4 headers and the payload */
  if (write(ns3sock, payload, payload_len) < 0)
    perror("writing payload to socket failed");
  return transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
static int
radio_read(void *buf, unsigned short buf_len)
{
  static char newbuf[65535];
   /*  Read from socket  */
  int ret = read(ns3sock, &newbuf, 65535);
  if (ret > 0 && ret <= buf_len)
    memcpy(buf, newbuf, buf_len);
  return ret;
}
/*---------------------------------------------------------------------------*/
static int
channel_clear(void)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
receiving_packet(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
pending_packet(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
off(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
const struct radio_driver socketradio_driver =
  {
    init,
    prepare,
    transmit,
    radio_send,
    radio_read,
    channel_clear,
    receiving_packet,
    pending_packet,
    on,
    off,
  };
/*---------------------------------------------------------------------------*/
