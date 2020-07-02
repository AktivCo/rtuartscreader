[Английский/English](README_EN.md)

## Описание

**Rtuartscreader** -- это реализация драйвера пользовательского уровня ([IFD Handler](https://pcsclite.apdu.fr/api/group__IFDHandler.html#details)), предназначенного для обеспечения возможности взаимодействия с картами [Рутокен 2151 и Рутокен 2010](https://www.rutoken.ru/products/all/rutoken-m2m/#models) при помощи аппаратного приемопередатчика UART и контактов общего назначения микроконтроллера. Драйвер предназначен для работы в демонстрационном комплекте [Рутокен M2M](https://www.rutoken.ru/products/all/rutoken-m2m/), базирующемся на ПК Raspberry Pi 2 или 3.

Артефакты проекта:
* `librtuartscreader.so` -- библиотека драйвера. По умолчанию устанавливается в `/usr/lib/pcsc/drivers/serial/`.
* `librtuartscreader` -- файл конфигурации драйвера, используемый pcscd для загрузки драйвера. По умолчанию устанавливается в `/etc/reader.conf.d/`.

## Сборка

Типичная последовательность действий для сборки и установки:

```
mkdir build || exit 1
cd build || exit 1
cmake .. || exit 1
make || exit 1
make install || exit 1
```

#### Переменные конфигурации сборки проекта CMake

Следующие переменные, указанные при конфигурации cmake-проекта, повлияют на его сборку:
* `-DRTUARTSCREADER_SERIAL_PORT=/serial/port/device/path` позволяет указать путь до файла устройства последовательного порта, который будет использоваться для взаимодействия со смарт-картой по протоколу UART. По умолчанию значение переменной `/dev/ttyS0`.
* `-DRTUARTSCREADER_BUILD_TESTS=OFF` позволяет выключить сборку юнит-тестов. По умолчанию тесты собираются и будут установлены по пути `/usr/local/bin/`
* `-DRTUARTSCREADER_RUN_TESTS=OFF` позволяет выключить выполнение юнит-тестов как один из шагов сборки. По умолчанию, если архитектура процессора, под который собирается проект, совпадает с архитектурой процессора ПК, на котором собирается проект, во время сборки будут выполнены юниттесты.

#### Кросс-компиляция

Действия, необходимые для кросс-компиляции проекта описаны в [официальной документации cmake](https://cmake.org/cmake/help/v3.6/manual/cmake-toolchains.7.html#id6). Ниже приведен пример cmake-toolchain файла для компиляции проекта под Raspberry Pi 3 при помощи crosstool-ng:

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

Требуется обязательно указывать значение переменной CMAKE_SYSTEM_PROCESSOR. Полнофункциональный драйвер собирается только в случае, если значение этой переменной начинается на "arm". В противном случае собираются только юнит-тесты.

Кросс-компиляция проекта под Raspberry Pi 3 следующим набором команд (cmake-toolchain-файл расположен по пути `/opt/cmake-toolchain/glibc-armv7hf-gcc8.cmake`):

```
mkdir build && cd build || exit 1
cmake -DCMAKE_TOOLCHAIN_FILE=/opt/cmake-toolchain/glibc-armv7hf-gcc8.cmake -DRTUARTSCREADER_RUN_TESTS=OFF -DRTUARTSCREADER_SERIAL_PORT=/dev/ttyAMA0 .. || exit 1
make install DESTDIR=`pwd`/glibc-armv7hf-gcc8/ || exit 1
```

## Конфигурационный файл librtuartscreader

Конфигурационный файл `librtuartscreader` содержит следующие значения:
* `DEVICENAME` -- путь к файлу устройства последовательного порта, к которому подключен считыватель смарт-карт. В демонстрационном комплекте [Рутокен M2M](https://www.rutoken.ru/products/all/rutoken-m2m/) значение должно быть `/dev/ttyAMA0`.
* `FRIENDLYNAME` -- базовое имя считывателя, используемое для идентификации смарткарт, работающих через данный драйвер, в API PCSC. По умолчанию установлено в `Aktiv Rutoken UART SC Reader`.
* `LIBPATH` -- путь к библиотеке драйвера `librtuartscreader.so`. По умолчанию установлено в `/usr/lib/pcsc/drivers/serial/librtuartscreader.so`.

Больше информации о конфигурационном файле можно найти в [документации pcsc-lite](https://pcsclite.apdu.fr/api/group__IFDHandler.html#details).

## Отладочный вывод

Драйвер выполняет вывод отладочной информации с использованием встроенного в pcscd механизма логирования. Куда будет писаться лог, зависит от режима запуска и настроек pcscd. В случае, если pcscd запущен в foreground-режиме, отладочный вывод перенаправляется в stdout. В background-режиме используется syslog -- отладочный вывол попадает в файл `/var/log/messages`.

Подробность отладочного вывода может регулироваться посредством установки значения переменной окружения `LIBRTUARTSCREADER_ifdLogLevel`. Значение этой переменной вычисляется как сумма значений, соответствующих выбранным дискретным уровням логирования:
* 1 -- логировать сообщения о критических ошибках;
* 2 -- логировать сообщения об ошибках;
* 4 -- логировать информационные сообщения;
* 8 -- логировать периодические сообщения (возникают из-за опроса драйвера о состоянии подключения карты со стороны pcscd).

Поддерживаются как десятичные, так и шестнадцатиричные значения переменной `LIBRTUARTSCREADER_ifdLogLevel`.

По умолчанию уровень отладочного вывода равен `3`, что соответствует логированию критических сообщений и сообщений об ошибках.

Запустить `pcscd` в foreground-режиме с уровнем логирования, обеспечивающим вывод информации о критических ошибках, просто ошибках и информационных сообщений, можно следующим образом: `sudo LIBRTUARTSCREADER_ifdLogLevel=7 pcscd -afd`.

## Лицензия

Проект распространяется по [двухпунктной лицензии BSD](LICENSE), за исключением составляющих, о лицензиях которых написано ниже.

Публичные заголовочные файлы pcsc-lite в директории [PCSC](PCSC/include/PCSC) распространяются по оригинальной трехпунктной лицензии BSD; текст лицензии и указания авторства размещены в самих заголовочных файлах.

Исходный код проектов-зависимостей в директории [3rdparty](3rdparty) и проекта [pigpio](pigpio) распространяются по своим оригинальным лицензиям, расположенным внутри директорий этих проектов.
