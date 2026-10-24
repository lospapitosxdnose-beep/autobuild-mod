cmake_minimum_required(VERSION 3.25)
project(AutoBuild VERSION 1.0.0)

add_library(AutoBuild SHARED src/main.cpp)
target_compile_features(AutoBuild PUBLIC cxx_std_23)
setup_geode_mod(AutoBuild)
