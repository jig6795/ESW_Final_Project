#ifndef DHT22_H
#define DHT22_H

void sigalarm_handler(int sig);
void *send_temp(void *arg);
void read_dht_data();

#endif
