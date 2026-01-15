cmake_minimum_required(VERSION 3.15)
project(jse_native)

# Node.js başlık dosyalarını cmake-js zaten sağlıyor, 
# biz sadece node-addon-api (napi.h) klasörünü bulmalıyız.
execute_process(COMMAND node -p "require('node-addon-api').include"
                OUTPUT_VARIABLE NODE_ADDON_API_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# Tırnak işaretlerini kaldırmak için string temizliği
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

add_library(${PROJECT_NAME} SHARED "jse_native.cc")

# Çıktı dosyasını .node yap
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Kütüphane yolunu ekle
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

# Node.js başlıklarını ekle (cmake-js'den gelen)
if(CMAKE_JS_INC)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE NAPI_DISABLE_CPP_EXCEPTIONS)