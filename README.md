# Introduction

This project implements an Internet of Things (IoT) system where a Raspberry Pi acts as a gateway, enabling nRF24-based sensor nodes (typically Arduino devices) to connect to a cloud platform (Azure). The system facilitates the collection of sensor data from distributed nodes and its transmission to the cloud for storage, analysis, and visualization.

Key technologies used in this project include:
* Raspberry Pi (as the gateway)
* Arduino (for sensor nodes)
* nRF24L01+ wireless transceivers
* Python (for the gateway logic on Raspberry Pi)
* Microsoft Azure (as the cloud platform)

This project builds upon core code blocks and libraries from external sources, as acknowledged below.

IoT
=======

IoT PoC project to test connectivity Raspberry PI and Arduino on NRF24 based code
Core code blocks was received from http://blog.riyas.org/2014/08/raspberry-pi-as-nrf24l01-base-station-internet-connected-wireless.html with great respects.
NRF24 library is copied from a different source.
