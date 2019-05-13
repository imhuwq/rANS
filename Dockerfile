FROM docker.imhuwq.com/ubuntu:16.04
MAINTAINER imhuwq "imhuwq@gmail.com"

USER root
RUN echo "deploy ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
USER deploy
# 创建项目源码目录，这个目录将成为 Container 里面构建和执行的工作区
RUN mkdir -p /home/deploy/rANS
WORKDIR /home/deploy/rANS
ENV LC_ALL C.UTF-8