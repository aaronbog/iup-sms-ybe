find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_executable(ybe_sms "main.cpp" "clause.cpp"  "useful.cpp" "solveCadicalClass.cpp" "solveGeneral.cpp" "minimalityCheck_V2.cpp" "minimalityCheck.cpp")
target_link_libraries(ybe_sms PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../cadical-user-propagation/build/libcadical.a")

include_directories(${Boost_INCLUDE_DIRS}) 
target_link_libraries(ybe_sms PRIVATE ${Boost_LIBRARIES})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp") 

target_compile_options(ybe_sms PRIVATE "-Wall" "-Wextra" "-pedantic" "-O3")
