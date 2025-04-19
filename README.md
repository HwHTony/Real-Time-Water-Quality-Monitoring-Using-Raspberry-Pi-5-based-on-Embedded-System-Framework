# Water-Monitoring
Water Quality Monitoring System - Raspberry Pi Client Project Overview This project aims to use a Raspberry Pi 5B to connect water quality sensors (pH sensor, turbidity sensor, and temperature sensor), collect sensor data, and send the data in JSON format to a Qt server via Socket communication. The project is implemented in three phases:

Sensor Data Reading and Printing: Read sensor data and print it to the terminal.

Socket Communication: Send sensor data to the Qt server via TCP Socket.

JSON Data Packaging: Use the cJSON library to package sensor data into JSON format and send it.

Hardware Requirements Raspberry Pi 5B

pH Sensor: Measures the acidity/alkalinity of water.

Turbidity Sensor: Measures the turbidity of water.

Temperature Sensor (DS18B20): Measures water temperature.

PCF8591 Module: Used to convert analog signals to digital signals.

Jumper Wires, Breadboard, Resistor (4.7kΩ)

The water quality detection system adopts Raspberry Pi 5, which functions to detect temperature, turbidity, pH value, and display them through TFT screen.

For the convenience of debugging the program, the first thread is enabled to transmit to the TFT screen display through multithreading, the second thread is used to output sensor data on the console, and the third thread is sent to the Qt server through a JSON string.

Firstly, install the Raspberry Pi system, burn the system onto the TF card using the official programmer, and then start the Raspberry Pi. Need to enable I2C, SPI, SSH, etc

DS18B20 is a temperature detection device that reads temperature data through a single bus protocol, connects the DIO pin to GPO4, and then reads/sys/bus/w1/devices/28-000000579aa1/w1_Slave. The content of the device is calculated to obtain the temperature value

The turbidity sensor needs to undergo AD conversion to read the channel value of PCF8591 and calculate it. PCF8591 uses I2C protocol and can check whether the device address is connected through commands.

The pH sensor also needs to be converted through AD conversion, and then the channel value of PCF8591 is read. After calculation, the value needs to be noted that the pH is alkaline, with a maximum of 14

The TFT screen display adopts the SPI protocol. In order to make the font display smooth, the FreeType library is used. After initializing the device, the data is directly sent to the screen, but the screen needs to be cleared, otherwise the sensor's updated content will overlap on the display screen.

Raspberry Pi connects to Qt as a client, initializes the socket, and waits for the server to start before connecting

The server is developed using the Qt framework, with version QT5. The socket is initialized first, and then the client is waited for to connect. After connecting, parse the JSON string sent by Raspberry Pi and display it on the interface.

We need to use a static IP address here. We need to change the system's DHCP to manual allocation, leave the gateway and mask unchanged, fill in the default IP address, and then fill in the specified IP address.
