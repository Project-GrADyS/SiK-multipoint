<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![LinkedIn][linkedin-shield]][linkedin-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://ardupilot.org/copter/docs/common-3dr-radio-advanced-configuration-and-technical-information.html">
    <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/1f/ArduPilot_logo.svg/2560px-ArduPilot_logo.svg.png" alt="Logo" width="270" height="44">
  </a>

  <h3 align="center">SiK-multipoint</h3>

  <p align="center">
    Updated container design that facilitates the configuration of sik radios for multipoint
    <br />
    <a href="https://www.lac.inf.puc-rio.br/index.php/gradys/"><strong>See more about GRADYS »</strong></a>
    <br />
    <br />
    <a href="https://github.com/othneildrew/Best-README-Template">View Demo</a>
    ·
    <a href="https://github.com/othneildrew/Best-README-Template/issues">Report Bug</a>
    ·
    <a href="https://github.com/othneildrew/Best-README-Template/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#differences-from-standard-image">Differences from standard image</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#building-docker-image">Building docker image</a></li>
        <li><a href="#creating-the-container">Creating the container</a></li>
        <li><a href="#enter-in-the-terminal">Enter in the terminal</a></li>
        <li><a href="#install-firmware">Install Firmware</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

[![Product Name Screen Shot][product-screenshot]](https://example.com)

Firmware for SiLabs Si1000 - Si102x/3x ISM radios

SiK is a collection of firmware and tools for radios based on the cheap, versatile SiLabs Si1000 SoC.

Currently, it supports the following boards:

 - HopeRF HM-TRP
 - HopeRF RF50-DEMO
 - RFDesign RFD900a
 - RFDesign RFD900u
 - RFDesign RFD900p

Adding support for additional boards should not be difficult.

Currently the firmware components include:

 - A bootloader with support for firmware upgrades over the serial interface.
 - Radio firmware with support for parsing AT commands, storing parameters and FHSS/TDM functionality

See the user documentation above for a list of current firmware features

## Multipoint Support - Differences from standard image
### New AT Variables

The AT commands, which stand for "Attention" commands, are a set of instructions used for controlling and communicating with various devices, particularly communication modules such as GSM, GPRS, and other modems. These commands are sent to the device through a communication interface, typically serial communication. Here are new commands used for multipoint firmware:

###### S15: NODEID
MUST be a unique ID on the network, otherwise cross talk will occur
Base ID is defined by setting to 0, this is the node that keeps the network in sync with all other nodes.
When setting the the NODEID to the base (ie 0) it will always have link to it's self thus link will never be lost.

###### S16: NODEDESTINATION
This is where all the serial data recived on this node should be sent to.
For example, to send all data to the base node only set this to 0.
DEFAULT is 65535 which is broadcast to all nodes.

###### S17: SYNCANY
Sync Any should be used sparingly, this allows any node to sync from any node in the network not just the base.
By allowing many nodes to sync from each other they could drift away from the main sync channel,
thus efectivly becoming a diffrent NETID.

###### S18: NODECOUNT
This is the number of nodes in the nework, so if you have 3 nodes (Id's 0, 1, 2) you have a network of 3.
In the next revision of the software this could disapear and become auto detect.
The easiest way to determin the correct node count for the network is - max(NODEID)+1

### Diffrent RT Syntax

All RT's are sent to every node by default, to send them to one node only folow the command with a comma and node id.
Example getting the parameter list from base node would be 'RTI5,0'

_To see all other available AT commands, see the [Documentation](https://ardupilot.org/copter/docs/common-3dr-radio-advanced-configuration-and-technical-information.html)_

_Here is the official radio [SiK Repository](https://github.com/ArduPilot/SiK)_


<p align="right">(<a href="#readme-top">back to top</a>)</p>



### What You Will Need

 - At least two Si1000 - Si102x/3x - based radio devices (just one radio by itself is not very useful).
 - A FTDI adapter

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started
### Building docker image


_Building a Docker image is a crucial step in the process of containerizing and deploying applications. This process involves creating a customized, self-contained environment for an application to ensure consistency and reproducibility across different environments._

1. Clone the repo
   ```sh
   git clone https://github.com/johan-rocha/SiK-multipoint
   ```
2. In the folder, run
   ```sh
   docker build -t multisik-docker .
   ```

### Creating the container for device reading...
1. With the device connected, do:
   ```sh
   docker run -t -i --device=/dev/ttyUSB0 --name <container_name> multisik-docker bash
   ```

### Enter in the terminal docker
1. Start container
   ```sh
    docker start <container_name>
   ```
2. open container terminal
   ```sh
   docker exec -it <container_name> /bin/bash
   ```
### Installing firmware
1. changing the parameters before installation
   ```sh
   nano radio/parameters.c
   ```
2. compiling .ihx images
   ```sh
   make install
   ```
3. uploading firmware
   ```sh
   ./tools/uploader.py --resetparams --port /dev/ttyUSB0 /usr/src/app/MultiSiK/Firmware/obj/hm_trp/radio~hm_trp/radio~hm_trp.ihx
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage Examples

_For more examples, please refer to the [Documentation](https://ardupilot.org/copter/docs/common-3dr-radio-advanced-configuration-and-technical-information.html)_

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Johan Rocha - johanmarcos50@gmail.com


<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/johan-rocha-4a8224226/
[product-screenshot]: images/screenshot.png