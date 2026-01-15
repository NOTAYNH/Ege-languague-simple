cmake_minimum_required(VERSION 3.15)
project(jse_native)

# 1. Kütüphane klasörünü bul
execute_process(COMMAND node -p "require('node-addon-api').include"
                OUTPUT_VARIABLE NODE_ADDON_API_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

add_library(${PROJECT_NAME} SHARED "jse_native.cc")

# 2. Windows için kritik kütüphane bağlama ayarları
if(WIN32)
    # cmake-js tarafından indirilen node.lib dosyasını bul ve bağla
    if(NOT CMAKE_JS_LIB)
        message(FATAL_ERROR "CMAKE_JS_LIB tanımlı değil! Derleme durduruldu.")
    endif()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_JS_LIB})
endif()

# 3. Dosya ayarları
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# 4. Include yolları
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})
if(CMAKE_JS_INC)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE NAPI_DISABLE_CPP_EXCEPTIONS)