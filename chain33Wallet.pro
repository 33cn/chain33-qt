#-------------------------------------------------
#
# Project created by QtCreator 2017-11-22T14:31:11
#
#-------------------------------------------------

TEMPLATE = app
TARGET = chain33-qt
CONFIG(debug, debug|release) {
    TARGET = chain33-qt-test
}
VERSION = 1.0.1.0
INCLUDEPATH +=  /usr/local/include qt ./ ./qt/connector ./qt/seedUi ./qt/functionui ./qt/qrencode ./qt/menu ./qt/mainui
DEFINES += QT_GUI WIN32_LEAN_AND_MEAN
CONFIG += no_include_pwd
CONFIG += thread

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
}
QT += network
QT += xml

!greaterThan(QT_MAJOR_VERSION, 4) {
   QT += script
}

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

SOURCES += main.cpp \
    qt/functionui/qvalidatedlineedit.cpp \
    qt/functionui/notificator.cpp \
    qt/functionui/bitcoinunits.cpp \
    qt/functionui/guiutil.cpp \
    qt/functionui/bitcoinaddressvalidator.cpp \
    qt/menu/aboutdialog.cpp \
    qt/functionui/bitcoinamountfield.cpp \
    qt/functionui/transactionrecord.cpp \
    qt/menu/transactiondescdialog.cpp \
    qt/menu/editaddressdialog.cpp \
    qt/menu/askpassphrasedialog.cpp \
    qt/functionui/csvmodelwriter.cpp \
    qt/functionui/transactionfilterproxy.cpp \
    qt/mainui/mainui.cpp \
    qt/mainui/homepageui.cpp \
    qt/mainui/addressui.cpp \
    qt/mainui/addresslistui.cpp \
    qt/menu/veifyseeddialog.cpp \
    qt/mainui/transactionslistui.cpp \
    qt/mainui/walletsendui.cpp \
    qt/mainui/statusbarui.cpp \
    qt/connector/basejsonconnector.cpp \
    qt/functionui/friendsaddrlistxml.cpp \
    qt/mainui/transactionslistmodel.cpp \
    qt/mainui/addresstablemodel.cpp \
    qt/seedUi/seedui.cpp \
    qt/seedUi/createseedui.cpp \
    qt/seedUi/inputseedui.cpp \
    qt/seedUi/switchcreateseedui.cpp \
    qt/mainui/manageui.cpp \
    qt/menu/cliconsole.cpp \
    qt/connector/singleapplication.cpp \
    qt/mainui/txviewitemdelegate.cpp \
    qt/connector/basefuntion.cpp \
    qt/functionui/platformstyle.cpp \
    qt/menu/introdialog.cpp \
    qt/menu/offlineminingdialog.cpp \
    qt/menu/changedirdialog.cpp \
    qt/menu/closeingdialog.cpp \
    qt/connector/clearthread.cpp \
    qt/connector/cstyleconfig.cpp

greaterThan(QT_MAJOR_VERSION, 4) {
    SOURCES += qt/menu/receiverequestdialog.cpp \
    qt/qrencode/bitstream.c \
    qt/qrencode/mask.c \
    qt/qrencode/mmask.c \
    qt/qrencode/mqrspec.c \
    qt/qrencode/qrencode.c \
    qt/qrencode/qrinput.c \
    qt/qrencode/qrspec.c \
    qt/qrencode/rscode.c \
    qt/qrencode/split.c
}

HEADERS  += \
    qt/functionui/qvalidatedlineedit.h \
    qt/functionui/guiconstants.h \
    qt/functionui/notificator.h \
    qt/functionui/bitcoinunits.h \
    qt/functionui/guiutil.h \
    qt/functionui/bitcoinaddressvalidator.h \
    qt/menu/aboutdialog.h \
    qt/functionui/bitcoinamountfield.h \
    qt/functionui/transactionrecord.h \
    qt/menu/transactiondescdialog.h \
    qt/menu/editaddressdialog.h \
    qt/menu/askpassphrasedialog.h \
    qt/functionui/csvmodelwriter.h \
    qt/functionui/transactionfilterproxy.h \
    qt/functionui/uint256.h \
    qt/mainui/mainui.h \
    qt/mainui/homepageui.h \
    qt/mainui/addressui.h \
    qt/mainui/addresslistui.h \
    qt/menu/veifyseeddialog.h \
    qt/mainui/txviewitemdelegate.h \
    qt/mainui/transactionslistui.h \
    qt/mainui/walletsendui.h \
    qt/mainui/statusbarui.h \
    qt/connector/basejsonconnector.h \
    qt/connector/enumtype.h \
    qt/functionui/friendsaddrlistxml.h \
    qt/mainui/transactionslistmodel.h \
    qt/mainui/addresstablemodel.h \
    qt/functionui/base58.h \
    qt/functionui/mysha256.h \
    qt/seedUi/seedui.h \
    qt/seedUi/createseedui.h \
    qt/seedUi/inputseedui.h \
    qt/seedUi/switchcreateseedui.h \
    qt/mainui/manageui.h \
    qt/menu/cliconsole.h \
    qt/connector/singleapplication.h \
    qt/connector/basefuntion.h \
    qt/functionui/platformstyle.h \
    qt/menu/introdialog.h \
    qt/menu/offlineminingdialog.h \
    qt/menu/changedirdialog.h \
    qt/menu/closeingdialog.h \
    qt/connector/clearthread.h \
    qt/connector/cstyleconfig.h

