CLEAN_TARGET     = QxtWeb
DEFINES         += BUILD_QXT_WEB
QT               = core network
QXT              = core network
CONVENIENCE     += $$CLEAN_TARGET

contains(DEFINES,QXT_HAVE_WEBSOCKETS) {
  QT += websockets
}

include(web.pri)
include(../qxtbase.pri)

unix|win32-g++*:QMAKE_PKGCONFIG_REQUIRES = QxtCore QxtNetwork
