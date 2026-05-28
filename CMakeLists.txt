cmake_minimum_required(VERSION 3.20)
project(OrderBook)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(CMAKE_MAKE_PROGRAM "C:/mingw64/bin/mingw32-make.exe" CACHE PATH "Path to make")

add_executable(orderbook 
    main.cpp
    src/order_book.cpp
    src/order.cpp
    src/price_level.cpp
    src/timer.cpp
)

set_target_properties(orderbook PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")