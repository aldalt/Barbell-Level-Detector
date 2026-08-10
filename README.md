# Barbell-Level-Detector
An embedded device that measures barbell orientation using an MPU-6050 IMU and STM32 microcontroller and provides visual and auditory feedback when the bar is not level enough.

(INSERT PICTURE OF THE FINAL DEVICE HERE)

**FEATURES**

- Real-time barbell orientation measurement
- MPU-6050 IMU with accelerometer and gyroscope
- Madgwick sensor fusion algorithm
- OLED orientation display
- [IN PROGRESS] Audible level warning
- [IN PROGRESS] Push-button calibration
- STM32-based embedded firmware written in C

**DEMONSTRATION**

(INSERT DEMO VIDEO HERE)

**HARDWARE**
|COMPONENT|MODEL|PURPOSE|
|:----|:----|:----|
|Microcontroller|STM32-F446RE Development Board|Main Processing Unit|
|IMU|Adafruit MPU-6050 Breakout Board|Accelerometer and Gyroscope Measurements|
|Display|Adafruit 0.96" OLED Graphic Display|Displaying Status, Orientation and Level Information|
|Buzzer|[INSERT MODEL]|Auditory Feedback|
|Power Source|3xAA Battery Pack|Power for Device|
|Button|[INSERT MODEL]|Initiates Calibration|

For this project, a STM32 development board was used to ensure simple hardware configuration, as this was intended to serve as an introduction to the STM32. The STM32-F446RE board was used because it has plenty of I/O, including support for I2C, SPI, PWM, GPIO and more, and has plenty of memory and onboard flash storage for experimentation. 

The MPU=6050 IMU and the display module are connected to the STM32 board using I2C. Two separate I2C channels are used, for simplicity of wiring and straightforward debugging.

[ADD INFORMATION ABOUT BUTTON CONNECTION]

[ADD INFORMATION ABOUT BUZZER CONNECITON]

[ADD INFORMATION ABOUT BATTERY CONNECTION]

**SOFTWARE**

**SYSTEM ARCHITECTURE**

**HOW IT WORKS**

**CALIBRATION**

**WIRING/PINOUT**

**REPOSITORY STRUCTURE**

**BUILDING AND FLASHING**

**CURRENT STATUS**

**POTENTIAL FUTURE IMPROVEMENTS**

**AUTHOR**




