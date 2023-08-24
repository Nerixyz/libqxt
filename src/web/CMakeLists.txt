set(WEB_SOURCES
    qhttpheader.cpp
    qhttpheader.h
    qxtabstracthttpconnector.cpp
    qxtabstracthttpconnector.h
    qxtabstractwebservice.cpp
    qxtabstractwebservice.h
    qxtabstractwebsessionmanager_p.h
    qxtabstractwebsessionmanager.cpp
    qxtabstractwebsessionmanager.h
    qxthtmltemplate.cpp
    qxthtmltemplate.h
    qxthttpserverconnector.cpp
    qxthttpsessionmanager.cpp
    qxthttpsessionmanager.h
    qxtscgiserverconnector.cpp
    qxtweb.h
    qxtwebcgiservice_p.h
    qxtwebcgiservice.cpp
    qxtwebcgiservice.h
    qxtwebcontent.cpp
    qxtwebcontent.h
    qxtwebevent.cpp
    qxtwebevent.h
    qxtwebjsonrpcservice_p.h
    qxtwebjsonrpcservice.cpp
    qxtwebjsonrpcservice.h
    qxtwebservicedirectory_p.h
    qxtwebservicedirectory.cpp
    qxtwebservicedirectory.h
    qxtwebslotservice.cpp
    qxtwebslotservice.h
)

set_property(SOURCE qxtwebjsonrpcservice.cpp PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE qxtwebslotservice.cpp PROPERTY SKIP_AUTOMOC ON)

add_library(libqxtweb ${QXT_LIB_TYPE} ${WEB_SOURCES})
target_link_libraries(libqxtweb
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Network
    libqxtcore
    libqxtnetwork
)
target_include_directories(libqxtweb PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libqxtweb PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/QxtWeb)
set_target_properties(libqxtweb
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
)
target_compile_definitions(libqxtweb PRIVATE BUILD_QXT_WEB)

if (Qt5WebSockets_FOUND)
    target_compile_definitions(libqxtweb PRIVATE QXT_HAVE_WEBSOCKETS)
    target_link_libraries(libqxtweb Qt${QT_MAJOR_VERSION}::WebSockets)
else()
    target_compile_definitions(libqxtweb PRIVATE QXT_NO_WEBSOCKETS)
endif()

if (QXT_STATIC)
    target_compile_definitions(libqxtweb PRIVATE QXT_STATIC)
endif()
