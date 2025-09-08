project(self_inst_adv_f)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.F90)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "self_inst_adv_f")

install(TARGETS ${PROJECT_NAME} DESTINATION bin)