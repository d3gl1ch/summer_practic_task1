function(get_all_sources dir result_var)
    file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
        "${dir}/*.c"
        "${dir}/*.h"
    )
    set(${result_var} ${SOURCE_FILES} PARENT_SCOPE)
endfunction()