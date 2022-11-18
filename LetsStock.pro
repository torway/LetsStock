QT  += core gui sql printsupport charts webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 sdk_no_version_check

ICON = logo.icns
RC_ICONS = logo.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    RangeSlider/RangeSlider.cpp \
    add_article.cpp \
    add_categorie.cpp \
    add_client.cpp \
    add_excel.cpp \
    add_order.cpp \
    add_sell.cpp \
    add_vendor.cpp \
    letsstock.cpp \
    main.cpp

HEADERS += \
    RangeSlider/RangeSlider.h \
    add_article.h \
    add_categorie.h \
    add_client.h \
    add_excel.h \
    add_order.h \
    add_sell.h \
    add_vendor.h \
    letsstock.h

FORMS += \
    add_article.ui \
    add_categorie.ui \
    add_client.ui \
    add_excel.ui \
    add_order.ui \
    add_sell.ui \
    add_vendor.ui \
    letsstock.ui

RESOURCES += \
    ressources.qrc

QXLSX_PARENTPATH=./QXlsx/         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./QXlsx/source/  # current QXlsx source path is ./source/
include(./QXlsx/QXlsx.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
