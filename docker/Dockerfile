FROM ubuntu:focal

RUN apt-get update
RUN apt-get install -y software-properties-common wget
RUN add-apt-repository ppa:linuxgndu/sqlcipher -y
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN echo 'deb http://apt.llvm.org/focal/ llvm-toolchain-focal main' | tee -a /etc/apt/sources.list.d/llvm.list
RUN apt-get update && apt-get install -y libsqlcipher-dev llvm clang cmake zlib1g-dev
