#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL\QGLWidget>



void clear_all();

void paint_sides();

void collect_diagonals();

void paint_diagonals();

void paint_division();

void write_text();


class GLWidget : public QGLWidget {

    Q_OBJECT

public:
    GLWidget(QWidget *parent = NULL);


    void initializeGL();
    void resizeGL(int w, int h);
    
    void paintGL();
    
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

};

#endif
