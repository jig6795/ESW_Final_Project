# Temperature and humidity detection program using dht22 sensor
Server, client temperature and humidity detection program using TCP / IP based Raspberry pi 3 board

## Introduce
In winter, respiratory diseases such as the flu are prevalent.

maintenance of proper temperature and humidity in the room is closely related to respiratory health.

Therefore, we developed an alarm system to maintain proper indoor temperature and humidity.

## Development environment and required sensors
### Development environment
server was developed with Macbook pro Mojave 10.14.1 and 

client development environment was developed in Raspbian Stretch with desktop OS released on 2018-11-13 based on Raspberrypi 3

### required sensors
Temperature and humidity sensors use the dht22 sensor from adafruit.

Since the development language uses c language, in order to control GPIO using c language in raspberry pi3, the wiringpi library must be installed in advance

## Function Available
The following functions are available in the program.

<img width="500" alt="2018-12-09 3 22 10" src="https://user-images.githubusercontent.com/44190882/49694082-8c418280-fbc6-11e8-8c81-884c7dbbb15a.png">

## How to use
Each Makefile is configured in the server and client directories.

Execute each Makefile with "make" command.

After the executable is configured, 

the server and client will ask for each argument when the executable is executed.

### server
./server_running port_number
### client
./client server_IP_address server_port_number nick_name

There is no limit to the number of users of the client, but the raspberry pi3 measuring the temperature and humidity is configured to have a maximum of 10.
