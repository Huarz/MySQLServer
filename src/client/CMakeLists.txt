aux_source_directory(. SRC_LIST)

add_executable(SQLClient ${SRC_LIST})
target_link_libraries(SQLClient pthread)