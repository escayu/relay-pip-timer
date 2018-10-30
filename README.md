# relay-pip-timer
Manage the time a device remains activated (lights on) when no presence is detected 

The hw devices that make up the project are:
     1. Arduino board
     
     2. Presence detector (PIP) connected to digital input 2 of the Arduino board
     3. Relay that makes the voltage switch that feeds the bulb. It is connected to the digital output 3 of the Arduino board
     4. 5 volt power supply for the Arduino board.
   Start a timer of 120 seconds. When the timer expires,
   acts on a relay to open the circuit to which the light is connected (turn on light).
   The timer restarts each time the PIP detects presence.
