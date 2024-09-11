aux_source_directory(. CLIENT_LIST)

add_executable(ChatClient ${CLIENT_LIST} ${MODEL_LIST})

target_link_libraries(ChatClient pthread)