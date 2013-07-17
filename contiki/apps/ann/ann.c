#include "ann.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",lladdr->addr[0], lladdr->addr[1], lladdr->addr[2], lladdr->addr[3],lladdr->addr[4], lladdr->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#endif

#define NTABLE_SIZE 2 
#define ACTIVATION_THRESHOLD 0.8
#define ACTIVE0 -1.0
#define ACTIVE1 1.0
#define ACTIVEADJACENT0 0.0
#define ACTIVEADJACENT1 0.5
#define INHIBITORY0 1.0
#define INHIBITORY1 -1.0
#define ACTIVE_WEIGHT 1.0
#define ADJACENT_WEIGHT 0.5
#define BUFSIZE 6
#define UIP_IP_BUF                ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

  static struct etimer sensor_timer;
  static struct etimer training_timer;
  static struct uip_udp_conn *conn;
  float activation = 0;
  int layer = 1;
  int rxlayer, svalue, rxvalue, training, ncount, acount, adjcount;
  char txbuf[BUFSIZE];
  char rxbuf[BUFSIZE];

struct neighbor {
  struct neighbor * next;
  uint8_t addr[16];
  float weight;
  uint8_t active;
  uint8_t adjacent;
};

struct neighbor *s;

/* ---------------------------------------------------------------------------*/

PROCESS(ann_process, "ANN process");
AUTOSTART_PROCESSES(&ann_process);
LIST(neighbors);
MEMB(neighbor_mem, struct neighbor, NTABLE_SIZE);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ann_process, ev, data)
{
  PROCESS_BEGIN();
  SENSORS_ACTIVATE(socket_sensor);

  PRINTF("ANN Started\n");

  /* Connection initializaiton */
  conn = udp_broadcast_new(UIP_HTONS(65), NULL);
  /* Set timers */
  etimer_set(&sensor_timer, CLOCK_SECOND/2);

  while(1) {
    PROCESS_WAIT_EVENT();

/* -------------------- Sensor Input -------------------- */


    /* Node configuration, first layer input and last layer output via sensor socket */
    if (ev == PROCESS_EVENT_TIMER && etimer_expired(&sensor_timer))  {
      etimer_reset(&sensor_timer);
      /* Poll sensor every half second; react if necessary */
      svalue = socket_sensor.value(0);
      /* First-layer Feedforward Multicast */
      if (svalue == 1 || svalue == 0) {  
        tcpip_poll_udp(conn);
        PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event && uip_poll());
        sprintf(txbuf, "L%dV%d", layer, svalue); 
        //printf("Initial Feedforward Multicast: %s\n", txbuf);
        uip_send(txbuf, BUFSIZE);
       //socket_sensor_changed();
      }
      /* Query Received for Output Layer */
      else if (svalue == -2) {
        printf("%02x: Received Query with activation at %f, %d\n", uip_lladdr.addr[7], activation, (int)activation);
        /* Report state in response to query */
        if (activation >= ACTIVATION_THRESHOLD*(ACTIVE1 + INHIBITORY0 + ACTIVEADJACENT0)) {
          socket_sensor_respond(1);
        }
        else {
          socket_sensor_respond(0);
        }
      }
      /* Training Signal Received */
      else if (svalue == -3) {
        /* Input Layer Training */
        if (layer == 1) {
          /* Signal adjacency to next layer */
          tcpip_poll_udp(conn);
          PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event && uip_poll());
          sprintf(txbuf, "L%dVA", layer); 
          uip_send(txbuf, BUFSIZE);
        }
        /* Output Layer Training */
        else {
          training = 1; 
          adjcount = 0;
          for (s = list_head(neighbors); s != NULL; s = s->next) {
            /* Adjust all weights according to active input */
            if (s->active)
              s->weight = ((ACTIVE1 - ACTIVE0)/(float)acount);
          }
          /* Wait for reception of adjacent node traffic */
          etimer_set(&training_timer, CLOCK_SECOND*5);
        }
     }
      /* Display Neighbor List */
      else if (svalue == layer) {
        for (s = list_head(neighbors); s != NULL; s = s->next) {
          PRINT6ADDR(s->addr);
          printf("Active: %d Weight: %f\n", s->active, s->weight);
        }
      }
      /* Assign new layer value to node and discover input nodes */
      else if (svalue > 1) {
        /* Solicit new layer information */
        tcpip_poll_udp(conn);
        PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event && uip_poll());
        sprintf(txbuf, "L0VS"); 
        uip_send(txbuf, BUFSIZE);
        /* n-layer Initializtion */
        ncount = training = 0;
        activation = 0.0;
        layer = svalue;
        list_init(neighbors);
        memb_init(&neighbor_mem);
        printf("%02x: Layer changed to %d\n", uip_lladdr.addr[7], layer);
        /* Discover adjacent nodes */
        tcpip_poll_udp(conn);
        PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event && uip_poll());
        sprintf(txbuf, "L0VD"); 
        //printf("Discovery Request: %s\n", txbuf);
        uip_send(txbuf, BUFSIZE);
      }
    }

