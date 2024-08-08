
include(function_alg.cmake)

if(${project_name} MATCHES "ws73" OR ${project_name} MATCHES "ws63" OR ${project_name} MATCHES "ws53")
add_subdirectory(iot_alg)
endif()
