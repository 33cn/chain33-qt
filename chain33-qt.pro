#-------------------------------------------------
#
# Project created by QtCreator 2017-11-22T14:31:11
#
#-------------------------------------------------

TEMPLATE = app
TARGET = chain33-qt
CONFIG(debug, debug|release) {
    TARGET = chain33-qt-Debug
}
VERSION = 1.0.0
INCLUDEPATH +=  /usr/local/include src ./ ./src/connector ./src/seedUi ./src/functionui ./src/qrencode ./src/menu ./src/mainui
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
    src/functionui/qvalidatedlineedit.cpp \
    src/functionui/notificator.cpp \
    src/functionui/bitcoinunits.cpp \
    src/functionui/guiutil.cpp \
    src/functionui/bitcoinaddressvalidator.cpp \
    src/menu/aboutdialog.cpp \
    src/functionui/bitcoinamountfield.cpp \
    src/functionui/transactionrecord.cpp \
    src/menu/transactiondescdialog.cpp \
    src/menu/editaddressdialog.cpp \
    src/menu/askpassphrasedialog.cpp \
    src/functionui/csvmodelwriter.cpp \
    src/functionui/transactionfilterproxy.cpp \
    src/mainui/mainui.cpp \
    src/mainui/homepageui.cpp \
    src/mainui/addressui.cpp \
    src/mainui/addresslistui.cpp \
    src/menu/veifyseeddialog.cpp \
    src/mainui/transactionslistui.cpp \
    src/mainui/walletsendui.cpp \
    src/mainui/statusbarui.cpp \
    src/connector/basejsonconnector.cpp \
    src/functionui/friendsaddrlistxml.cpp \
    src/mainui/transactionslistmodel.cpp \
    src/mainui/addresstablemodel.cpp \
    src/seedUi/seedui.cpp \
    src/seedUi/createseedui.cpp \
    src/seedUi/inputseedui.cpp \
    src/seedUi/switchcreateseedui.cpp \
    src/mainui/manageui.cpp \
    src/menu/cliconsole.cpp \
    src/connector/singleapplication.cpp \
    src/mainui/txviewitemdelegate.cpp \
    src/connector/basefuntion.cpp \
    src/functionui/platformstyle.cpp \
    src/menu/introdialog.cpp \
    src/menu/offlineminingdialog.cpp \
    src/menu/changedirdialog.cpp \
    src/menu/closeingdialog.cpp \
    src/connector/clearthread.cpp \
    src/connector/cstyleconfig.cpp

greaterThan(QT_MAJOR_VERSION, 4) {
    SOURCES += src/menu/receiverequestdialog.cpp \
    src/qrencode/bitstream.c \
    src/qrencode/mask.c \
    src/qrencode/mmask.c \
    src/qrencode/mqrspec.c \
    src/qrencode/qrencode.c \
    src/qrencode/qrinput.c \
    src/qrencode/qrspec.c \
    src/qrencode/rscode.c \
    src/qrencode/split.c
}

HEADERS  += \
    src/functionui/qvalidatedlineedit.h \
    src/functionui/guiconstants.h \
    src/functionui/notificator.h \
    src/functionui/bitcoinunits.h \
    src/functionui/guiutil.h \
    src/functionui/bitcoinaddressvalidator.h \
    src/menu/aboutdialog.h \
    src/functionui/bitcoinamountfield.h \
    src/functionui/transactionrecord.h \
    src/menu/transactiondescdialog.h \
    src/menu/editaddressdialog.h \
    src/menu/askpassphrasedialog.h \
    src/functionui/csvmodelwriter.h \
    src/functionui/transactionfilterproxy.h \
    src/functionui/uint256.h \
    src/mainui/mainui.h \
    src/mainui/homepageui.h \
    src/mainui/addressui.h \
    src/mainui/addresslistui.h \
    src/menu/veifyseeddialog.h \
    src/mainui/txviewitemdelegate.h \
    src/mainui/transactionslistui.h \
    src/mainui/walletsendui.h \
    src/mainui/statusbarui.h \
    src/connector/basejsonconnector.h \
    src/connector/enumtype.h \
    src/functionui/friendsaddrlistxml.h \
    src/mainui/transactionslistmodel.h \
    src/mainui/addresstablemodel.h \
    src/functionui/base58.h \
    src/functionui/mysha256.h \
    src/seedUi/seedui.h \
    src/seedUi/createseedui.h \
    src/seedUi/inputseedui.h \
    src/seedUi/switchcreateseedui.h \
    src/mainui/manageui.h \
    src/menu/cliconsole.h \
    src/connector/singleapplication.h \
    src/connector/basefuntion.h \
    src/functionui/platformstyle.h \
    src/menu/introdialog.h \
    src/menu/offlineminingdialog.h \
    src/menu/changedirdialog.h \
    src/menu/closeingdialog.h \
    src/connector/clearthread.h \
    src/connector/cstyleconfig.h

