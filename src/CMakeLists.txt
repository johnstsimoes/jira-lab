find_package(fmt CONFIG REQUIRED)

#----- Include LibStein
include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
    LibStein
    GIT_REPOSITORY "git@github.com:johnstsimoes/libstein.git"
    GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(LibStein)
include_directories(${LibStein_SOURCE_DIR}/include)

add_library(${PROJECT_NAME}_lib "")

target_sources(${PROJECT_NAME}_lib PUBLIC
    jira/jira.cpp)

add_executable(${PROJECT_NAME}
    main.cpp)

target_link_libraries(${PROJECT_NAME}
    LINK_PUBLIC
    fmt::fmt
    LibStein
    lua
    ${PROJECT_NAME}_lib)
