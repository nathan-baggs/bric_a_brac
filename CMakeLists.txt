cmake_minimum_required(VERSION 3.18)

project(
  bric_a_brac
  VERSION "0.1.0"
  DESCRIPTION "Custom game engine"
  LANGUAGES CXX)

include(FetchContent)
include(GenerateExportHeader)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("src")
add_subdirectory("samples")
