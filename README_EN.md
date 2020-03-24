[Russian/Русский](README_RUS.md)

## Description

Rtuart is an implementation of user-space driver ([IFD Handler](https://pcsclite.apdu.fr/api/group__IFDHandler.html#details))
for [pcsc-lite](https://pcsclite.apdu.fr/) to provide access via PCSC interface to
[Rutoken 4010](https://www.rutoken.ru/products/all/rutoken-m2m/#models) connected over serial interface.

The artifacts of the project are:
* `librtuart.so` -- driver library. By default it gets installed to `/usr/lib/pcsc/drivers/serial/`.
* `librtuart` -- driver configuration file used by pcscd to start up the driver. By default it gets installed to
`/etc/reader.conf.d/`.
* `rtuart_transport_test` -- executable to test low-level protocol used for interaction with the smartcard.
By default it gets installed to `/usr/bin/`.

## How to build

Typical build/install sequence is as follows:

```
mkdir build || exit -1
cd build || exit -1
cmake .. || exit -1
make || exit -1
make install || exit -1
```

#### CMake definitions

* Use `cmake -DRTUART_SERIAL_PORT=/serial/port/device/path ..` to specify the path to serial device set up into `librtuart`
configuration file. Default value is `/dev/ttyS0`.

## librtuart configuration file

`librtuart` configuration file contains the following values:
* `DEVICENAME` -- path to serial port device, which Rutoken 4010 smartcard is connected to.
* `FRIENDLYNAME` -- prefix for the reader name used to identify smartcard in PCSC API.
* `LIBPATH` -- path to driver library `librtuart.so`.

For more information see [pcsc-lite documentation](https://pcsclite.apdu.fr/api/group__IFDHandler.html#details).

## rtuart_transport_test

`rtuart_transport_test` performs interaction with Rutoken 4010 smartcard over low-level protocol. To run the test you need to specify
serial port device path as the first argument of executable, e.g. `rtuart_transport_test /dev/ttyS0`. The test tries to open device file,
so elevated privileges may be required. Moreover, the test can not be run the same time the driver is loaded.

## Debugging

The driver is capable of providing debug information using pcscd built-in logging mechanism. The log destination
depends on pcscd settings and run mode; it is written to stdout when pcscd is running in foreground mode.

The verbosity of log can be controlled by setting `LIBRTUART_ifdLogLevel` environment variable.
The value of variable is calculated as a sum of values representing discrete log levels:
* 1 -- log critical errors;
* 2 -- log errors;
* 4 -- log info messages;
* 8 -- log debug messages (including periodic ones).

Both decimal and hex values of `LIBRTUART_ifdLogLevel` variable are supported.

Default log level is `3`, which means critical and recoverable errors will be logged.

To start `pcscd` in foreground with the log level including critical, recoverable errors and information messages one can do
the following call:
`sudo LIBRTUART_ifdLogLevel=7 pcscd -afd`.

## License

Project is distributed under [2-clause BSD License](LICENSE).

The files in [PCSC](PCSC/include/PCSC) directory containing pcsc-lite public headers are distributed under 3-clause BSD
License, see the corresponding source files for license text and copyright notices.