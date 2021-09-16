FROM johnstsimoes/cpp-ready

RUN git clone https://github.com/libcpr/cpr.git /cpr
RUN git clone https://github.com/fmtlib/fmt.git /fmt

RUN apk add --no-cache -f openssl-dev
RUN apk add --no-cache -f nlohmann-json
RUN apk add --no-cache -f zlib-dev
RUN apk add --no-cache -f linux-headers
RUN apk add --no-cache -f gtest-dev lua5.4-dev readline-dev

RUN ln -s /usr/include/lua5.4/ /usr/include/lua
RUN ln -s /usr/lib/liblua-5.4.so.0 /usr/lib/liblua.so

RUN mkdir -p /cpr/build && cd /cpr/build && cmake .. && make && make install
RUN mkdir -p /fmt/build && cd /fmt/build && cmake .. && make && make install

ADD . /jira-lab
RUN mkdir -p /jira-lab/build && cd /jira-lab/build && cmake .. && make
RUN cp /jira-lab/build/src/jira-lab /usr/local/bin
