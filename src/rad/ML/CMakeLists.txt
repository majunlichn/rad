set(RAD_ML_HEADERS
    MLBackend.h
    MLGlobal.h
    MLBuffer.h
    MLCommon.h
    MLContext.h
    MLDevice.h
    MLOp.h
    MLTensor.h
)

set(RAD_ML_SOURCES
    MLBackend.cpp
    MLGlobal.cpp
    MLBuffer.cpp
    MLCommon.cpp
    MLContext.cpp
    MLDevice.cpp
    MLOp.cpp
    MLTensor.cpp
)

set(RAD_ML_CPU_HEADERS
    Cpu/CpuMLBackend.h
    Cpu/CpuMLBuffer.h
    Cpu/CpuMLContext.h
    Cpu/CpuMLDevice.h
    Cpu/CpuMLTensor.h
)

set(RAD_ML_CPU_SOURCES
    Cpu/CpuMLBackend.cpp
    Cpu/CpuMLBuffer.cpp
    Cpu/CpuMLContext.cpp
    Cpu/CpuMLDevice.cpp
    Cpu/CpuMLTensor.cpp
)

add_library(rad_ml STATIC)

target_sources(rad_ml PRIVATE ${RAD_ML_HEADERS} ${RAD_ML_SOURCES} ${RAD_ML_CPU_HEADERS}
    ${RAD_ML_CPU_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_ML_HEADERS} ${RAD_ML_SOURCES}
    ${RAD_ML_CPU_HEADERS} ${RAD_ML_CPU_SOURCES})

target_link_libraries(rad_ml
    PUBLIC rad_common
    PUBLIC rad_container
    PUBLIC rad_io
    PRIVATE rad_system
)

set(RAD_ML_TEST_SOURCES
    ML.test.h
    ML.test.cpp
    Cpu/CpuMLBackend.test.cpp
    Cpu/CpuMLTensor.test.cpp
)

if(RAD_BUILD_TESTS)
    add_executable(rad_ml_tests)
    target_sources(rad_ml_tests PRIVATE ${RAD_ML_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_ML_TEST_SOURCES})
    target_link_libraries(rad_ml_tests
        PRIVATE rad_ml
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_ml_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_ml_tests)
endif()
