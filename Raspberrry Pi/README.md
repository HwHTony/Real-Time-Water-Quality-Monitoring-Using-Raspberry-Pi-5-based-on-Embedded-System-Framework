### Project Introduction
This project is a real - time water quality monitoring system based on Raspberry Pi 5, aiming to use the embedded system framework to monitor and display multiple water quality parameters in real - time. The system collects water quality data such as turbidity, pH value, and temperature through various connected sensors, and transmits this data via Socket communication. Additionally, it is equipped with a TFT display to visually present the monitored water quality parameters.

### Key Features
* Data Acquisition: The system uses the PCF8591 ADC to collect turbidity and pH value data, and the DS18B20 sensor to collect temperature data.
* Data Display: The TFT display shows the collected water quality parameters in real - time.
* Data Communication: It uses Socket communication to transmit the collected data to other devices.

### Hardware Dependencies
* Raspberry Pi 5: Serves as the core computing unit of the system.
* PCF8591 ADC: Converts analog signals to digital signals for collecting turbidity and pH value data.
* DS18B20 Temperature Sensor: Collects environmental temperature data.
* TFT Display: Displays water quality parameters in real - time.

### Software Dependencies
* FreeType Font Library: Used for text display on the screen.
* gpiod Library: Used for GPIO pin control.
* CMake: Used for project building and compilation.



### Compilation Rules
#### Environment Preparation
Before compiling the project, ensure that the following software is installed on the system:

* CMake: Version 4.0 or higher.
* GCC Compiler: Supports C and C++ compilation.
* FreeType Font Library: For text display.
* gpiod Library: For GPIO control.


#### Compilation Steps

1. Clone the Project Code

```
git clone https://github.com/HwHTony/Real-Time-Water-Quality-Monitoring-Using-Raspberry-Pi-5-based-on-Embedded-System-Framework.git
cd Real-Time-Water-Quality-Monitoring-Using-Raspberry-Pi-5-based-on-Embedded-System-Framework/raspberryPi
```

2. Create a Build Directory

```bash
mkdir build
cd build
```



3. Run CMake to Generate the Makefile

   * Compile the main program

     ```
     cmake -DBUILD_TESTS=OFF ..
     ```

   * Compile the test program

     ```
     cmake -DBUILD_TESTS=ON .. 
     ```

4. Compile the Project

```bash
make
```

#### Notes

* During the compilation process, you may need to adjust the CMake configuration according to your system environment, such as the compiler path and library file path.
* If you encounter compilation errors, check whether the system environment and dependent libraries are installed correctly.