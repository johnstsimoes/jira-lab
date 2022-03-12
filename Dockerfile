FROM johnstsimoes/cpp-ready

LABEL maintainer="John Simoes" \
      description="Dockerfile to build jira-lab." \
      version="0.1.0"

RUN apk update
RUN apk upgrade
RUN apk add --no-cache -f readline-dev

RUN mkdir /lua \
    && cd /lua \
    && wget http://www.lua.org/ftp/lua-5.4.4.tar.gz \
    && tar -xvf lua-5.4.4.tar.gz \
    && cd lua-5.4.4 \
    && make \
    && make local \
    && mkdir /usr/include/lua \
    && cp install/include/* /usr/include/lua/ \
    && cp install/lib/liblua.a /usr/lib/

RUN ln -s /usr/include/lua5.4/ /usr/include/lua
RUN ln -s /usr/lib/liblua-5.4.so.0 /usr/lib/liblua.so

RUN git clone https://github.com/johnstsimoes/libstein.git
RUN cd libstein && mkdir build && cd build && cmake .. && make && make install

ADD . /jira-lab
RUN rm -rf /jira-lab/build
RUN mkdir -p /jira-lab/build && cd /jira-lab/build && cmake ..
RUN cd /jira-lab/build && make && make install

ENTRYPOINT redis-server --daemonize yes && bash
