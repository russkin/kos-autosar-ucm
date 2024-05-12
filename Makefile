SCRIPT_DIR=$(shell pwd)
BUILD=${SCRIPT_DIR}/.build
INSTALL_PREFIX=${BUILD}/../.install/opt/kos-ucm
TARGET=aarch64-kos
SDK_PREFIX=/opt/kos
TOOLCHAIN_SUFFIX=
BUILD_TYPE:=Debug

build:
	"${SDK_PREFIX}/toolchain/bin/cmake" -G "Unix Makefiles" -B "${BUILD}" 	\
		-D CMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} 							\
		-D CMAKE_INSTALL_PREFIX:STRING="${INSTALL_PREFIX}" 					\
		-D SDK_PREFIX=${SDK_PREFIX}											\
		-D CMAKE_FIND_ROOT_PATH="${SDK_PREFIX}/sysroot-${TARGET}" \
		-D CMAKE_TOOLCHAIN_FILE="${SDK_PREFIX}/toolchain/share/toolchain-${TARGET}${TOOLCHAIN_SUFFIX}.cmake" \
		"${SCRIPT_DIR}/src"
	"${SDK_PREFIX}/toolchain/bin/cmake" --build "${BUILD}" --target kos-qemu-image

install: build
	"${SDK_PREFIX}/toolchain/bin/cmake" --install "${BUILD}" 

package: build
	"${SDK_PREFIX}/toolchain/bin/cmake" --build "${BUILD}" --target package

run: build
	"${SDK_PREFIX}/toolchain/bin/cmake" --build "${BUILD}" --target sim

clean:
	rm -rf "${BUILD}" "${INSTALL_PREFIX}"