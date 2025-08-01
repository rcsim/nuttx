#!/usr/bin/env sh
############################################################################
# tools/ci/platforms/darwin.sh
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

# NuttX CI Job for macOS, runs on Apple Intel Mac

# Prerequisites for macOS
#  - Xcode (cc, etc)
#  - homebrew
#  - curl
#  - wget
#  - cmake

set -e
set -o xtrace

add_path() {
  PATH=$1:${PATH}
}

arm_clang_toolchain() {
  add_path "${NUTTXTOOLS}"/clang-arm-none-eabi/bin

  if [ ! -f "${NUTTXTOOLS}/clang-arm-none-eabi/bin/clang" ]; then
    local basefile
    basefile=LLVMEmbeddedToolchainForArm-17.0.1-Darwin
    cd "${NUTTXTOOLS}"
    # Download the latest ARM clang toolchain prebuilt by ARM
    curl -O -L -s https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm/releases/download/release-17.0.1/${basefile}.dmg
    sudo hdiutil attach ${basefile}.dmg
    sudo cp -R /Volumes/${basefile}/${basefile} "${NUTTXTOOLS}"/${basefile}
    sudo mv ${basefile} clang-arm-none-eabi
    rm ${basefile}.dmg
  fi

  command clang --version
}

arm_gcc_toolchain() {
  add_path "${NUTTXTOOLS}"/gcc-arm-none-eabi/bin

  if [ ! -f "${NUTTXTOOLS}/gcc-arm-none-eabi/bin/arm-none-eabi-gcc" ]; then
    local basefile
    basefile=arm-gnu-toolchain-13.2.rel1-darwin-x86_64-arm-none-eabi
    cd "${NUTTXTOOLS}"
    curl -O -L -s https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/${basefile}.tar.xz
    xz -d ${basefile}.tar.xz
    tar xf ${basefile}.tar
    mv ${basefile} gcc-arm-none-eabi
    rm ${basefile}.tar
  fi

  command arm-none-eabi-gcc --version
}

arm64_gcc_toolchain() {
  add_path "${NUTTXTOOLS}"/gcc-aarch64-none-elf/bin

  if [ ! -f "${NUTTXTOOLS}/gcc-aarch64-none-elf/bin/aarch64-none-elf-gcc" ]; then
    local basefile
    basefile=arm-gnu-toolchain-13.2.Rel1-darwin-x86_64-aarch64-none-elf
    cd "${NUTTXTOOLS}"
    # Download the latest ARM64 GCC toolchain prebuilt by ARM
    curl -O -L -s https://developer.arm.com/-/media/Files/downloads/gnu/13.2.Rel1/binrel/${basefile}.tar.xz
    xz -d ${basefile}.tar.xz
    tar xf ${basefile}.tar
    mv ${basefile} gcc-aarch64-none-elf
    rm ${basefile}.tar
  fi

  command aarch64-none-elf-gcc --version
}

avr_gcc_toolchain() {
  if ! type avr-gcc > /dev/null 2>&1; then
    brew tap osx-cross/avr
    brew install avr-gcc
  fi

  command avr-gcc --version
}

autoconf_brew() {
  if ! type autoconf > /dev/null 2>&1; then
    brew install autoconf
  fi

  command autoconf --version
}

binutils() {
  add_path "${NUTTXTOOLS}"/bintools/bin

  if ! type objcopy > /dev/null 2>&1; then
    brew install binutils
    mkdir -p "${NUTTXTOOLS}"/bintools/bin
    # It is possible we cached prebuilt but did brew install so recreate
    # symlink if it exists
    rm -f "${NUTTXTOOLS}"/bintools/bin/objcopy
    ln -s /usr/local/opt/binutils/bin/objcopy "${NUTTXTOOLS}"/bintools/bin/objcopy
  fi

  command objcopy --version
}

bloaty() {
  add_path "${NUTTXTOOLS}"/bloaty/bin

  if [ ! -f "${NUTTXTOOLS}/bloaty/bin/bloaty" ]; then
    git clone --branch main https://github.com/google/bloaty "${NUTTXTOOLS}"/bloaty-src
    cd "${NUTTXTOOLS}"/bloaty-src
    # Due to issues with latest MacOS versions use pinned commit.
    # https://github.com/google/bloaty/pull/326
    # https://github.com/google/bloaty/pull/347
    # https://github.com/google/bloaty/pull/385
    git checkout 8026607280ef139bc0ea806e88cfe4fd0af60bad
    mkdir -p "${NUTTXTOOLS}"/bloaty
    cmake -B build/bloaty -GNinja -D BLOATY_PREFER_SYSTEM_CAPSTONE=NO -D CMAKE_INSTALL_PREFIX="${NUTTXTOOLS}"/bloaty
    cmake --build build/bloaty
    cmake --build build/bloaty --target install
    cd "${NUTTXTOOLS}"
    rm -rf bloaty-src
    ls -a "${NUTTXTOOLS}"/bloaty
  fi

  command bloaty --version
}

c_cache() {
  add_path "${NUTTXTOOLS}"/ccache/bin

  if ! type ccache > /dev/null 2>&1; then
    brew install ccache
  fi

  setup_links
  command ccache --version
}

