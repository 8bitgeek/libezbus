* EzBus is still under development.
* EzBus is a token-bus protocol designed specifically for the RS-485 bus physical layer.
* EzBus can provide Ethernet-like throughput and stream socket semantics to resource constrained architectures.
* EzBus like other token bus and token ring architectures, has deterministic properties.
* EzBus is a peer-to-peer topology, where all peers are created equal, there is no "master".
* EzBus nodes may join and leave, the network at any time. The token-ring is self-healing.
* EzBus is intended to be lightweight and simple enough to run on small memory footprint micro-controllers.
* EzBuz + CARIBOU RTOS + Minimal socket-echo app is currently using about 38K FLASH, and 8K of RAM on STM32F746.
* EzBus currently works with CARIBOU RTOS, Linux, can be easily ported to other targets.
* EzBus API is easy to use by implementing a small number of easy to use callback functions.
* EzBus is written in C using the GCC compiler. 

2MBaud = 1Mbps parcel data thoughput
![EzBus 1mbps](https://raw.githubusercontent.com/8bitgeek/ezbus/documentation/images/ezbus-1mbps-1501.png)

*3 Nodes @ 115Kbps ~ 100Hz token period*
![EzBus 3 node](https://raw.githubusercontent.com/8bitgeek/ezbus/master/images/ezbus-3node-115kbps.png)



