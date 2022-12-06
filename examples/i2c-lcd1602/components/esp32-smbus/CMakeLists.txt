# For IDF v4.x:
#idf_component_register(SRCS "smbus.c" INCLUDE_DIRS include)

# set(COMPONENT_ADD_INCLUDEDIRS ".")
# set(COMPONENT_SRCS "smbus.c")
# register_component()

list(APPEND srcs
        "smbus.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                                    "./include"
                                    "../../../../components/driver/include")