/* -------------------- Network Input -------------------- */

    /* n-layer input via adjacent layer nodes */
    if (uip_newdata()) {
      /* Determine adjacency of received data */
      memcpy(&rxbuf, uip_appdata, BUFSIZE);
      char rxl = rxbuf[1];
      rxlayer = atoi(&rxl);
      /* Check received value to determine type of transmission */
      if (rxlayer == layer - 1 || rxlayer == 0) { 
        char rxv = rxbuf[3];
        /* Adjacency Notification Received */
        if (rxv == 'A') {
          if (training) {
            /* Find sending node in list and mark as adjacent */
            for (s = list_head(neighbors); s != NULL; s = s->next) {
              if (!memcmp(s->addr, &UIP_IP_BUF->srcipaddr, 16)) {
                adjcount += 1;
                s->adjacent = 1;
              }
            }
            etimer_set(&training_timer, CLOCK_SECOND*3);
          }
        }
        /* Discovery Request Received */
        else if (rxv == 'D') {
          tcpip_poll_udp(conn);
          PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event && uip_poll());
          sprintf(txbuf, "L%dVR", layer); 
          //printf("%02x Advertising Layer: %s\n", uip_lladdr.addr[7], txbuf);
          uip_send(txbuf, BUFSIZE);
        }
        /* Removal Solicitation Received */
        else if (rxv == 'S') {
          for (s = list_head(neighbors); s != NULL; s = s->next) {
            if (!memcmp(s->addr, &UIP_IP_BUF->srcipaddr, 16)) {
              list_remove(neighbors, s);
            }
          } 
        }
        /* Discovery Response Received - Record node/weight */ 
        else if (rxv == 'R') {
          int exists = 0;
          for (s = list_head(neighbors); s != NULL; s = s->next) {
            if (!memcmp(s->addr, &UIP_IP_BUF->srcipaddr, 16)) { 
              exists = 1;
            }
          }
          if (!exists) { 
            struct neighbor * n;
            n = memb_alloc(&neighbor_mem);
            memcpy(n->addr, &UIP_IP_BUF->srcipaddr, 16); 
            n->weight = 0.0;
            n->active = 0;
            n->adjacent = 0;
            ncount += 1;
            list_add(neighbors, n);
          } 
        }
        /* Adjacent Layer Input */
        else if (!training) {
          /* Search neighbor table */
          for (s = list_head(neighbors); s != NULL; s = s->next) {
            if (!memcmp(s->addr, &UIP_IP_BUF->srcipaddr, 16)) 
              break;
          }
          /* Update activation energy */
          //printf("%02x/%d RECV: %s\n", uip_lladdr.addr[7], layer, rxbuf);
          rxvalue = atoi(&rxv);
          if (rxvalue != s->active) { 
            if (rxvalue) {
              activation += s->weight;
              acount += 1;
              s->active = 1;
            }
            else {
              activation -= s->weight;
              acount -= 1;
              s->active = 0;
            }
            /* Feedforward Propogation */
            if (activation >= 1) { 
              sprintf(txbuf, "L%dV1", layer, svalue); 
            }
            else {
              sprintf(txbuf, "L%dV0", layer, svalue); 
            }
            tcpip_poll_udp(conn);
            PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event && uip_poll());
            uip_send(txbuf, BUFSIZE);
          }
        }
      }
    }

/* ------------------------------------------------------- */

    /* Update Training State if necessary */
    if (training && etimer_expired(&training_timer)) {
      for (s = list_head(neighbors); s != NULL; s = s->next) {
        /* Adjacent cell weighting */
        if (s->adjacent) {
          s->weight = ((ACTIVEADJACENT1 - ACTIVEADJACENT0)/adjcount);
        }
        /* Inhibitory cell weighting */
        else {
          if (s->weight == 0) {
            s->weight = ((INHIBITORY1 - INHIBITORY0)/acount);
          }
        }
      }
      training = 0;
      activation = ACTIVE1 + INHIBITORY0 + ACTIVEADJACENT0;
      printf("%02x: Adjcount: %d Ncount: %d Acount: %d Adjacent Weights: %f Inhibitory Weights: %f Activation: %f Training Complete\n", 
        uip_lladdr.addr[7], adjcount, ncount, acount, 
        ((ACTIVEADJACENT1 - ACTIVEADJACENT0)/adjcount),
        ((INHIBITORY1 - INHIBITORY0)/acount), activation);
    }
  }

  PRINTF("ANN Finished\n");
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
