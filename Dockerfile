FROM ubuntu:latest

LABEL maintainer="John Simoes" \
      description="Dockerfile to build jira-lab on Ubuntu." \
      version="0.1.0"

RUN apt -y update
RUN apt -y upgrade
RUN apt -y install --no-install-recommends tzdata
RUN apt -y install --no-install-recommends git cmake make g++
RUN apt -y install --no-install-recommends vim bash libssl-dev
# RUN apt -y install --no-install-recommends zlib-dev
# linux-headers
RUN apt -y install --no-install-recommends libcurl4-openssl-dev
RUN apt -y install --no-install-recommends libhiredis-dev redis libfmt-dev

RUN apt -y install --no-install-recommends libreadline-dev
RUN apt -y install --no-install-recommends ca-certificates
RUN apt -y install --no-install-recommends googletest
RUN apt -y install --no-install-recommends wget

RUN cd /usr/src/googletest \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make \
    && make install

RUN mkdir /lua \
    && cd /lua \
    && wget http://www.lua.org/ftp/lua-5.4.3.tar.gz \
    && tar -xvf lua-5.4.3.tar.gz \
    && cd lua-5.4.3 \
    && make \
    && make local \
    && mkdir /usr/include/lua \
    && cp install/include/* /usr/include/lua/ \
    && cp install/lib/liblua.a /usr/lib/ \
    && rm -rf /lua

RUN apt install nlohmann-json3-dev
RUN apt install libssl-dev

ADD . /jira-lab
RUN rm -rf /jira-lab/build
RUN mkdir -p /jira-lab/build && cd /jira-lab/build && cmake ..
RUN cd /jira-lab/build && make
RUN cp /jira-lab/build/src/jira-lab /usr/local/bin

# ENTRYPOINT redis-server --daemonize yes && bash
