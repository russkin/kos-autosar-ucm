FROM ubuntu:20.04 as build
# get actual download url from https://os.kaspersky.ru/development/?form=download or use local downloaded package path
ARG SDK_DOWNLOAD_URL
ARG SDK_VERSION=1.1.1.40
ADD $SDK_DOWNLOAD_URL /tmp/kos.deb 
RUN apt-get update                          && \
    DEBIAN_FRONTEND=noninteractive             \
    apt install -y /tmp/kos.deb             && \
    ln -sf /opt/KasperskyOS-Community-Edition-$SDK_VERSION /opt/kos
ENV PATH="${PATH}:/opt/kos/toolchain/bin"
WORKDIR /app
ADD Makefile .
ADD ./cmake ./cmake
ADD ./src ./src
RUN make package


FROM ubuntu:20.04 as runtime
RUN apt-get update                          && \
    DEBIAN_FRONTEND=noninteractive             \
    groupadd -g 10001 kos                   && \
    useradd -u 10000 -g kos demo
WORKDIR /opt/kos-ucm/bin
# Custom cache invalidation
ARG CACHEBUST=0
COPY --from=build /app/.build/kos-autosar_0.0.1_amd64.deb /tmp/kos-autosar.deb
RUN apt install -y /tmp/kos-autosar.deb
USER demo

ENTRYPOINT ["./qemu-system-aarch64", "-m", "512", "-machine", "vexpress-a15,secure=on", "-cpu", "cortex-a72", "-nographic", "-monitor", "none", "-smp", "4", "-nic", "user", "-serial", "stdio", "-kernel"]
CMD ["kos-qemu-image"]
