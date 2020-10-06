cmake_minimum_required(VERSION 2.8.3)
project(ovc4_host)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

include_directories(
  include/
  ${OpenCV_INCLUDE_DIRS}
#  ${ignition-msgs1_INCLUDE_DIRS}
#  ${ignition-transport4_INCLUDE_DIRS}
)

add_library(ovc4_host
  src/subscriber.cpp
)

target_link_libraries(ovc4_host ${OpenCV_LIBS})

add_executable(ovc4_host_node src/ovc4_host_driver.cpp)

target_link_libraries(ovc4_host_node
  Threads::Threads
  ${OpenCV_LIBS}
  ovc4_host
  )

#target_link_libraries(${PROJECT_NAME}_node
#  ${ignition-msgs1_LIBRARIES}
#  ${ignition-transport4_LIBRARIES}
#)

