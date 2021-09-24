QMAKE_EXTRA_TARGETS += libqxt libqxt_git libqxt_conf libqxt_update libqxt_build libqxt_distclean distclean

libqxt_git.target = libqxt/configure
libqxt_git.commands = git clone https://bitbucket.org/libqxt/libqxt

libqxt_conf.target = libqxt/.qmake.cache
libqxt_conf.depends = libqxt_git libqxt_git.pri libqxt/configure libqxt/features/qxtvars.prf $$_PRO_FILE_
libqxt_conf.commands = cd libqxt && ./configure -nomake docs -static -no-pkgconfig

libqxt_update.depends = FORCE
libqxt_update.commands = -cd libqxt && git pull

libqxt.depends = FORCE libqxt_update libqxt_conf
libqxt.commands = $(MAKE) -C libqxt all && touch libqxt/.build

libqxt_build.target = libqxt/.build
libqxt_build.depends = libqxt_conf libqxt/Makefile
libqxt_build.CONFIG = recursive
libqxt_build.commands = $(MAKE) -C libqxt all && touch libqxt/.build

libqxt_distclean.commands = git diff-index --quiet HEAD -- && $(DEL_FILE) -r libqxt
distclean.depends = libqxt_distclean

INCLUDEPATH += libqxt/include libqxt/src

CONFIG(debug,debug|release) {
  libqxt_conf.commands += -debug
}

contains(QXT,sql) {
  QXT += core
  INCLUDEPATH += libqxt/include/QxtSql libqxt/src/sql
  LIBS += libqxt/lib/libQxtSql.a
  PRE_TARGETDEPS += libqxt/lib/libQxtSql.a
  QMAKE_EXTRA_TARGETS += libqxtsql
  libqxtsql.target = libqxt/lib/libQxtSql.a
  libqxtsql.depends = libqxtcore
} else {
  libqxt_conf.commands += -nomake sql
}

contains(QXT,widgets) {
  QXT += core
  INCLUDEPATH += libqxt/include/QxtWidgets libqxt/src/widgets
  LIBS += libqxt/lib/libQxtWidgets.a
  PRE_TARGETDEPS += libqxt/lib/libQxtWidgets.a
  QMAKE_EXTRA_TARGETS += libqxtwidgets
  !packagesExist(xrandr) {
    libqxt_conf.commands += -no-xrandr
  }
  libqxtwidgets.target = libqxt/lib/libQxtWidgets.a
  libqxtwidgets.depends = libqxtcore
} else {
  libqxt_conf.commands += -nomake widgets -no-xrandr
}

contains(QXT,berkeley) {
  QXT += core
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
  !contains(QXT,websockets) {
    libqxt_conf.commands += -no-websockets
  }
} else {
  libqxt_conf.commands += -nomake web -no-websockets
}

contains(QXT,network) {
  !contains(QXT,openssl) {
    libqxt_conf.commands += -no-openssl
    QXT -= libssh
  }

  contains(QXT,libssh) {
    !win32: LIBS += -lcrypto -lz
  } else {
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
  libqxtcore.depends = libqxt_build
}

include(libqxt/features/qxtvars.prf)
