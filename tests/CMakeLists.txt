include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE)

# Honour the parent project's compiler/runtime settings on Windows.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(lambdaplusplus_tests
    compose.t.cpp
    attempt.t.cpp
    nothrow.t.cpp
    match.t.cpp)

target_link_libraries(lambdaplusplus_tests
    PRIVATE
        lambdaplusplus::lambdaplusplus
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(lambdaplusplus_tests)
