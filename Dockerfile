FROM johnstsimoes/cpp-ready:latest

RUN apk add --no-cache -f gtest-dev lua5.4-dev readline-dev

RUN ln -s /usr/include/lua5.4/ /usr/include/lua
RUN ln -s /usr/lib/liblua-5.4.so.0 /usr/lib/liblua.so
RUN ln -s /usr/local/lib/libcpr.so /usr/lib/libcpr.so

ADD . /jira-lab
RUN rm -rf /jira-lab/build
RUN mkdir -p /jira-lab/build && cd /jira-lab/build && cmake ..
RUN cd /jira-lab/build && make
RUN cp /jira-lab/build/src/jira-lab /usr/local/bin

ENTRYPOINT redis-server --daemonize yes && bash
