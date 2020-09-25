FROM ubuntu:latest

# set env to disable interactive mode
# during install of debconf
ARG DEBIAN_FRONTEND=noninteractive

# update dependencies if needed
RUN apt-get update -y

# install cmake, boost, openssl and mysql connector
RUN apt-get install -y cmake \
    build-essential \
    libboost-all-dev \
    libssl-dev \
    libmysqlcppconn-dev

RUN apt-get install -y nodejs npm