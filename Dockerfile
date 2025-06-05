FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      build-essential \
      libreadline-dev \
      pkg-config \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/minishell

COPY . .

RUN make

ENTRYPOINT ["./minishell"]
