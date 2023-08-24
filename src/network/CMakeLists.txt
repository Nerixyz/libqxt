set(NETWORK_SOURCES
    qxtjsonrpccall.cpp
    qxtjsonrpccall.h
    qxtjsonrpcclient.cpp
    qxtjsonrpcclient.h
    qxtmail_p.h
    qxtmailattachment.cpp
    qxtmailattachment.h
    qxtmailmessage.cpp
    qxtmailmessage.h
    qxtnetwork.h
    qxtpop3_p.h
    qxtpop3.cpp
    qxtpop3.h
    qxtpop3listreply.h
    qxtpop3reply_p.h
    qxtpop3reply.cpp
    qxtpop3reply.h
    qxtpop3retrreply.h
    qxtpop3statreply.h
    qxtrpcpeer.cpp
    qxtrpcpeer.h
    qxtsmtp_p.h
    qxtsmtp.cpp
    qxtsmtp.h
    qxtsslconnectionmanager.cpp
    qxtsslconnectionmanager.h
    qxtsslserver.cpp
    qxtsslserver.h
    qxttcpconnectionmanager_p.h
    qxttcpconnectionmanager.cpp
    qxttcpconnectionmanager.h
    qxtxmlrpc_p.cpp
    qxtxmlrpc_p.h
    qxtxmlrpccall.cpp
    qxtxmlrpccall.h
    qxtxmlrpcclient.cpp
    qxtxmlrpcclient.h
)

find_package(libssh2)

if (libssh2_FOUND)
    list(APPEND NETWORK_SOURCES
        qxtsshchannel_p.h
        qxtsshchannel.cpp
        qxtsshchannel.h
        qxtsshclient_p.h
        qxtsshclient.cpp
        qxtsshclient.h
        qxtsshprocess.cpp
        qxtsshprocess.h
        qxtsshtcpsocket.cpp
        qxtsshtcpsocket.h
    )
endif()

add_library(libqxtnetwork ${QXT_LIB_TYPE} ${NETWORK_SOURCES})
target_link_libraries(libqxtnetwork
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Network
    libqxtcore)
target_include_directories(libqxtnetwork PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libqxtnetwork PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/QxtNetwork)
set_target_properties(libqxtnetwork
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
)
target_compile_definitions(libqxtnetwork PRIVATE BUILD_QXT_NETWORK)

if (libssh2_FOUND)
    if(NOT TARGET libssh2::libssh2 AND TARGET Libssh2::libssh2)
        add_library(libssh2::libssh2 ALIAS Libssh2::libssh2)
    endif()

    target_link_libraries(libqxtnetwork
        libssh2::libssh2
    )
else()
    target_compile_definitions(libqxtnetwork PRIVATE NO_LIBSSH)
endif()

if (QXT_STATIC)
    target_compile_definitions(libqxtnetwork PRIVATE QXT_STATIC)
endif()
