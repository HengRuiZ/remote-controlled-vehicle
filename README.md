# remote controlled vihical

Controller:Two rocking bars are used to indicate the direction and speed. They are connected to A/D converter. NRF24L01 is used to send command to the vehicle.

Vehicle:Get command from controller, then decide the action of a servo and two motors.

Both are powered by STM32F103, and communicate through NRF24L01.

Wheels are drived by MOS driver.
