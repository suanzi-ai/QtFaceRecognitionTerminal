TEMPLATE = app
TARGET = FaceScope
CONFIG  += C++11
QT += widgets

INCLUDEPATH += lib/hisi_sdk/include \
    lib/sz_sdk/include \
    lib \
    core \
    core/app \
    core/io \
    ui

LIBS += -L$$PWD/lib/hisi_sdk/lib \
    -ltde -lsvpruntime -lnnie \
    -lsns_os05a \
    -lsns_mn34220 -lsns_imx458 -lsns_imx335 \
    -lsns_imx327_2l -lsns_imx327 -lsns_imx307_2l -lsns_imx307 \
    -l_hiawb -l_hiawb_natura -l_hiae -live -lmd \
    -lhifisheyecalibrate -lhi_cipher -lhdmi -lisp \
    -l_hidehaze -l_hidrc -l_hildci -lmpi \
    -ldnvqe -lupvqe -lsecurec -lVoiceEngine \
    -L$$PWD/lib/spdlog \
    -lspdlog

HEADERS = \
    core/app/alive_task.h \
    core/app/detect_task.h \
    core/app/recognize_task.h \
    core/detection_float.h \
    core/image_package.h \
    core/io/hdmi.h \
    core/io/isp.h \
    core/io/ive.h \
    core/io/iveapp.h \
    core/io/mipi.h \
    core/io/mipi_tx.h \
    core/io/mipi_tx_driver.h \
    core/io/mmzimage.h \
    core/io/mpp.h \
    core/io/rp-lcd-mipi-8inch-800x1280-config.h \
    core/io/sml-lcd-mipi-8inch-800x1280-config.h \
    core/io/utility.h \
    core/io/vi.h \
    core/io/vi_vpss.h \
    core/io/vi_vpss_vo.h \
    core/io/vo.h \
    core/io/vpss.h \
    core/person.h \
    ui/detecttipwidget.h \
    ui/recognizetipwidget.h \
    ui/videoframewidget.h \
    ui/videoplayer.h \
    core/app/camera_reader.h \
    core/pingpangbuffer.h


SOURCES += \
    core/app/alive_task.cpp \
    core/app/detect_task.cpp \
    core/app/recognize_task.cpp \
    core/io/hdmi.cpp \
    core/io/isp.cpp \
    core/io/ive.cpp \
    core/io/iveapp.cpp \
    core/io/mipi.cpp \
    core/io/mipi_tx.cpp \
    core/io/mipi_tx_driver.cpp \
    core/io/mmzimage.cpp \
    core/io/mpp.cpp \
    core/io/rp-lcd-mipi-8inch-800x1280-config.c \
    core/io/sml-lcd-mipi-8inch-800x1280-config.c \
    core/io/utility.cpp \
    core/io/vi.cpp \
    core/io/vi_vpss.cpp \
    core/io/vi_vpss_vo.cpp \
    core/io/vo.cpp \
    core/io/vpss.cpp \
    main.cpp \
    ui/detecttipwidget.cpp \
    ui/recognizetipwidget.cpp \
    ui/videoframewidget.cpp \
    ui/videoplayer.cpp \
    core/app/camera_reader.cpp \
    core/pingpangbuffer.cpp
