#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QDesktopWidget>

#include "GLWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    GLWidget window;
    int window_width = 800;
    int window_height = 600;
        
    window.resize(window_width,window_height);
    QRect screen = app.desktop()->screenGeometry();
    window.move(screen.width()/2 - window_width/2,
                screen.height()/2 - window_height/2);
    window.show();
    
    return app.exec();

    return 0;
}
