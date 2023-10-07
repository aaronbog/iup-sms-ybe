add_executable(ybe_sms "main.cpp" "clause.cpp"  "useful.cpp" "solveCadicalClass.cpp" "solveGeneral.cpp" "minimalityCheck.cpp")
target_link_libraries(ybe_sms PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../cadical-user-propagation/build/libcadical.a")

include_directories(${Boost_INCLUDE_DIRS}) 
target_link_libraries(ybe_sms PRIVATE ${Boost_LIBRARIES})

target_compile_options(ybe_sms PRIVATE "-Wall" "-Wextra" "-pedantic" "-O3")
