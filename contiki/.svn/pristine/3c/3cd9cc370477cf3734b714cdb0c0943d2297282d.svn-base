#ifndef __SOCKET_SENSOR_H__
#define __SOCKET_SENSOR_H__

#include "lib/sensors.h"
#include "netinet/in.h"
#include "sys/socket.h"
#include "fcntl.h"

extern const struct sensors_sensor socket_sensor;

#define SOCKET_SENSOR "Socket"

int sfd;
void socket_sensor_changed(void);
void socket_sensor_respond(int active);

#endif /* __SOCKET_SENSOR_H__ */
