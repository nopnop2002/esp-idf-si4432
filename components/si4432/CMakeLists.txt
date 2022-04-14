set(component_srcs "si4432.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS ".")
