# Barbell-Level-Detector
An embedded device that measures barbell orientation using an MPU-6050 IMU and STM32 microcontroller and provides visual feedback when the bar is not level enough.

(INSERT PICTURE OF THE FINAL DEVICE HERE)


## FEATURES
- Real-time barbell orientation measurement
- MPU-6050 IMU with accelerometer and gyroscope
- Madgwick sensor fusion algorithm
- OLED orientation display
- [IN PROGRESS] Push-button calibration
- STM32-based embedded firmware written in C


## DEMONSTRATION
### VIDEO
(INSERT DEMO VIDEO HERE)


## HARDWARE
|COMPONENT|MODEL|PURPOSE|
|:----|:----|:----|
|Microcontroller|STM32-F446RE Development Board|Main Processing Unit|
|IMU|Adafruit MPU-6050 Breakout Board|Accelerometer and Gyroscope Measurements|
|Display|Adafruit 0.96" OLED Graphic Display|Displaying Status, Orientation and Level Information|
|Power Source|6xAAA Battery Pack|Power for Device|
|Button|Momentary Switch|Initiates Calibration|

For this project, a STM32 development board was used to ensure simple hardware configuration, as this was intended to serve as an introduction to the STM32. The STM32-F446RE board was used because it has plenty of I/O, including support for I2C, SPI, PWM, GPIO and more, and has plenty of memory and onboard flash storage for experimentation. 

The MPU-6050 IMU and the display module are connected to the STM32 board using I2C. Two separate I2C channels are used, for simplicity of wiring and straightforward debugging.

For the calibration button, a simple momentary switch was wired up to the STM32 using a GPIO input pin.

To power the device, a bank of 6 AAA batteries were wired in series, to provide 9V DC power to the device. Since the STM32 can accept a range of 7V-12V on its VIN pin, 9V provides sufficient power to fall within this range, while not being unnecessarily heavy. AAA batteries are compact, cheap and easy to swap, making them a reasonable choice.  


## SOFTWARE

Development Software Used: STM32CubeIDE, STM32CubeMX, STM32 HAL Library

Programming Language Used: C

Interfaces Used: I2C, Timers, PWM, GPIO

Algorithms Used: Madgwick Sensor Fusion Algorithm, IMU Calibration Algorithm

[ADD A BASIC EXPLANATION OF HOW THE SOFTWARE WORKS]


## SYSTEM ARCHITECTURE

[ADD A BLOCK DIAGRAM HERE]

[EXPLAIN THE BLOCK DIAGRAM HERE]


## HOW IT WORKS

### HOW THE IMU GATHERS DATA
The inertial measurement unit (IMU) measures acceleration and gyroscope (rotational velocity) data, storing each of these measured values in an 8-bit register. To retrieve this data, the STM32 sends a request over the I2C bus to the IMU's unique hardware address (0x68). In response, the IMU streams all 14 8-bit data measurements consecutively over the I2C bus in one burst, in a 14-byte buffer that is read in by the STM32. 

The single output stream from the IMU can then be separated into the x, y and z commponents of the acceleration and gyroscope, as well as the temperature (although the temperature is not used here). This data is then passed into the Madgwick algorithm to be filtered. 

### HOW THE MADGWICK ALGORITHM WORKS
The Madgwick Algorithm, invented by Sebastian Madgwick, is a sensor fusion technique that involves representing an object's orientation as a quaternion, which is a 4-dimensional math vector. One part represents scalar magnitude of an object's rotation, where the other three parts represent the vector components of the onject's angle of rotation. The gyroscope and accelerometer data are processed in distinct ways that are fused together at the end using gradient descent.

To process the gyroscope data, the algorithm integrates the incoming angular velocity over the time that passes between readings, accurately representing the change in the device's velocity. However, this data suffers from excessive noise and compouding error in the gyroscope's orientation, so the result is merely an approximation.

