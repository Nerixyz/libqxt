set(SQL_SOURCES
    qxtsql.h
    qxtsqlfile.cpp
    qxtsqlfile.h
    qxtsqlpackage.cpp
    qxtsqlpackage.h
    qxtsqlpackagemodel.cpp
    qxtsqlpackagemodel.h
    qxtsqlthreadmanager.cpp
    qxtsqlthreadmanager.h
    qxtsqltransaction.cpp
    qxtsqltransaction.h
)

add_library(libqxtsql ${QXT_LIB_TYPE} ${SQL_SOURCES})
target_link_libraries(libqxtsql
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Sql
    libqxtcore)
target_include_directories(libqxtsql PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libqxtsql PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/QxtSql)
set_target_properties(libqxtsql
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
)
target_compile_definitions(libqxtsql PRIVATE BUILD_QXT_SQL)

if (QXT_STATIC)
    target_compile_definitions(libqxtsql PRIVATE QXT_STATIC)
endif()
