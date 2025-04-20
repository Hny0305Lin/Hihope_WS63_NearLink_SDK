set(COMPONENT_NAME "course_experiment")

set(SOURCES
)

set(PUBLIC_HEADER
)
if(DEFINED CONFIG_ENABLE_EXPERIMENT_1)
    add_subdirectory_if_exist(experiment1)
endif()
if(DEFINED CONFIG_ENABLE_EXPERIMENT_3)
    add_subdirectory_if_exist(experiment3)
endif()
if(DEFINED CONFIG_ENABLE_EXPERIMENT_4)
    add_subdirectory_if_exist(experiment4) 
endif()

add_subdirectory_if_exist(custom)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
