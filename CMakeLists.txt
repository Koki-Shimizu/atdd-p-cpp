cmake_minimum_required(VERSION 3.15)
project(ATDD_Practice CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# メインプログラム
add_executable(hello_world src/main.cpp)

# Catch2テストフレームワークのダウンロードと設定
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.0
)
FetchContent_MakeAvailable(Catch2)

# テスト実行ファイル
add_executable(tests tests/test_main.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2)

# Catch2のテストを有効化
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)