To get a more accurate result, a gradient function is used to atler the quaternion to reduce the error. First, quaternion multiplication is used to multiply the velocity estimate vector by the acccelerometer by its quaternion conjugate and the reference vector of Earth's gravity ([0,0,0,1]). This gives us a calculated gravity vector in the sensor's coordinate frame. We then subtract the acceleration vector from the accelerometer from this calculated gravity vector, giving us an objective function representing the error in the gyroscope's orientation estimate. 

To determine which way the gyroscope reading should be altered to reduce its error, a gradient function is then calculated, whigh is a matrix of first order partial derivatives of each component of the objective function relative to each component of the quaternion. The transpose of this matrix can then be multiplied by the objective error function to determine the direction of greatest ascent of error.

To eliminate this error, the algorithm takes the calculated direction of greatest ascent and reverses it, giving the direction of greatest descent of error. This reversed vector is then normalized to ensure a consistent step size. This step represents a single iteration of gradient descent, which tells the algorithm exactly how to tweak the four components of the quaternion to align the gyroscope's drift with the stable reference of physical gravity.

Every sensor sample, the algorithm calculates one gradient descent, which is then scaled by a tuneable parameter called the filter gain, represented by beta. This determines how much the algorithm trusts the accelerometer relative to the gyroscope. 

Finally, the algorithm subtracts this scaled gradient from the angular velocity estimate calculated with the gyroscope data. This gives an mprovced rate of change in the quaternion with elss error. This is then integrated over the time step and added to the previous orientation, to calculate the new position. From each of these resulting position quaternions, the pitch, roll, and yaw can be calculated using standardized formulas to get real world orientation data. 

### HOW "LEVEL" IS DEFINED
[EXPLAIN THE THRESHOLD FOR WHAT IS LEVEL HERE]


## CALIBRATION

### CALIBRATION PROCEDURE

The device features a momentary switch (button) on the side to "calibrate" it, or to set what is defined as "level". After the device initializes the IMU, it prompts the user to press the button, to set the "level" position while the barbell is located on the rack. When the button is pressed, the IMU takes 400 samples of the x, y and z components of the gyroscope and acceleration measurements, and averages them to get a baseline for each value. This is necessary because the Madgwick algorithm involves integrating change in the values over time, so there must be some "initial" value for the first calculation.

### WHY CALIBRATION IS NECESSARY

There are several reasons why a calibration function is necessary, instead of just setting some specific values for the initial acceleration and gyroscope values. Firstly, there is no guarantee that the barbell is actually straight. If the barbell is worn or has a lot of weight on it, and the device is not perfectly in the center, it might not start perfectly level. Secondly, if the device is being used on a grip other than a standard straight barbell, such as a curl bar, the device might not actually be level with the ground when the barbell is, meaning that some adjustment would be necessary.

## WIRING/PINOUT

### SCHEMATIC
[ADD A SCHEMATIC HERE]

### PINOUT

|ST Morpho Pin|Function|
|---|---|


[ADD A PINOUT CHART]

### PHOTO
[SHOW A PICTURE OF THE WIRES]


## 3D PRINTED ENCLOSURE

[ADD A PICTURE OF THE 3D PRINTED ENCLOSURE]

[EXPLAIN THE REASONING BEHIND THE MATERIAL CHOSEN]

[EXPLAIN TPU BARBELL GRIP]

## POTENTIAL FUTURE IMPROVEMENTS

There are several features that could be added to a second version of the project to improve practicality, including:
- A custom PCB
- A rechargable lithium-ion battery pack with USB-C charging
- Adjustable thresholds for "level"
- The ability to log data
- A slightly bigger screen for easier visibility


## REPOSITORY STRUCTURE

[ADD A DIAGRAM FOR THE STRUCTURE OF THE REPOSITORY]


## AUTHOR
Alexander Alt

Computer Engineering Student

Purdue University

West Lafayette, IN



