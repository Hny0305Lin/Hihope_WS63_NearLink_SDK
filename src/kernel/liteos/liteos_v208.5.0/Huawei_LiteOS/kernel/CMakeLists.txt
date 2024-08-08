
set(MODULE_y base extended init)
FOREACH(CUR_MODULE_y ${MODULE_y})
    add_subdirectory(${CUR_MODULE_y})
ENDFOREACH(CUR_MODULE_y)
