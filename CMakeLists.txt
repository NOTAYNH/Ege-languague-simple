cmake_minimum_required(VERSION 3.15)
project(jse_native)

# Kütüphanenin yerini elle node_modules içinde arıyoruz
include_directories(/workspaces/Ege-languague-simple/node_modules/node-addon-api)

add_library(${PROJECT_NAME} SHARED "jse_native.cc")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Standart Node.js include yolları
include_directories(/home/codespace/.cmake-js/node-x64/v24.11.1/include/node)

target_compile_definitions(${PROJECT_NAME} PRIVATE NAPI_DISABLE_CPP_EXCEPTIONS)
