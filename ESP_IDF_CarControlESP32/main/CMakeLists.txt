set(srcs
    "bt_handler.c"
    "main.c")
    
idf_component_register( SRCS "${srcs}"
                        INCLUDE_DIRS ".")