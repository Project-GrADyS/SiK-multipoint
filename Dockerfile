FROM ubuntu:jammy-20211029

WORKDIR /usr/src/app

COPY . .

RUN apt-get update && apt-get install -y apt-utils gnupg2 ca-certificates

RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 3FA7E0328081BFF6A14DA29AA6A19B38D3D831EF

RUN apt update && apt upgrade -y

RUN apt install g++ libreadline-dev libboost-regex-dev usbutils libusb-dev autoconf automake libtool libudev-dev make cmake cmake-curses-gui -y

RUN cd /usr/src/app/Python-2.4.6 && ./configure --enable-optimizations --includedir=/usr/include/tirpc/ && make && make install

RUN touch ~/.bashrc

RUN dpkg -i ./sdcc-libraries_3.5.0+dfsg-2_all.deb 

RUN dpkg -i ./sdcc_3.5.0+dfsg-2_amd64.deb

RUN cd /usr/src/app/ec2-new && autoreconf -i && ./configure && make

RUN apt install python2 python-argparse -y

RUN python2 get-pip.py && pip install pyserial==2.7

RUN cd /usr/src/app/MultiSiK/Firmware && make install