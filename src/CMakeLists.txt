find_package(fmt CONFIG REQUIRED)
find_library(LIB_CPR cpr)

#----- Include LibStein
include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
    LibStein
    GIT_REPOSITORY "https://github.com/johnstsimoes/libstein.git"
    GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(LibStein)
include_directories(${LibStein_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}
    main.cpp
    util.cpp
    settings.cpp
    lua_jira.cpp
    jira/jira_components.cpp
    jira/jira_jql.cpp)

target_link_libraries(${PROJECT_NAME}
    LINK_PUBLIC
    ${LIB_CPR}
    fmt::fmt
    LibStein
    readline
    lua)
    # ${PROJECT_NAME}_lib)