elf_toolchain() {
  if ! type x86_64-elf-gcc > /dev/null 2>&1; then
    brew install x86_64-elf-gcc
  fi

  command x86_64-elf-gcc --version
}

gen_romfs() {
  if ! type genromfs > /dev/null 2>&1; then
    brew tap PX4/px4
    brew install genromfs
  fi
}

gperf() {
  add_path "${NUTTXTOOLS}"/gperf/bin

  if [ ! -f "${NUTTXTOOLS}/gperf/bin/gperf" ]; then
    local basefile
    basefile=gperf-3.1

    cd "${NUTTXTOOLS}"
    curl -O -L -s http://ftp.gnu.org/pub/gnu/gperf/${basefile}.tar.gz
    tar zxf ${basefile}.tar.gz
    cd "${NUTTXTOOLS}"/${basefile}
    ./configure --prefix="${NUTTXTOOLS}"/gperf; make; make install
    cd "${NUTTXTOOLS}"
    rm -rf ${basefile}; rm ${basefile}.tar.gz
  fi

  command gperf --version
}

kconfig_frontends() {
  add_path "${NUTTXTOOLS}"/kconfig-frontends/bin

  if [ ! -f "${NUTTXTOOLS}/kconfig-frontends/bin/kconfig-conf" ]; then
    git clone --depth 1 https://bitbucket.org/nuttx/tools.git "${NUTTXTOOLS}"/nuttx-tools
    cd "${NUTTXTOOLS}"/nuttx-tools/kconfig-frontends
    ./configure --prefix="${NUTTXTOOLS}"/kconfig-frontends \
      --disable-kconfig --disable-nconf --disable-qconf \
      --disable-gconf --disable-mconf --disable-static \
      --disable-shared --disable-L10n
    # Avoid "aclocal/automake missing" errors
    touch aclocal.m4 Makefile.in
    make install
    cd "${NUTTXTOOLS}"
    rm -rf nuttx-tools
  fi
}

mips_gcc_toolchain() {
  add_path "${NUTTXTOOLS}"/pinguino-compilers/macosx/p32/bin

  if [ ! -d "${NUTTXTOOLS}/pinguino-compilers" ]; then
    cd "${NUTTXTOOLS}"
    git clone https://github.com/PinguinoIDE/pinguino-compilers
  fi

  command mips-elf-gcc --version
}

ninja_brew() {
  if ! type ninja > /dev/null 2>&1; then
    brew install ninja
  fi

  command ninja --version
}

python_tools() {
  if [ -z "${VIRTUAL_ENV}" ]; then
    # Python User Env
    export PIP_USER=yes
    export PYTHONUSERBASE=${NUTTXTOOLS}/pylocal
    echo "export PIP_USER=yes" >> "${NUTTXTOOLS}"/env.sh
    echo "export PYTHONUSERBASE=${NUTTXTOOLS}/pylocal" >> "${NUTTXTOOLS}"/env.sh
    add_path "${PYTHONUSERBASE}"/bin
  fi
  
  if [ "X$osarch" == "Xarm64" ]; then
    python3 -m venv --system-site-packages /opt/homebrew
  fi
  
  # workaround for Cython issue
  # https://github.com/yaml/pyyaml/pull/702#issuecomment-1638930830
  pip3 install "Cython<3.0"
  git clone https://github.com/yaml/pyyaml.git && \
  cd pyyaml && \
  git checkout release/5.4.1 && \
  sed -i.bak 's/Cython/Cython<3.0/g' pyproject.toml && \
  python setup.py sdist && \
  pip3 install --pre dist/PyYAML-5.4.1.tar.gz
  cd ..
  rm -rf pyyaml

  pip3 install \
    cmake-format \
    construct \
    cvt2utf \
    cxxfilt \
    esptool==4.8.dev4 \
    imgtool==1.9.0 \
    kconfiglib \
    pexpect==4.8.0 \
    pyelftools \
    pyserial==3.5 \
    pytest==6.2.5 \
    pytest-json==0.4.0 \
    pytest-ordering==0.6 \
    pytest-repeat==0.9.1
}

riscv_gcc_toolchain() {
  add_path "${NUTTXTOOLS}"/riscv-none-elf-gcc/bin

  if [ ! -f "${NUTTXTOOLS}/riscv-none-elf-gcc/bin/riscv-none-elf-gcc" ]; then
    local basefile
    basefile=xpack-riscv-none-elf-gcc-14.2.0-3-darwin-x64
    cd "${NUTTXTOOLS}"
    # Download the latest RISCV GCC toolchain prebuilt by xPack
    curl -O -L -s https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v14.2.0-3/${basefile}.tar.gz
    tar zxf ${basefile}.tar.gz
    mv xpack-riscv-none-elf-gcc-14.2.0-3 riscv-none-elf-gcc
    rm ${basefile}.tar.gz
  fi

  command riscv-none-elf-gcc --version
}

rust() {
  if ! type rustc > /dev/null 2>&1; then
    brew install rust
  fi

  command rustc --version
}

