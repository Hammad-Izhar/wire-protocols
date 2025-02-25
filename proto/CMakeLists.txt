set(PROTO_DIR "${CMAKE_SOURCE_DIR}/proto")
set(PROTO_FILES ${PROTO_DIR}/socketout.proto)

message(STATUS "Generating protobuf files into ${GEN_DIR}/proto:")
foreach(PROTO_FILE ${PROTO_FILES})
    message(STATUS "  - ${PROTO_FILE}")    
endforeach()


add_library(socketout_proto STATIC ${PROTO_FILES})
target_link_libraries(socketout_proto PUBLIC gRPC::grpc++ protobuf::libprotobuf)
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

protobuf_generate(
    TARGET socketout_proto
    LANGUAGE cpp
    PROTOS ${PROTO_FILES}
)

protobuf_generate(
    TARGET socketout_proto
    LANGUAGE grpc
    PROTOS ${PROTO_FILES}
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc 
    PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)