## C Gtk+ MacChanger
Command-line interface is a thing of the past. MacChanger allows you to change your *MAC address* using GUI. This enables even the novice Linux users to use it.

## Prerequisites
* git
* gtk3-dev
* pkg-config

#### can install the requisites using apt:
* apt install git
* apt install gtk3-dev
* apt install pkg-config
#### or use other alternatives.

## Getting Start
## clone the repository to your machine
* mkdir imageviewer-1.0
* git init
* git clone https://github.com/PromiseMasango/ImageViewer-1.0

## Screenshots
![Welcome screen](/images/image1.png)
![select image](/images/image2.png)
![about](/images/image3.png)

## Features
* Check permanent mac address of a network interface.
* Check current mac address of a network interface.
* Forge a mac address for a network interface. 

## installation
### compile code by running the following command on a terminal
* make
### run the program by typing the following command on a terminal
* ./macchanger

### additional readings
1. Make sure the network interface is down before you attempt to change its mac address.
