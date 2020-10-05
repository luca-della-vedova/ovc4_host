cmake_minimum_required(VERSION 2.8.3)
project(ovc4_host)

include_directories(
  include/
#  ${ignition-msgs1_INCLUDE_DIRS}
#  ${ignition-transport4_INCLUDE_DIRS}
)

add_library(ovc4_host
  src/subscriber.cpp
)

add_executable(ovc4_host_node src/ovc4_host_driver.cpp)

#target_link_libraries(${PROJECT_NAME}_node
#  ${ignition-msgs1_LIBRARIES}
#  ${ignition-transport4_LIBRARIES}
#)

