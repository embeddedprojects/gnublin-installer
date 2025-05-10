FROM ubuntu:14.04

RUN apt-get update && apt-get install -y \
    make libcurl3-dev libparted0-dev libparted0debian1 libwxgtk2.8-dev gawk g++ && rm -rf /var/lib/apt/lists/*

WORKDIR /gnublin
COPY . .
RUN make
ENTRYPOINT [ "./gnublin-installer" ]