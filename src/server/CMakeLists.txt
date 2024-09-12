# 加载所有目录下源文件
aux_source_directory(../db DB_LIST)
aux_source_directory(../model MODEL_LIST)
aux_source_directory(../redis REDIS_LIST)
aux_source_directory(. SERVER_LIST)

# 指定生成可执行文件
add_executable(ChatServer ${DB_LIST} ${MODEL_LIST} ${SERVER_LIST} ${REDIS_LIST})

target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)