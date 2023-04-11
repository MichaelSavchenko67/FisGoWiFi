cmake_minimum_required(VERSION 3.0)

#####################################################
# Выбор платформы для сборки Ф/РФ
#set(FISGO_DEVICE_PLATFORM "Dreamkas-F")
set(FISGO_DEVICE_PLATFORM "Dreamkas-RF")
#####################################################

if(${FISGO_DEVICE_PLATFORM} STREQUAL "Dreamkas-RF")
    message("RF device!!!")
    #dreamkas RF
    set(CMAKE_CXX_COMPILER /usr/local/usr/bin/arm-linux-g++)
    set(CMAKE_C_COMPILER /usr/local/usr/bin/arm-linux-gcc)

    project(fisgo_wifi)

    set(CMAKE_C_FLAGS   "-std=gnu99")
    set(CMAKE_CXX_FLAGS "-std=c++11 -Werror -Wno-pmf-conversions -Wall")

    set(CMAKE_LIBRARY_PATH "/usr/local/usr/arm-buildroot-linux-uclibcgnueabi/sysroot/lib")

    include_directories(/usr/local/arm_linux_4.8/arm-nuvoton-linux-uclibceabi/include/sqlite3/)

    set(CMAKE_EXE_LINKER_FLAGS "-lsqlite3 -liconv")

    add_definitions(-DDREAMKAS_RF)
else()
    message("F device!!!")
    #dreamkas F
    set(CMAKE_CXX_COMPILER arm-linux-g++)
    set(CMAKE_C_COMPILER arm-linux-gcc)

    project(fisgo_wifi)

    set(CMAKE_C_FLAGS   "-std=gnu99")
    set(CMAKE_CXX_FLAGS "-std=c++11 -Werror -Wno-pmf-conversions -Wall")

    include_directories(/usr/local/arm_linux_4.8/arm-nuvoton-linux-uclibceabi/include/sqlite3/)

    set(CMAKE_EXE_LINKER_FLAGS "-lsqlite3 -liconv")

    add_definitions(-DDREAMKAS_F)
endif()

include_directories(src/include)
aux_source_directory(src/ srcDir)

file(GLOB appHead src/include/*.h)

add_library(fisgo_wifi SHARED ${srcDir} ${appHead})
#add_executable(fisgo_wifi ${srcDir} ${appHead})
