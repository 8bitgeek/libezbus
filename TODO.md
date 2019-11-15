* LAYER-0

* Packet receiver to have a static temporary holding buffer
  such that packets may be received to be NACK'ed in the 
  event that the driver I/O receiver buffer is occupied.

* Straighten up placement of packet into driver I/O tranmit
  buffer 

* Add "insort" method to ezbus_peer_list.

* Add "self_next" method to ezbus_peer_list to retrieve the 
  next peer in the list after self for token passing.

* Add token timeout timer to driver for executing a callback
  in the event token has taken too long.

* Token passing.

* Parcel packet transmitting and recieving callbacks.



* LAYER-1

* Parcel assembly / disassembly


