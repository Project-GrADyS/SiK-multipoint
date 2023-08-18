based on the following sites and repositories

https://stackoverflow.com/questions/24225647/docker-a-way-to-give-access-to-a-host-usb-or-serial-device

https://github.com/tridge/ec2/issues/2

https://github.com/paragonRobotics/ec2-new

https://github.com/tdnet12434/MultiSiK

# Building docker image
- in the folder, run docker build . -t multisik-docker
- delete the last container created

# Creating the container for device reading...

docker run -t -i --device=/dev/ttyUSB0 --name <container_name> multisik-docker bash

# Enter in the terminal docker
- docker start <container_name>
- docker exec -it <container_name> /bin/bash

