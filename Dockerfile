FROM alpine:latest
RUN apk update
RUN apk add --update build-base
COPY lib /app/lib
COPY Makefile /app/Makefile
RUN mkdir /app/.keys
COPY main.cpp /app/main.cpp
COPY src /app/src
RUN make -C /app build
