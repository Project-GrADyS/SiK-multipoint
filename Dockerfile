FROM ubuntu

WORKDIR /usr/src/app

COPY . .

RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 3FA7E0328081BFF6A14DA29AA6A19B38D3D831EF

RUN echo "deb https://download.mono-project.com/repo/ubuntu stable-focal main" | sudo tee /etc/apt/sources.list.d/mono-official-stable.list


RUN apt update && apt upgrade -y

RUN apt install python3 mono-complete -y

RUN dpkg -i ./sdcc_3.5.0+dfsg-2_amd64

RUN sh ./ec2/install-sh

CMD["", ""]