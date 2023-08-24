set(ZEROCONF_SOURCES
    qxtdiscoverableservice_p.h
    qxtdiscoverableservice.cpp
    qxtdiscoverableservice.h
    qxtdiscoverableservicename.cpp
    qxtdiscoverableservicename.h
    qxtservicebrowser_p.h
    qxtservicebrowser.cpp
    qxtservicebrowser.h
    qxtzeroconf.h
)

option(QXT_ZEROCONF_USE_AVAHI "Use AVAHI over Bonjour" Off)

if(QXT_ZEROCONF_USE_AVAHI)
    list(APPEND ZEROCONF_SOURCES
        avahi/qxtavahipoll_p.h
        avahi/qxtavahipoll.cpp
        avahi/qxtavahipoll.h
        avahi/qxtmdns_avahi_p.h
        avahi/qxtmdns_avahi.cpp
        avahi/qxtmdns_avahi.h
    )
else()
    list(APPEND ZEROCONF_SOURCES
        bonjour/qxtmdns_bonjour.cpp
        bonjour/qxtmdns_bonjour.h
    )
endif()

add_library(libqxtzeroconf ${QXT_LIB_TYPE} ${ZEROCONF_SOURCES})
target_link_libraries(libqxtzeroconf
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Network
    libqxtcore
    libqxtnetwork
    ${DNSSD_LIBRARIES}
)
target_include_directories(libqxtzeroconf PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libqxtzeroconf PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/QxtZeroconf)
target_include_directories(libqxtzeroconf PRIVATE ${DNSSD_INCLUDE_DIRS})
set_target_properties(libqxtzeroconf
    PROPERTIES
        AUTOMOC ON
)
target_compile_definitions(libqxtzeroconf PRIVATE BUILD_QXT_ZEROCONF)

if (QXT_STATIC)
    target_compile_definitions(libqxtzeroconf PRIVATE QXT_STATIC)
endif()
