set(SRC_LIST callerUserservice.cc ../user.pb.cc)
add_executable(consumer ${SRC_LIST})

target_link_libraries(consumer mprpc protobuf)