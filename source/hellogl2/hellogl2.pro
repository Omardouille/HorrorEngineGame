HEADERS       = glwidget.h \
    collision.h \
                window.h \
                mainwindow.h \
    scene.h \
    transform.h \
    basicio.h \
    pool.h \
    OBJ_Loader.h
SOURCES       = glwidget.cpp \
    collision.cpp \
                main.cpp \
                window.cpp \
                mainwindow.cpp \
    scene.cpp \
    transform.cpp \
    pool.cpp

QT           += widgets

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/hellogl2
INSTALLS += target
CONFIG += c++14
