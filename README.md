# NetAnalyzer
A network analyzing project making use of the MQTT protocol

## Description

This is a tool to measure network health/speed on a local scale, developed in order to test the dependability of microprocessors. Requires nodejs, python 3, arduino software, and two microprocessors.

## How To Use

You need preferably at least 2 microprocessors.

1) Install the arduino code into uPs. This project was made with Wemos D1 Mini therefore changes may be required for other uPs.
2) Connect the pin 13 of two devices, named D7 on d1 mini.
_Optional_ 2b) Connect 5v and ground between the uPs to be able to use a single microusb.
3) Start up the Broker-Py project.
4) Power uPs, they should start up as APs, connect to them and configure them. The inputs are:
MQTT_Server = IP of the device running the broker.
MQTT_Port = Port the broker is listening at.
MQTT_Role = Role of the uP, "T" is for transmitter, "R" for receiver.
MQTT_Ping/Report_Topic = Topics for pinging and reporting.
For most purposes it is enough to enter server ip and alternating roles on two uPs.
5) Start MQTT_Client.

A foo.csv file should be created with 3 columns, # of ping, and two delays. Second column is the delay between two uPs, second is the RTT between broker and uP.
