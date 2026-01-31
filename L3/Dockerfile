# Dockerfile
FROM ubuntu:22.04

# Install g++ and Valgrind
RUN apt-get update && \
    apt-get install -y g++ valgrind && \
    apt-get clean

WORKDIR /app

