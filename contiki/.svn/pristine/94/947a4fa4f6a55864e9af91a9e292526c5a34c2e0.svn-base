#include "dev/socket-sensor.h"

const struct sensors_sensor socket_sensor;

/*---------------------------------------------------------------------------*/
void
socket_sensor_init(uint8_t * port)
{
  int p = 65000;
  struct sockaddr_in sock;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 1;

  /*  Create Socket */
  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd < 0)
    perror("Failed to create Sensor Socket");
  /* Set socket read timeout */  
  if (setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
    perror("socket timeout failed");  // Set recv() timeout


  memset(&sock, 0, sizeof(struct sockaddr_in)); 
  sock.sin_family = AF_INET;
  sock.sin_addr.s_addr = INADDR_ANY;
  sock.sin_port = htons(p);

  /* Connect to Host OS port */
  while (bind(sfd, (struct sockaddr *) &sock, sizeof(struct sockaddr_in)) < 0 && p < 65100) {
    p++;
    sock.sin_port = htons(p);
  }
}
/*---------------------------------------------------------------------------*/
void
socket_sensor_respond(int active)
{
  struct sockaddr_in tuisock;
  int tuisfd;
  tuisfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (tuisfd < 0)
    perror("Failed to create socket");

  memset(&tuisock, 0, sizeof(struct sockaddr_in));
  tuisock.sin_family = AF_INET;
  tuisock.sin_port = htons(6565);

  int bytesWritten;
  if (active == 1) {
    bytesWritten = sendto(tuisfd, "1", 1, 0, (struct sockaddr *) &tuisock, sizeof(struct sockaddr_in));
  }
  else {
    bytesWritten = sendto(tuisfd, "0", 1, 0, (struct sockaddr *) &tuisock, sizeof(struct sockaddr_in));
  }
  printf("Wrote %d bytes to sensor\n", bytesWritten);
}
/*---------------------------------------------------------------------------*/
static char 
value(int type)
{
  ssize_t numBytes;
  char buf;
  socklen_t len;
  uint8_t layer; 

  /* Read value of socket */
  len = sizeof(struct sockaddr_in);
  numBytes = read(sfd, &buf, 1);

  /* Return as query */
  if (buf == 'Q') {
    return -2;
  }
  else if (buf == 'T') {
    return -3;
  }

  layer = atoi(&buf); 
  if (numBytes == 1) {
    /* Return as layer assignment */
    if (layer) {
      return layer;
    } 
    /* Return as sensor input */
    else {
      if (buf == '1')
        return 1;
      return 0;
    }
  }
  return -1;
}

/*---------------------------------------------------------------------------*/
static int
configure(int type, uint8_t * port)
{
  switch(type){
    case SENSORS_HW_INIT:
      return 1;
    case SENSORS_ACTIVE:
      return 1;
  }

  return 0;
}
/*---------------------------------------------------------------------------*/
static void *
status(int type)
{
  switch(type) {
    case SENSORS_READY:
      return 1;
  }

  return 0;
}
/*---------------------------------------------------------------------------*/
void
socket_sensor_changed(void)
{
  /* Doesn't work for some reason */
  //sensors_changed(&socket_sensor);
  perror("Sensor changed!");
}
/*---------------------------------------------------------------------------*/

SENSORS_SENSOR(socket_sensor, SOCKET_SENSOR,
	       value, configure, status);

