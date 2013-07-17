/*! \file socketradio.h
 *  \brief Driver for the virtual radio used to communicate over an IPC socket to the ns-3 medium.  */

#ifndef SOCKETRADIO_H
#define SOCKETRADIO_H

#include "contiki.h"	/* Test this */
#include "contiki-net.h"  /* Test this */
#include "dev/radio.h"

/* For Socket IPC */
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef MACPHY_OVERLAY
/* Whether MACPHY Overlay mode is used or not (default: no; PHY Overlay) */
#define MACPHY_OVERLAY                 0
#endif 

PROCESS_NAME(socketradio_process);

/*! \fn static void pollhandler(void)
 *  \brief Action to take when the radio is polled.  The socket is inspected for incoming data an IO is performed if necessary */
static void pollhandler(void);

/*! \fn void socket_init(void)
 *  \brief Initializes the socket file descriptor for use by the Contiki node */
void socket_init();

/*! \fn void socket_exit(void)
 *  \brief Socket teardown to break and cleanup connection to ns-3 process  */
void socket_exit(void);

/*! \struct radio_driver socketradio_driver
 *  \brief The Socket Radio Driver structure. */
extern const struct radio_driver socketradio_driver;

/*! var int ns3sock
 *  \brief File descriptor for the IPC read/write socket to corresponding ns-3 node */
static unsigned int ns3sock;

/*! \fn static int init (void)
 *  \brief Starts the socketradio process responsible for ns-3 communication. */
static int init (void);

/*! \fn static int prepare (const void *payload, unsigned short payload_len)
 *  \brief Prepares data to be sent over the radio.  Does nothing. */
static int prepare(const void *payload, unsigned short payload_len);

/*! \fn static int transmit(unsigned short transmit_len)
 *  \brief Transmits data using radio.  Does nothing and returns successful acknowledgement. */
static int transmit(unsigned short transmit_len);

/*! \fn static int radio_send(const void *payload, unsigned short payload_len)  
 *  \brief Prepares and transmits data via the emulated radio.  Writes to socket to send data over IPC to ns-3. */
static int radio_send(const void *payload, unsigned short payload_len);

/*! \fn static int radio_read(void *buf, unsigned short buf_len)
 *  \brief Extracts data from the emulated radio.  Invokes a read socket call to pull from the ns-3 IPC socket. */
static int radio_read(void *buf, unsigned short buf_len);

/*! \fn static int channel_clear(void)
 *  \brief Checks if the channel is clear.  Does nothing *nd returns clear. */
static int channel_clear(void);

/*! \fn static int receiving_packet(void)
 *  \brief Checks if the channel is currently receiving a packet.  Does nothing and returns clear. */
static int receiving_packet(void);

/*! \fn static int pending_packet(void)
 *  \brief Checks the virtual radio hardware buffer if a packet is waiting.  Does nothing and returns clear.  */
static int pending_packet(void);

/*! \fn static int on(void)
 *  \brief Turns the virtual radio hardware on.  Does nothing and returns a successful acknowledgement.  */
static int on(void);

/*! \fn static int off(void)
 *  \brief Turns the virtual radio hardware off.  Does nothing and returns a seccessful acknowledgement.  */
static int off(void);

#endif /* SOCKETRADIO_H */