greaterThan(QT_MAJOR_VERSION, 4) {
    HEADERS += src/menu/receiverequestdialog.h \
    src/qrencode/bitstream.h \
    src/qrencode/config.h \
    src/qrencode/mask.h \
    src/qrencode/mmask.h \
    src/qrencode/mqrspec.h \
    src/qrencode/qrencode_inner.h \
    src/qrencode/qrencode.h \
    src/qrencode/qrinput.h \
    src/qrencode/qrspec.h \
    src/qrencode/rscode.h \
    src/qrencode/split.h
}

FORMS    += \
    src/forms/aboutdialog.ui \
    src/forms/askpassphrasedialog.ui \
    src/forms/editaddressdialog.ui \
    src/forms/mainui.ui \
    src/forms/homepageui.ui \
    src/forms/addressui.ui \
    src/forms/addresslistui.ui \
    src/forms/transactionslistui.ui \
    src/forms/walletsendui.ui \
    src/forms/statusbarui.ui \
    src/forms/transactiondescdialog.ui \
    src/forms/seedui.ui \
    src/forms/createseedui.ui \
    src/forms/inputseedui.ui \
    src/forms/switchcreateseedui.ui \
    src/forms/manageui.ui \
    src/forms/cliconsole.ui \
    src/forms/introdialog.ui \
    src/forms/offlineminingdialog.ui \
    src/forms/changedirdialog.ui \
    src/forms/closeingdialog.ui \
    src/menu/veifyseeddialog.ui

greaterThan(QT_MAJOR_VERSION, 4) {
    FORMS    += src/forms/receiverequestdialog.ui
}

RESOURCES += src/bitcoin.qrc

#When define #QT_DEBUG do not use "change data dir" of the menu, otherwise data may be destroyed, caused by not close chain33 in void ManageUI::CloseChain33(){}

CODECFORTR = UTF-8

# for lrelease/lupdate
# also add new translations to src/bitcoin.qrc under translations/
TRANSLATIONS = $$files(src/locale/bitcoin_*.ts)

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
isEmpty(QM_DIR):QM_DIR = $$PWD/src/locale
# automatically build translations, so they can be included in resource file
TSQM.name = lrelease ${QMAKE_FILE_IN}
TSQM.input = TRANSLATIONS
TSQM.output = $$QM_DIR/${QMAKE_FILE_BASE}.qm
TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
TSQM.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += TSQM

windows:DEFINES += WIN32

CONFIG(release, debug|release) {
    windows:RC_FILE = src/res/bitcoin-qt.rc
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

macx:HEADERS += src/functionui/macdockiconhandler.h
macx:HEADERS += src/functionui/macnotificationhandler.h
macx:OBJECTIVE_SOURCES += src/functionui/macdockiconhandler.mm
macx:OBJECTIVE_SOURCES += src/functionui/macnotificationhandler.mm
macx:LIBS += -framework Foundation -framework ApplicationServices -framework AppKit
macx:DEFINES += MAC_OSX MSG_NOSIGNAL=0
macx:ICON = src/res/icons/bitcoin.icns
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
