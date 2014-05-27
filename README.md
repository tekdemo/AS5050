AS5050 Arduino Lib
====================
This is a utility library for interfacing with the AS5050 magnetic rotary encoder chip using an Arduino. While still a work in progress, it performs basic functions reliably.

Current Features
----------------
- Convert raw angular data to degrees or radians for integration into other code
- Track the total rotation from either a physical or relative position.
- utility functions for reading and writing on-chip registers
- Header with significant documentation of chip data, including register, addresses, protocol, and responses.
- Automatic handling of internal gain. 
- Read/write arbitrary chip registers with simplified interface.

Work in progress
----------------
- Rudimentary error handling of transmission line / parity errors.
- Example programs for demonstration

Future/wishlist
---------------
- Interrupt driven update mechanism
- Properly handle all common chip communication errors


