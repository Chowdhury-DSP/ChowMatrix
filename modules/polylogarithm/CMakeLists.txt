add_library(polylogarithm STATIC
    Li2.cpp
)

# Need to build with -fPIC flag on Linux
set_target_properties(polylogarithm PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
