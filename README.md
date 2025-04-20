# Water-Monitoring Project Overview 

Water Quality Monitoring System - Raspberry Pi Client Project Overview This project aims to use a Raspberry Pi 5B to connect water quality sensors (pH sensor, turbidity sensor, and temperature sensor), collect sensor data, and send the data in JSON format to a Qt server via Socket communication. The project is implemented in three phases:
Sensor Data Reading and Printing: Read sensor data and print it to the terminal.
Socket Communication: Send sensor data to the Qt server via TCP Socket.
JSON Data Packaging: Use the cJSON library to package sensor data into JSON format and send it.

# Our social media accounts:

# Overall picture of project
![50e4e01c9ffe5677b40e1786a53f933(1)](https://github.com/user-attachments/assets/d5941572-d77e-4d71-8b64-4c6b96b15a7e)
# Components of project 
The water quality detection system adopts Raspberry Pi 5, which functions to detect temperature, turbidity, pH value, and display them through TFT screen.
Hardware Requirements Raspberry Pi 5B
PCF8591 Module: Used to convert analog signals to digital signals
Jumper Wires, Breadboard, Resistor (4.7kΩ)
pH Sensor: Measures the acidity/alkalinity of water
![c6e5329ed70385d48ee4542adf73e48](https://github.com/user-attachments/assets/437c451b-59bb-4119-8ec6-2daaf720ab05)
Turbidity Sensor: Measures the turbidity of water
![02fbbc3e89fd4bab5a81e7e90fad540](https://github.com/user-attachments/assets/b61f7d3f-282b-4bd9-97ba-4c1093b6191d)
Temperature Sensor (DS18B20): Measures water temperature
![1f49b684e95e9e0b664cfd3a9871602](https://github.com/user-attachments/assets/d508750d-787a-474a-b918-28c4fbd435f7)
