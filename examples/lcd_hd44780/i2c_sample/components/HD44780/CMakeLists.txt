# set(COMPONENT_ADD_INCLUDEDIRS ".")
# set(COMPONENT_SRCDIRS ".")
# register_component()
list(APPEND srcs
        "HD44780.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                                    "./include"
                                    "../../../../../components/driver/include")

