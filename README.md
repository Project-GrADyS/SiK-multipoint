https://stackoverflow.com/questions/24225647/docker-a-way-to-give-access-to-a-host-usb-or-serial-device

docker run -t -i --device=/dev/ttyUSB0 ubuntu bash
