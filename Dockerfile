FROM alpine:latest
RUN apk update
RUN apk add --update build-base
COPY src /app/src
COPY lib /app/lib
COPY Makefile /app/Makefile
COPY main.cpp /app/main.cpp
RUN mkdir /app/.keys
RUN make -C /app build