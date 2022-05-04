add_library(${PROJECT_NAME}_lib "")

target_sources(${PROJECT_NAME}_lib PUBLIC
    util.cpp
    settings.cpp
    util/lua_util.cpp
    base/lua_base.cpp
    jira/lua_jira.cpp
    jira/jira_changes.cpp
    jira/jira_components.cpp
    jira/jira_metadata.cpp
    jira/jira_jql.cpp
    jira/jira_ticket.cpp
    json_to_lua.cpp)

add_executable(${PROJECT_NAME}
    main.cpp)

target_link_libraries(${PROJECT_NAME}
    LINK_PUBLIC
    fmt::fmt
    curl
    libstein
    readline
    hiredis
    lua
    ${PROJECT_NAME}_lib)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
