
# **Coldboot**

The goal of **coldboot** is to produce and maintain an ordered bus. 

An **ordered bus** means the bus which is running with ordered ownership of the **mutex token**.  

The **coldboot** state of a node may be entered while the bus ordered or disordered.

The **coldboot** state machine must determine the state of the bus and transition the node onto the bus according to the current bus state.

The **coldboot** state machine is divided into two major function blocks, **coldboot minor**, and **coldboot major**.

## **Coldboot Minor**

When a node initially enters the **coldboot** state, the overall state of the bus is unknown to the node.

The node will transition initially into a **coldboot minor** state.

In the **coldboot minor** state, the node will only listen to traffic for some time, in particular, listening for the **mutex token** which indicates that an **ordered bus** is in-progress already.

If, after sufficient time, there is no sign of a **mutex token**, and/or there are other **coldboot** packets are in progress, then the node transitions to **coldboot major** state.

If, there is an **ordered bus** detected, then the node will wait silently, until invited to respond with a **warm boot** invitation broadcast.

## **Coldboot Major**

The function of **coldboot major** is to determine which node is the so called **dominant node**.

The node which emerges from **cold boot** as the **dominant node** is the node which holds the **mutex token** 

How is the **dominant node** determined? 

Once a node transitions to **coldboot major** state, it will begin broadcasting and listening for identity packets.



## **Coldboot State Diagram**

![](assets/coldboot.dot.svg)
