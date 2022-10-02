# can-mqtt-bridge

This program listens for messages on a CAN bus interface, decodes them according to a DBC file and forwards them to an MQTT broker. The DBC file must include an MQTT topic in the comment field of each signal.

The program is written for a real-time telemetry system for a racecar, but can surely be used in many other scenarios.

## Installation

First you will need to install the following libraries.

 - Vector_DBC ([Bitbucket](https://bitbucket.org/tobylorenz/vector_dbc/src/master/))
 - paho.mqtt.cpp ([GitHub](https://github.com/eclipse/paho.mqtt.cpp))

Now you can compile the program.

    $ g++ can-mqtt-bridge.cpp -o can-mqtt-bridge -lVector_DBC -lpaho-mqttpp3 -lpaho-mqtt3as

## Usage

Below are the command line options for this program.

    Usage: can-mqtt-bridge [options] <CAN interface>

    Options: -d FILE          DBC file to decode CAN frames
             -f FREQUENCY     Maximum frequency to forward to MQTT for each signal
             -v               Verbose mode, prints all decoded values
             -H               MQTT host to publish to
             -U               MQTT username
             -P               MQTT password
             -p               MQTT port
             -h               Display this help message

    Example: can-mqtt-bridge -d can.dbc -f 5 -H mqtt.example.com -U admin -P admin -p 1883 can0


