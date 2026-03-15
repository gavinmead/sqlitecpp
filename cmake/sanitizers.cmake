# Sanitizer support

function(target_enable_sanitizers target)
    if(NOT SQLITECPP_ENABLE_SANITIZERS)
        return()
    endif()

    set(SANITIZER_FLAGS "")

    if(SQLITECPP_SANITIZER STREQUAL "address")
        set(SANITIZER_FLAGS "-fsanitize=address,undefined" "-fno-omit-frame-pointer")
    elseif(SQLITECPP_SANITIZER STREQUAL "thread")
        set(SANITIZER_FLAGS "-fsanitize=thread")
    elseif(SQLITECPP_SANITIZER STREQUAL "undefined")
        set(SANITIZER_FLAGS "-fsanitize=undefined")
    elseif(SQLITECPP_SANITIZER STREQUAL "memory")
        set(SANITIZER_FLAGS "-fsanitize=memory" "-fno-omit-frame-pointer")
    endif()

    target_compile_options(${target} PRIVATE ${SANITIZER_FLAGS})
    target_link_options(${target} PRIVATE ${SANITIZER_FLAGS})
endfunction()
