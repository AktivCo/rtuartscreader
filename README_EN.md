[Russian/Русский](README_RUS.md)

## Description

**Rtuartscreader** is an implementation of user-space driver ([IFD Handler](https://pcsclite.apdu.fr/api/group__IFDHandler.html#details))
for [pcsc-lite](https://pcsclite.apdu.fr/) to provide access via PCSC interface to the smart cards
[Rutoken 2151 and Rutoken 2010](https://www.rutoken.ru/products/all/rutoken-m2m/#models) using UART transmitter and GPIO pins of a microcontroller. The driver is designed for the use in [Rutoken M2M](https://www.rutoken.ru/products/all/rutoken-m2m/) demonstration kit based on Raspberry Pi 2/3 PC.

The artifacts of the project are:
* `librtuartscreader.so` -- driver library. By default, it gets installed to `/usr/lib/pcsc/drivers/serial/`.
* `librtuartscreader` -- driver configuration file used by pcscd to start-up the driver. By default, it gets installed to
`/etc/reader.conf.d/`.

## How to build

Typical build/install sequence is as follows:

```
mkdir build || exit 1
cd build || exit 1
cmake .. || exit 1
make || exit 1
make install || exit 1
```

#### CMake configuration variables

The product configuration may be controlled by setting the following variables when cmake configuration step is performed:
* `-DRTUARTSCREADER_SERIAL_PORT=/serial/port/device/path` allows to specify the path to serial device set up into `librtuartscreader` configuration file. The device is expected to correspond to UART transmitter connected to the card. Default value is `/dev/ttyS0`.
* `-DRTUARTSCREADER_BUILD_TESTS=OFF` allows to disable building of unit tests. By default the tests will be built. Its install path is `/usr/local/bin/`
* `-DRTUARTSCREADER_RUN_TESTS=OFF` allows to disable execution of the unit tests during the build. By default the tests will be executed if target machine processor architecture is the same as the host.

#### Cross-compilation

[Documentation for cmake](https://cmake.org/cmake/help/v3.6/manual/cmake-toolchains.7.html#id6) gives a comprehensive overview how to cross-compile cmake projects. Below is an example cmake-toolchain file that may be used to compile the project for Raspberry Pi 3 with the toolchain generated with crosstool-ng:

```
set(TARGET_TRIPLE glibc-armv7hf-gcc8)

set(CMAKE_SYSTEM_NAME Linux)

set(TOOLCHAIN_DIR "/opt/x-tools/${TARGET_TRIPLE}/bin/")

string(REPLACE "-" ";" TARGET_TRIPLE_LIST ${TARGET_TRIPLE})
list(GET TARGET_TRIPLE_LIST 1 TARGET_PROCESSOR)

file(GLOB GCC RELATIVE ${TOOLCHAIN_DIR} ${TOOLCHAIN_DIR}*-gcc)
string(REPLACE -gcc "" CROSS_COMPILE_TOOL_PREFIX ${GCC})

set(CROSS_COMPILE_XTOOLS ${TOOLCHAIN_DIR}${CROSS_COMPILE_TOOL_PREFIX})
set(CMAKE_C_COMPILER   ${CROSS_COMPILE_XTOOLS}-gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE_XTOOLS}-g++)

set(CMAKE_SYSTEM_PROCESSOR ${TARGET_PROCESSOR})

set(CMAKE_FIND_ROOT_PATH  ${TOOLCHAIN_DIR})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

It's required that CMAKE_SYSTEM_PROCESSOR variable is correctly defined. Fully functional driver will be built if only the value of this variable starts with "arm". Otherwise, the unit tests only will be built.

You may use the following commands to perform cross-comilation of the project for Raspberry Pi 3 (if the cmake-toolchain file is located at `/opt/cmake-toolchain/glibc-armv7hf-gcc8.cmake`):

```
mkdir build && cd build || exit 1
cmake -DCMAKE_TOOLCHAIN_FILE=/opt/cmake-toolchain/glibc-armv7hf-gcc8.cmake -DRTUARTSCREADER_RUN_TESTS=OFF -DRTUARTSCREADER_SERIAL_PORT=/dev/ttyAMA0 .. || exit 1
make install DESTDIR=`pwd`/glibc-armv7hf-gcc8/ || exit 1
```

## librtuartscreader configuration file

`librtuartscreader` configuration file contains the following values:
* `DEVICENAME` -- path to serial port device, which smartcard connector is connected to. In application to [Rutoken M2M](https://www.rutoken.ru/products/all/rutoken-m2m/) demonstration kit the value must be `/dev/ttyAMA0`.
* `FRIENDLYNAME` -- prefix for the reader name used to identify smartcard in PCSC API. By default the value is `Aktiv Rutoken UART SC Reader`.
* `LIBPATH` -- path to driver library `librtuartscreader.so`. By default the value is `/usr/lib/pcsc/drivers/serial/librtuartscreader.so`.

For more information about serial reader IFD Handler configuration file see [pcsc-lite documentation](https://pcsclite.apdu.fr/api/group__IFDHandler.html#details).

## Debugging

The driver is capable of providing debug information using pcscd built-in logging mechanism. The log destination
depends on pcscd settings and run mode. If pcscd is executed in foreground mode, debug output is printed to stdout. If pcscd is in background mode, syslog is used, so that debug output can be found in `/var/log/messages`.

The verbosity of log may be controlled by setting `LIBRTUARTSCREADER_ifdLogLevel` environment variable.
The value of variable is calculated as a sum of values representing discrete log levels:
* 1 -- log critical errors;
* 2 -- log errors;
* 4 -- log info messages;
* 8 -- log periodic messages (these occur because pcscd polls for a card status change).

Both decimal and hex values of `LIBRTUARTSCREADER_ifdLogLevel` variable are supported.

Default log level is `3`, which means critical and recoverable errors will be logged.

To start `pcscd` in foreground with the log level including critical, recoverable errors and information messages one may do
the following call:
`sudo LIBRTUART_ifdLogLevel=7 pcscd -afd`.

## License

Project is distributed under [2-clause BSD License](LICENSE) except for the parts explicitly specified below.

The files in [PCSC](PCSC/include/PCSC) directory containing pcsc-lite public headers are distributed under 3-clause BSD
License, see the corresponding source files for license text and copyright notices.

The source code of the dependency projects from [3rdparty](3rdparty) directory and [pigpio](pigpio) project are distributed under their original licenses, the text of which can be found in the corresponding project directory.
