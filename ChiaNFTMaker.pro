QT       += core gui
QT       += charts
QT       += network
QT += concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    composethread.cpp \
    cryption.cpp \
    dialogabout.cpp \
    dialogcharts.cpp \
    dialoghelp.cpp \
    dialogordervie.cpp \
    dialogregister.cpp \
    dialogsort.cpp \
    formadd.cpp \
    formmint.cpp \
    formoffer.cpp \
    iniadd.cpp \
    listwalletidthread.cpp \
    main.cpp \
    mainwindow.cpp \
    mintthread.cpp \
    offerthread.cpp

HEADERS += \
    ChiaNFTMaker.h \
    composethread.h \
    cryption.h \
    dialogabout.h \
    dialogcharts.h \
    dialoghelp.h \
    dialogordervie.h \
    dialogregister.h \
    dialogsort.h \
    formadd.h \
    formmint.h \
    formoffer.h \
    iniadd.h \
    listwalletidthread.h \
    mainwindow.h \
    mintthread.h \
    offerthread.h

FORMS += \
    dialogabout.ui \
    dialogcharts.ui \
    dialoghelp.ui \
    dialogordervie.ui \
    dialogregister.ui \
    dialogsort.ui \
    formadd.ui \
    formmint.ui \
    formoffer.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#应用图标
RC_ICONS += ChiaMintNFT1.ico

#版本号
VERSION = 3.0.0
#中文
RC_LANG = 0x0004
# 公司名
QMAKE_TARGET_COMPANY = China Digital Party
# 产品名称
QMAKE_TARGET_PRODUCT = ChiaNFTMaker
# 详细描述
QMAKE_TARGET_DESCRIPTION = 批量合成NFT图片、批量生成元数据、批量铸造、批量创建offer
# 版权
QMAKE_TARGET_COPYRIGHT = Copyright(C) 2022 the China Digital Party

RESOURCES += \
    resource.qrc

TRANSLATIONS +=cn.ts \
                en.ts
