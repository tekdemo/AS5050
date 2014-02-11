AS5050 Arduino Lib
====================
This is a utility library for interfacing with the AS5050 magnetic rotary encoder chip. While still a work in progress, it currently can do the following:

Current Features
----------------
- Convert raw angular data to degrees or radians for integration into other code
- Track the total rotation from either a physical or relative position.
- utility functions for reading and writing on-chip registers
- Header with significant documentation of chip data, including register, addresses,protocol, and responses.

Work in progress
----------------
- Automatic gain adjustment
- Rudimentary error handling 
- Example programs for demonstration

Future/wishlist
---------------
- Interrupt driven update mechanism
- Properly handle all common chip communication errors