dlang() {
  if ! type ldc2 > /dev/null 2>&1; then
    brew install ldc
  fi

  command ldc2 --version | head -n 5
}

zig() {
  brew install zig

  command zig cc --version
}

xtensa_esp_gcc_toolchain() {
  add_path "${NUTTXTOOLS}"/xtensa-esp-elf/bin

  if [ ! -f "${NUTTXTOOLS}/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc" ]; then
    local basefile
    basefile=xtensa-esp-elf-14.2.0_20241119-x86_64-apple-darwin
    cd "${NUTTXTOOLS}"
    # Download the latest ESP32, ESP32-S2 and ESP32-S3 GCC toolchain prebuilt by Espressif
    curl -O -L -s https://github.com/espressif/crosstool-NG/releases/download/esp-14.2.0_20241119/${basefile}.tar.xz
    xz -d ${basefile}.tar.xz
    tar xf ${basefile}.tar
    rm ${basefile}.tar
  fi

  command xtensa-esp32-elf-gcc --version
}

u_boot_tools() {
  if ! type mkimage > /dev/null 2>&1; then
    brew install u-boot-tools
  fi
}

util_linux() {
  if ! type flock > /dev/null 2>&1; then
    brew tap discoteq/discoteq
    brew install flock
  fi

  command flock --version
}

wasi_sdk() {
  add_path "${NUTTXTOOLS}"/wamrc

  if [ ! -f "${NUTTXTOOLS}/wamrc/wasi-sdk/bin/clang" ]; then
    local wasibasefile
    local wasmbasefile
    wasibasefile=wasi-sdk-19.0-macos
    wasmbasefile=wamrc-1.1.2-x86_64-macos-latest
    cd "${NUTTXTOOLS}"
    mkdir -p wamrc
    curl -O -L -s https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-19/${wasibasefile}.tar.gz
    tar xzf ${wasibasefile}.tar.gz
    mv wasi-sdk-19.0 wasi-sdk
    rm ${wasibasefile}.tar.gz
    cd wamrc
    curl -O -L -s https://github.com/bytecodealliance/wasm-micro-runtime/releases/download/WAMR-1.1.2/${wasmbasefile}.tar.gz
    tar xzf ${wasmbasefile}.tar.gz
    rm ${wasmbasefile}.tar.gz
  fi

  export WASI_SDK_PATH="${NUTTXTOOLS}/wasi-sdk"
  echo "export WASI_SDK_PATH=${NUTTXTOOLS}/wasi-sdk" >> "${NUTTXTOOLS}"/env.sh

  command "${WASI_SDK_PATH}"/bin/clang --version
  command wamrc --version
}

setup_links() {
  # Configure ccache
  mkdir -p "${NUTTXTOOLS}"/ccache/bin/
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/aarch64-none-elf-gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/aarch64-none-elf-g++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/arm-none-eabi-gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/arm-none-eabi-g++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/avr-gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/avr-g++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/cc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/c++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/clang
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/clang++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/g++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/p32-gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/riscv64-unknown-elf-gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/riscv64-unknown-elf-g++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/sparc-gaisler-elf-gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/sparc-gaisler-elf-g++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/x86_64-elf-gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/x86_64-elf-g++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/xtensa-esp32-elf-gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/xtensa-esp32-elf-g++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/xtensa-esp32s2-elf-gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/xtensa-esp32s2-elf-g++
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/xtensa-esp32s3-elf-gcc
  ln -sf "$(which ccache)" "${NUTTXTOOLS}"/ccache/bin/xtensa-esp32s3-elf-g++
}

install_build_tools() {
  mkdir -p "${NUTTXTOOLS}"
  echo "#!/usr/bin/env sh" > "${NUTTXTOOLS}"/env.sh

  install="ninja_brew autoconf_brew arm_clang_toolchain arm_gcc_toolchain arm64_gcc_toolchain avr_gcc_toolchain binutils bloaty elf_toolchain gen_romfs gperf kconfig_frontends mips_gcc_toolchain python_tools riscv_gcc_toolchain rust dlang zig xtensa_esp_gcc_toolchain u_boot_tools util_linux wasi_sdk c_cache"

  mkdir -p "${NUTTXTOOLS}"/homebrew
  export HOMEBREW_CACHE=${NUTTXTOOLS}/homebrew
  echo "export HOMEBREW_CACHE=${NUTTXTOOLS}/homebrew" >> "${NUTTXTOOLS}"/env.sh
  # https://github.com/apache/arrow/issues/15025
  rm -f /usr/local/bin/2to3* || :
  rm -f /usr/local/bin/idle3* || :
  rm -f /usr/local/bin/pydoc3* || :
  rm -f /usr/local/bin/python3* || :
  rm -f /usr/local/bin/python3-config || :
  # same for openssl
  rm -f /usr/local/bin/openssl || :

  oldpath=$(cd . && pwd -P)
  for func in ${install}; do
    ${func}
  done
  cd "${oldpath}"

  echo "PATH=${PATH}" >> "${NUTTXTOOLS}"/env.sh
  echo "export PATH" >> "${NUTTXTOOLS}"/env.sh
}

install_build_tools