greaterThan(QT_MAJOR_VERSION, 4) {
    HEADERS += qt/menu/receiverequestdialog.h \
    qt/qrencode/bitstream.h \
    qt/qrencode/config.h \
    qt/qrencode/mask.h \
    qt/qrencode/mmask.h \
    qt/qrencode/mqrspec.h \
    qt/qrencode/qrencode_inner.h \
    qt/qrencode/qrencode.h \
    qt/qrencode/qrinput.h \
    qt/qrencode/qrspec.h \
    qt/qrencode/rscode.h \
    qt/qrencode/split.h
}

FORMS    += \
    qt/forms/aboutdialog.ui \
    qt/forms/askpassphrasedialog.ui \
    qt/forms/editaddressdialog.ui \
    qt/forms/mainui.ui \
    qt/forms/homepageui.ui \
    qt/forms/addressui.ui \
    qt/forms/addresslistui.ui \
    qt/forms/transactionslistui.ui \
    qt/forms/walletsendui.ui \
    qt/forms/statusbarui.ui \
    qt/forms/transactiondescdialog.ui \
    qt/forms/seedui.ui \
    qt/forms/createseedui.ui \
    qt/forms/inputseedui.ui \
    qt/forms/switchcreateseedui.ui \
    qt/forms/manageui.ui \
    qt/forms/cliconsole.ui \
    qt/forms/introdialog.ui \
    qt/forms/offlineminingdialog.ui \
    qt/forms/changedirdialog.ui \
    qt/forms/closeingdialog.ui \
    qt/menu/veifyseeddialog.ui

greaterThan(QT_MAJOR_VERSION, 4) {
    FORMS    += qt/forms/receiverequestdialog.ui
}

RESOURCES += qt/bitcoin.qrc

#When define #QT_DEBUG do not use "change data dir" of the menu, otherwise data may be destroyed, caused by not close chain33 in void ManageUI::CloseChain33(){}

CODECFORTR = UTF-8

# for lrelease/lupdate
# also add new translations to qt/bitcoin.qrc under translations/
TRANSLATIONS = $$files(qt/locale/bitcoin_*.ts)

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
isEmpty(QM_DIR):QM_DIR = $$PWD/qt/locale
# automatically build translations, so they can be included in resource file
TSQM.name = lrelease ${QMAKE_FILE_IN}
TSQM.input = TRANSLATIONS
TSQM.output = $$QM_DIR/${QMAKE_FILE_BASE}.qm
TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
TSQM.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += TSQM

windows:DEFINES += WIN32

CONFIG(release, debug|release) {
    windows:RC_FILE = qt/res/bitcoin-qt.rc
}

windows:!contains(MINGW_THREAD_BUGFIX, 0) {
    # At least qmake's win32-g++-cross profile is missing the -lmingwthrd
    # thread-safety flag. GCC has -mthreads to enable this, but it doesn't
    # work with static linking. -lmingwthrd must come BEFORE -lmingw, so
    # it is prepended to QMAKE_LIBS_QT_ENTRY.
    # It can be turned off with MINGW_THREAD_BUGFIX=0, just in case it causes
    # any problems on some untested qmake profile now or in the future.
    DEFINES += _MT BOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN
    QMAKE_LIBS_QT_ENTRY = -lmingwthrd $$QMAKE_LIBS_QT_ENTRY
}

macx:HEADERS += qt/functionui/macdockiconhandler.h
macx:HEADERS += qt/functionui/macnotificationhandler.h
macx:OBJECTIVE_SOURCES += qt/functionui/macdockiconhandler.mm
macx:OBJECTIVE_SOURCES += qt/functionui/macnotificationhandler.mm
macx:LIBS += -framework Foundation -framework ApplicationServices -framework AppKit
macx:DEFINES += MAC_OSX MSG_NOSIGNAL=0
macx:ICON = qt/res/icons/bitcoin.icns
macx:TARGET = "chain33-qt"
macx:QMAKE_CFLAGS_THREAD += -pthread
macx:QMAKE_LFLAGS_THREAD += -pthread
macx:QMAKE_CXXFLAGS_THREAD += -pthread

LIBS += -L/usr/local/lib -L/opt/local/lib
windows:LIBS += -lws2_32 -lshlwapi -lmswsock -lole32 -loleaut32 -luuid -lgdi32
greaterThan(QT_MAJOR_VERSION, 4) {
    windows:LIBS += -lDbgHelp
}
contains(RELEASE, 1) {
    !windows:!macx {
        # Linux: turn dynamic linking back on for c/c++ runtime libraries
        LIBS += -Wl,-Bdynamic
    }
}

!windows:!macx {
    DEFINES += LINUX
    LIBS += -lrt -ldl
}

system($$QMAKE_LRELEASE -silent $$_PRO_FILE_)
