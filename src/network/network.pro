CLEAN_TARGET     = QxtNetwork
DEFINES         += BUILD_QXT_NETWORK
QT               = core network
QXT              = core
CONVENIENCE     += $$CLEAN_TARGET

include(network.pri)
include(../qxtbase.pri)

contains(DEFINES,QXT_HAVE_OPENSSL):!contains(DEFINES,NO_LIBSSH) {
 include(../3rdparty/libssh2/libssh2.pri)
}

unix|win32-g++*:QMAKE_PKGCONFIG_REQUIRES = QxtCore QtNetwork
