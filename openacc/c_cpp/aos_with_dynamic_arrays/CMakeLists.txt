project(aos_with_dynamic_arrays)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cxx)
set(HEADER_DIR ${CMAKE_CURRENT_LIST_DIR})
file(GLOB HEADER_FILES ${HEADER_DIR}/*.h)

include_directories(${HEADER_DIR})

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "aos_with_dynamic_arrays")

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES ${HEADER_FILES} DESTINATION include/${PROJECT_NAME})