# Water-Monitoring Project Overview 

Water Quality Monitoring System - Raspberry Pi Client Project Overview This project aims to use a Raspberry Pi 5B to connect water quality sensors (pH sensor, turbidity sensor, and temperature sensor), collect sensor data, and send the data in JSON format to a Qt server via Socket communication. The project is implemented in three phases:
Sensor Data Reading and Printing: Read sensor data and print it to the terminal.
Socket Communication: Send sensor data to the Qt server via TCP Socket.
JSON Data Packaging: Use the cJSON library to package sensor data into JSON format and send it.

# Our social media accounts:
https://www.youtube.com/watch?v=Ooxhw4W-yWs

# Introduction

For the convenience of debugging the program, the first thread is enabled to transmit to the TFT screen display through multithreading, the second thread is used to output sensor data on the console, and the third thread is sent to the Qt server through a JSON string.
Firstly, install the Raspberry Pi system, burn the system onto the TF card using the official programmer, and then start the Raspberry Pi. Need to enable I2C, SPI, SSH, etc

DS18B20 is a temperature detection device that reads temperature data through a single bus protocol, connects the DIO pin to GPO4, and then reads/sys/bus/w1/devices/28-000000579aa1/w1_Slave. The content of the device is calculated to obtain the temperature value.

The turbidity sensor needs to undergo AD conversion to read the channel value of PCF8591 and calculate it. PCF8591 uses I2C protocol and can check whether the device address is connected through commands.

The pH sensor also needs to be converted through AD conversion, and then the channel value of PCF8591 is read. After calculation, the value needs to be noted that the pH is alkaline, with a maximum of 14.

The TFT screen display adopts the SPI protocol. In order to make the font display smooth, the FreeType library is used. After initializing the device, the data is directly sent to the screen, but the screen needs to be cleared, otherwise the sensor's updated content will overlap on the display screen.

Raspberry Pi connects to Qt as a client, initializes the socket, and waits for the server to start before connecting.

The server is developed using the Qt framework, with version QT5. The socket is initialized first, and then the client is waited for to connect. After connecting, parse the JSON string sent by Raspberry Pi and display it on the interface.

We need to use a static IP address here. We need to change the system's DHCP to manual allocation, leave the gateway and mask unchanged, fill in the default IP address, and then fill in the specified IP address.


# Overall picture of project
![50e4e01c9ffe5677b40e1786a53f933(1)](https://github.com/user-attachments/assets/d5941572-d77e-4d71-8b64-4c6b96b15a7e)
# Components of project 
The water quality detection system adopts Raspberry Pi 5, which functions to detect temperature, turbidity, pH value, and display them through TFT screen.
Hardware Requirements Raspberry Pi 5B
PCF8591 Module: Used to convert analog signals to digital signals
Jumper Wires, Breadboard, Resistor (4.7kΩ)
pH Sensor: Measures the acidity/alkalinity of water
![c6e5329ed70385d48ee4542adf73e48](https://github.com/user-attachments/assets/437c451b-59bb-4119-8ec6-2daaf720ab05)
Fig1
Turbidity Sensor: Measures the turbidity of water
![02fbbc3e89fd4bab5a81e7e90fad540](https://github.com/user-attachments/assets/b61f7d3f-282b-4bd9-97ba-4c1093b6191d)
Temperature Sensor (DS18B20): Measures water temperature
![1f49b684e95e9e0b664cfd3a9871602](https://github.com/user-attachments/assets/d508750d-787a-474a-b918-28c4fbd435f7)
PCF8591：
![19729c9f3a2c1abfaade235661d40ab](https://github.com/user-attachments/assets/19c434bb-860c-401d-890d-76d1b494f9bb)

# Operating principle and realisation logic of the sensor 

1. DS18B20 (temperature sensor) 
DS18B20 uses a single bus (1-Wire) protocol to communicate with the main control and can read data directly through the GPIO port. Its internal temperature sensor and digital conversion module are integrated, and the measurement resolution is 9~12 bits, default 12 bits. After the Raspberry Pi is configured with /sys/bus/w1/devices, read the raw data in the w1_slave file, extract the value after t= and divide by 1000 to get the Celsius temperature.

2. E201-C BNC (pH electrode) 
The E201-C is a glass electrode type sensor that works based on the potentiometric principle, where the electrode produces different voltage outputs (approximately -414mV to +414mV, corresponding to pH 0~14) in different pH solutions. Since the output is an analogue signal, it needs to be converted to a digital value by an ADC module such as PCF8591, and then the corresponding pH value is calculated by calibration. 

3. SKU:SEN0189 (turbidity sensor) 
SEN0189 works on the principle of infrared transmitted light method, when the more suspended solids in the water, the lower the transmittance, the lower the analogue voltage output of the sensor. Need to access the AD conversion module (PCF8591), the voltage value is converted to digital, through the table or linear fitting model converted to NTU turbidity units. 

# Process Chart
![flow chart](https://github.com/user-attachments/assets/297fcfe7-f17d-4489-b724-0cd23fd9fe1a)


# Team Contributions

Haoyang Zhang
Student number: 2960798Z
Contribution:


Ruiyang Chen
Student number: 2960734C
Contribution:



Binyu Wang
Student number: 2961262W
Contribution:


Junhao Shen
Student number: 2963364S
Contribution:

Jiayi Tang
Student number: 2961024T
Contribution:








