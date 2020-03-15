# can-mqtt-bridge

This program listens for packets on a CAN bus interface and relays them to an MQTT broker. The CAN messages and signals must be specified in a DBC file along with the MQTT topic for each signal.

## Installation

    $ g++ can-mqtt.cpp -o can-mqtt -ldbcppp

Note that this program is incomplete.
