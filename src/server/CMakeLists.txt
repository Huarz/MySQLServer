
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)

add_executable(SQLServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST})

target_link_libraries(SQLServer muduo_net muduo_base mysqlclient pthread)