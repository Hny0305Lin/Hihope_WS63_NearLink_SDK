######### if LOCAL_SRCS is NULL, return right now #########
if(NOT LOCAL_SRCS)
    return()
endif()

######### record libs' path and name to LITEOS_MODULE_LIBS_PATH_PROP #########
if(CMAKE_ARCHIVE_OUTPUT_DIRECTORY) # priority option
    set_property(GLOBAL APPEND PROPERTY LITEOS_MODULE_LIBS_PATH_PROP ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/lib${MODULE_NAME}.a)
else()
    # if no defined LIB_PATH, use default lib_path
    set_property(GLOBAL APPEND PROPERTY LITEOS_MODULE_LIBS_PATH_PROP ${CMAKE_CURRENT_BINARY_DIR}/lib${MODULE_NAME}.a)
endif()


####################### support only c asm and cpp files #######################
# add global include, compile macro and options to ${MODULE_NAME}${LOS_PUB_INTF_SUFFIX} #
set(MODULE_CONFIG_PUBLIC "${MODULE_NAME}${LOS_PUB_INTF_SUFFIX}")
add_library(${MODULE_CONFIG_PUBLIC} INTERFACE)

# set syscalls.h include path for user space
list(APPEND MODULE_INCLUDE_PRI ${LITEOS_U_INCLUDES})
list(APPEND MODULE_CXXINCLUDE_PRI ${LITEOS_U_INCLUDES})

# *.S files are treated as c-files, while *.s files are treated as asm-files, referring to module.mk
target_include_directories(${MODULE_CONFIG_PUBLIC} INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${MODULE_INCLUDE_PUB}>
    $<$<COMPILE_LANGUAGE:ASM>:${MODULE_INCLUDE_PUB}>
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CXXINCLUDE_PUB}>
)

target_compile_options(${MODULE_CONFIG_PUBLIC} INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${MODULE_COPTS_PUB}>
    $<$<COMPILE_LANGUAGE:ASM>:${MODULE_ASOPTS_PUB}>
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CXXOPTS_PUB}>
)

target_compile_definitions(${MODULE_CONFIG_PUBLIC} INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${MODULE_CMACRO_PUB}>
    $<$<COMPILE_LANGUAGE:ASM>:${MODULE_ASMACRO_PUB}>
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CMACRO_PUB}${MODULE_CXXMACRO_PUB}>
)

target_link_libraries(${MODULE_CONFIG_PUBLIC} INTERFACE
)


# add local include, compile macro and options to ${MODULE_NAME}${LOS_PRI_INTF_SUFFIX} #
set(MODULE_CONFIG_PRIVATE "${MODULE_NAME}${LOS_PRI_INTF_SUFFIX}")
add_library(${MODULE_CONFIG_PRIVATE} INTERFACE)

target_include_directories(${MODULE_CONFIG_PRIVATE} INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${MODULE_INCLUDE_PRI}>
    $<$<COMPILE_LANGUAGE:ASM>:${MODULE_INCLUDE_PRI}>
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CXXINCLUDE_PRI}>
)

FOREACH(TEMP_SRCS_FILES ${LOCAL_SRCS})
    get_filename_component(FILE_TYPE ${TEMP_SRCS_FILES} LAST_EXT)
    if(FILE_TYPE MATCHES "^\.S$")
        # *.S files are treated as c-files, while *.s files are treated as asm-files, referring to module.mk
        set_source_files_properties(${TEMP_SRCS_FILES} PROPERTIES LANGUAGE C)
    endif()
    if(FILE_TYPE MATCHES "^\.S$" OR FILE_TYPE MATCHES "^\.s$")
        # add private compile definitions and options only for *.S and *.s files
        if(MODULE_ASOPTS_PRI)
            set_source_files_properties(${TEMP_SRCS_FILES} PROPERTIES
                COMPILE_OPTIONS "${MODULE_ASOPTS_PRI}")
        endif()
        if(MODULE_ASMACRO_PRI)
            set_source_files_properties(${TEMP_SRCS_FILES} PROPERTIES
                COMPILE_DEFINITIONS "${MODULE_ASMACRO_PRI}")
        endif()
    elseif(FILE_TYPE MATCHES "^\.c$")
        # add private compile definitions and options only for *.c files
        if(MODULE_COPTS_PRI)
            set_source_files_properties(${TEMP_SRCS_FILES} PROPERTIES
                COMPILE_OPTIONS "${MODULE_COPTS_PRI}")
        endif()
        if(MODULE_CMACRO_PRI)
            set_source_files_properties(${TEMP_SRCS_FILES} PROPERTIES
                COMPILE_DEFINITIONS "${MODULE_CMACRO_PRI}")
        endif()
    endif()
ENDFOREACH(TEMP_SRCS_FILES)

# refer to cmake target_compile_features or generator expression for more details
target_compile_options(${MODULE_CONFIG_PRIVATE} INTERFACE
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CXXOPTS_PRI}>
)

target_compile_definitions(${MODULE_CONFIG_PRIVATE} INTERFACE
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CMACRO_PRI}${MODULE_CXXMACRO_PRI}>
)

target_link_libraries(${MODULE_CONFIG_PRIVATE} INTERFACE
)


# add srcs in the module to ${MODULE_NAME}, generating lib${MODULE_NAME}.a #
list(SORT LOCAL_SRCS)
add_library(${MODULE_NAME} STATIC ${LOCAL_SRCS})

target_link_libraries(${MODULE_NAME} PRIVATE
    $<BUILD_INTERFACE:${MODULE_CONFIG_PRIVATE}>
    $<BUILD_INTERFACE:${LOS_CC_PROP_INTF_PUB}>
    $<BUILD_INTERFACE:${VENDOR_CC_PROP_INTF_PUB}>
)

set_target_properties(${MODULE_NAME}
    PROPERTIES
    OUTPUT_NAME $<IF:$<STREQUAL:${OS_TYPE},windows>,lib${MODULE_NAME},${MODULE_NAME}>
)

if(LOSCFG_SAFETY_KERNEL)
set(MODULE_BIGO_NAME lib${MODULE_NAME}.O)
add_executable(${MODULE_BIGO_NAME} ${LOCAL_SRCS})
target_link_options(${MODULE_BIGO_NAME} PRIVATE -r -nostdlib)

target_link_libraries(${MODULE_BIGO_NAME} PRIVATE
    $<BUILD_INTERFACE:${MODULE_CONFIG_PRIVATE}>
    $<BUILD_INTERFACE:${LOS_CC_PROP_INTF_PUB}>
    $<BUILD_INTERFACE:${VENDOR_CC_PROP_INTF_PUB}>
)

set_target_properties(${MODULE_BIGO_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
endif()
