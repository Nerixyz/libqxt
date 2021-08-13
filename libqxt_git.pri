QMAKE_EXTRA_TARGETS += libqxt libqxt_git libqxt_conf libqxt_distclean distclean

libqxt_git.target = libqxt/configure
libqxt_git.commands = git clone https://bitbucket.org/libqxt/libqxt

libqxt_conf.target = libqxt/Makefile
libqxt_conf.depends = libqxt_git libqxt_git.pri $$_PRO_FILE_
libqxt_conf.commands = cd libqxt && ./configure -nomake docs -static

libqxt.commands = $(MAKE) -C libqxt all
libqxt.depends = libqxt_conf
libqxt.CONFIG = recursive
libqxt.commands = $(MAKE) -C libqxt all && rm -f libqxt/lib/*.prl

libqxt_distclean.commands = -$(DEL_FILE) -r libqxt
distclean.depends = libqxt_distclean

INCLUDEPATH += libqxt/include libqxt/src

CONFIG(debug,debug|release) {
  libqxt_conf.commands += -debug
}

contains(QXT,sql) {
  INCLUDEPATH += libqxt/include/QxtSql libqxt/src/sql
  LIBS += libqxt/lib/libQxtSql.a
  PRE_TARGETDEPS += libqxt/lib/libQxtSql.a
  libqxtsql.target = libqxt/lib/libQxtSql.a
  libqxtsql.depends = libqxtcore
} else {
  libqxt_conf.commands += -nomake sql
}

contains(QXT,widgets) {
  INCLUDEPATH += libqxt/include/QxtWidgets libqxt/src/widgets
  LIBS += libqxt/lib/libQxtWidgets.a
  PRE_TARGETDEPS += libqxt/lib/libQxtWidgets.a
  !packagesExist(xrandr) {
    libqxt_conf.commands += -no-xrandr
  }
  libqxtwidgets.target = libqxt/lib/libQxtWidgets.a
  libqxtwidgets.depends = libqxtcore
} else {
  libqxt_conf.commands += -nomake widgets -no-xrandr
}

contains(QXT,berkeley) {
  INCLUDEPATH += libqxt/include/QxtBerkeley libqxt/src/bdb
  LIBS += libqxt/lib/libQxtBerkeley.a -ldb
  PRE_TARGETDEPS += libqxt/lib/libQxtBerkeley.a
  QMAKE_EXTRA_TARGETS += libqxtberkeley
  libqxtberkeley.target = libqxt/lib/libQxtBerkeley.a
  libqxtberkeley.depends = libqxtcore
} else {
  libqxt_conf.commands += -no-db
}

contains(QXT,zeroconf) {
  QXT += network
  INCLUDEPATH += libqxt/include/QxtZeroconf libqxt/src/zeroconf
  LIBS += libqxt/lib/libQxtZeroconf.a
  PRE_TARGETDEPS += libqxt/lib/libQxtZeroconf.a
  QMAKE_EXTRA_TARGETS += libqxtzeroconf
  libqxtzeroconf.target = libqxt/lib/libQxtZeroconf.a
  libqxtzeroconf.depends = libqxtnetwork
} else {
  libqxt_conf.commands += -no-zeroconf
}

contains(QXT,web) {
  QXT += network
  INCLUDEPATH += libqxt/include/QxtWeb libqxt/src/web
  LIBS += libqxt/lib/libQxtWeb.a
  PRE_TARGETDEPS += libqxt/lib/libQxtWeb.a
  QMAKE_EXTRA_TARGETS += libqxtweb
  libqxtweb.target = libqxt/lib/libQxtWeb.a
  libqxtweb.depends = libqxtnetwork
} else {
  libqxt_conf.commands += -nomake web
}

contains(QXT,network) {
  !contains(QXT,openssl) {
    DEFINES -= HAVE_OPENSSL
    libqxt_conf.commands += -no-openssl
    QXT -= libssh
  }

  contains(QXT,libssh) {
    !win32: LIBS += -lcrypto -lz
  } else {
    DEFINES += NO_LIBSSH
    libqxt_conf.commands += -no-libssh
  }

  QXT += core
  INCLUDEPATH += libqxt/include/QxtNetwork libqxt/src/network
  LIBS += libqxt/lib/libQxtNetwork.a
  PRE_TARGETDEPS += libqxt/lib/libQxtNetwork.a
  QMAKE_EXTRA_TARGETS += libqxtnetwork
  libqxtnetwork.target = libqxt/lib/libQxtNetwork.a
  libqxtnetwork.depends = libqxtcore
} else {
  libqxt_conf.commands += -nomake network -no-openssl -no-libssh
}

contains(QXT,core) {
  INCLUDEPATH += libqxt/include/QxtCore libqxt/src/core
  LIBS += libqxt/lib/libQxtCore.a
  PRE_TARGETDEPS += libqxt/lib/libQxtCore.a
  QMAKE_EXTRA_TARGETS += libqxtcore
  libqxtcore.target = libqxt/lib/libQxtCore.a
  libqxtcore.depends = libqxt
}
