FROM ubuntu:latest
RUN apt update
RUN apt install --yes "g++" make
COPY lib /app/lib
COPY Makefile /app/Makefile
RUN mkdir /app/.keys
COPY main.cpp /app/main.cpp
COPY src /app/src
RUN make -C /app build