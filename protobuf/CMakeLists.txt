find_package(protobuf REQUIRED CONFIG)

if (protobuf_FOUND)
    message(STATUS "Using Protocol Buffers ${protobuf_VERSION}")
endif()

file(GLOB PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)

set(PROTO_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")
make_directory(${PROTO_BINARY_DIR})


foreach(PROTO_FILENAME ${PROTO_FILES})
    get_filename_component(PROTO_NAME_WE ${PROTO_FILENAME} NAME_WE)
    add_library(${PROTO_NAME_WE} OBJECT ${PROTO_FILENAME})
    target_link_libraries(${PROTO_NAME_WE} PUBLIC protobuf::libprotobuf)
    target_include_directories(${PROTO_NAME_WE} PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")
    protobuf_generate(
        TARGET ${PROTO_NAME_WE}
        IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}"
        PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
    )
endforeach()