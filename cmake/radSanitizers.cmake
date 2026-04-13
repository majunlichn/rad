include_guard()

option(ENABLE_ASAN "Enable AddressSanitizer (ASan)" OFF)

if(ENABLE_ASAN)
    if(MSVC)
        # https://learn.microsoft.com/en-us/cpp/sanitizers/asan
        add_compile_options(/fsanitize=address /Zi)
        add_link_options(/INCREMENTAL:NO)
    endif()
endif()
