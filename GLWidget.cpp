#include <QMouseEvent>
#include <QFont>
#include <QTextStream>

#include "GLWidget.h"
#include "subsidiary.h"
#include "Polygon.h"

#define LOAD_FILE "tes.txt"

bool need_sides = false;
bool need_diagonals = false;
bool need_division = false;
bool is_divided = false;
bool is_already_loaded = false;

extern int num_points;
int64_t max_width = 50;
int window_width = 0;
int window_height = 0;
int world_width = 50;
int world_height = 0;
Geometry::Point worldPosition;
Geometry::Point mousePosition;
Geometry::my_Polygon polygon;

std::list<Geometry::Diag*> diagonals;  //получаем из Polygon.cpp две противоположные диагонали со всей структурой.
std::list<Geometry::Diag*> diags;       //сюда помещаем все диагонали при обходе

int max_rank = 0;

Geometry::Diag *one_or_two;
std::list<Geometry::Point> res;

QString clear;
QString message;
QString enter;
QString load;
QString area1;
QString area2;
QString save;
QString movement;
QString zoom;
QString sExit;
QString zro;
QString one;
QString progress;

void clear_all()
{
    polygon.clear();
    diagonals.clear();
    diags.clear();
    res.clear();
    one_or_two = NULL;

    need_sides = false;
    need_diagonals = false;
    need_division = false;

    max_rank = 0;
    max_width = 50;
    world_width = 50;
    worldPosition.upload(0, 0);

    is_already_loaded = false;
    is_divided = false;
}

GLWidget::GLWidget(QWidget *parent): QGLWidget(parent)
{
    setMouseTracking(true);
    
    worldPosition.upload(-world_width/2, -world_height/2);
    mousePosition.upload(0, 0);
    
    QTextStream(&clear) << "C: clear polygon data";
    QTextStream(&enter) << "D: make division";
    QTextStream(&save) << "S: save polygon to result.txt";
    QTextStream(&movement) << "Arrows: movement";
    QTextStream(&zoom) << "+\\-: zoom";
    QTextStream(&sExit) << "Esc: exit";
    QTextStream(&load) << "L: load data from test.txt";
    QTextStream(&area1) << "A: print areas after division";
    QTextStream(&zro) << "0: draw only contour";
    QTextStream(&one) << "1: draw diagonals and division";
    //QTextStream(&progress) << "";
    //QTextStream(&area2) << "A: print areas after division";
}

void GLWidget::initializeGL() 
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
}

void GLWidget::resizeGL(int w, int h) 
{
    window_width = w;
    window_height = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    world_height = ((double)h / w) * world_width;

    glOrtho(worldPosition.x_num(), worldPosition.x_num() + world_width,
            worldPosition.y_num(), worldPosition.y_num() + world_height,
            -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}
void paint_sides()
{
    glColor4f(0,1,1,1);
    glBegin(GL_LINE_LOOP);  //рисуем контур
    std::list<Geometry::Point> tmp = polygon._points();
    std::list<Geometry::Point>::iterator r = tmp.begin();
    while(r != tmp.end())
    {
        double x = r -> x_num();
        double y = r -> y_num();
        glVertex2d(x, y);
        ++r;
    }
    glEnd();
}

void collect_diagonals()
{
    diags.push_back(one_or_two);

    if(one_or_two -> _rank() > max_rank)
        max_rank = one_or_two -> _rank();

    Geometry::Diag *tmp = one_or_two;
    if(one_or_two -> _next().empty() == false)
    {
        one_or_two = (one_or_two -> _next().front());
        collect_diagonals();
        one_or_two = tmp;
    }
    if(one_or_two -> _side_r().empty() == false)
    {
        one_or_two = (one_or_two -> _side_r().front());
        collect_diagonals();
        one_or_two = tmp;
    }
    if(one_or_two -> _side_l().empty() == false)
    {
        one_or_two = (one_or_two -> _side_l().front());
        collect_diagonals();
        one_or_two = tmp;
    }
}
void paint_diagonals()
{
    glBegin(GL_LINES);

    double color_tmp = 0;
    for (std::list<Geometry::Diag*>::iterator first = diags.begin(); first != --(diags.end()); first++)
    {
        color_tmp = (double) (1 + (*first) -> _rank()) / (1 + max_rank);
        glColor4f(0, color_tmp, 1 - color_tmp, 1);
        glVertex2d((*first) -> getS().x_num(), (*first) -> getS().y_num());
        glVertex2d((*first) -> getE().x_num(), (*first) -> getE().y_num());
    }
    glEnd();
}
void paint_division()
{
    glColor4f(1,0,0,1);
    glBegin(GL_LINES);

    std::list<Geometry::Point>::iterator second;
    for (std::list<Geometry::Point>::iterator first = res.begin(); first != --(res.end()); first++)
    {
        second = first;
        ++second;
        glVertex2d(first -> x_num(), first -> y_num());
        glVertex2d(second -> x_num(), second -> y_num());
    }
    glEnd();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    if(need_sides)
        paint_sides();
    else if(need_diagonals)
    {
        one_or_two = diagonals.front(); //one
        collect_diagonals();
        one_or_two = diagonals.back();  //two
        collect_diagonals();

        paint_diagonals();
    }
    if(need_division)
        paint_division();
    
    QString mousePos;
    QTextStream(&mousePos) << "(" << mousePosition.X() << "," << mousePosition.Y() << ")";
    renderText(10, 20, mousePos);
    renderText(10, 35, progress);

    renderText(300, 10, message);
    renderText(300, 25, area2);

    renderText(10, 65, area1);
    renderText(10, 80, load);
    renderText(10, 95, enter);
    renderText(10, 110, clear);
    renderText(10, 125, save);
    renderText(10, 140, movement);
    renderText(10, 155, zoom);
    renderText(10, 170, zro);
    renderText(10, 185, one);
}

   
void GLWidget::mouseMoveEvent(QMouseEvent *event) 
{
    mousePosition.x = worldPosition.x + ((double)world_width / window_width) * event -> x();
    mousePosition.y = worldPosition.y + (world_height - ((double)world_height / window_height) * event -> y());
    repaint();
}

void GLWidget::keyPressEvent(QKeyEvent* event) 
{
    need_sides = false;
    need_diagonals = false;
    need_division = false;


    switch(event->key()) {

    case Qt::Key_0:

        if(is_already_loaded == true)
        {
            need_sides = true;
            need_diagonals = false;
            need_division = false;
        }
        else
        {
            message.clear();
            QTextStream(&message) << "You need to load polygon first.";
        }

        repaint();

        break;

    case Qt::Key_1:

        if(is_divided == true)
        {
            need_sides = false;
            need_diagonals = true;
            need_division = true;
        }
        else
        {
            message.clear();
            QTextStream(&message) << "You need to divide polygon first.";
        }

        repaint();

        break;

    case Qt::Key_C:
        
        clear_all();

        area2.clear();
        message.clear();
        QTextStream(&message) << "Polygon was deleted.";

        need_sides = false;
        need_diagonals = false;
        need_division = false;

        resizeGL(window_width, window_height);
        repaint();

        break;
        
    case Qt::Key_S:
       
        polygon.save("result.txt");
        message.clear();
        QTextStream(&message) << "Polygon was saved to result.txt";
        
        repaint();
        break;
        
    case Qt::Key_L:
        
        try
        {
            if(is_already_loaded == false)
            {
                polygon.load_2d(LOAD_FILE);
                is_already_loaded = true;
                message.clear();
                QTextStream(&message) << "Polygon was loaded from " << LOAD_FILE;
                world_width = max_width;
                resizeGL(window_width, window_height);
            }
            else
            {
                message.clear();
                QTextStream(&message) << "Polygon has already loaded - you need to clear it to load a new one.";
            }

            need_sides = true;
            need_diagonals = false;
            need_division = false;

        }
        catch(char e[])
        {
            message.clear();
            QTextStream(&message) << e;
            clear_all();
        }


        repaint();

        break;
    
    case Qt::Key_D:

        try
        {
            if(is_divided == false)
            {
                if(num_points > 100)
                {
                    progress.clear();
                    QTextStream(&progress) << "Division is in progress. In command line you can view the progress. The polygon is huge, you can drink a cup of tea. =)";
                }
                repaint();

                polygon.division();
                is_divided = true;
                message.clear();
                QTextStream(&message) << "Polygon was divided.";
            }
            else
            {
                message.clear();
                QTextStream(&message) << "Polygon was already divided.";
            }

            need_sides = false;
            need_diagonals = true;
            need_division = true;

        }
        catch(char e[])
        {
            message.clear();
            QTextStream(&message) << e;
            clear_all();
            resizeGL(window_width, window_height);
        }

        repaint();

        break;

    case Qt::Key_Left:

        worldPosition.x -= world_width / 30;
        resizeGL(window_width, window_height);

        if(is_already_loaded)
            need_sides = true;
        if(is_divided)
        {
            need_sides = false;
            need_diagonals = true;
            need_division = true;
        }

        repaint();
        break;

    case Qt::Key_Right:

        worldPosition.x += world_width / 30;
        resizeGL(window_width, window_height);

        if(is_already_loaded)
            need_sides = true;
        if(is_divided)
        {
            need_sides = false;
            need_diagonals = true;
            need_division = true;
        }

        repaint();
        break;

    case Qt::Key_Down:

        worldPosition.y -= world_height / 30;
        resizeGL(window_width, window_height);

        if(is_already_loaded)
            need_sides = true;
        if(is_divided)
        {
            need_sides = false;
            need_diagonals = true;
            need_division = true;
        }

        repaint();
        break;

    case Qt::Key_Up:

        worldPosition.y += world_height / 30;
        resizeGL(window_width, window_height);

        if(is_already_loaded)
            need_sides = true;
        if(is_divided)
        {
            need_sides = false;
            need_diagonals = true;
            need_division = true;
        }

        repaint();
        break;

    case Qt::Key_Plus:

        world_width -= world_width / 20;
        resizeGL(window_width, window_height);

        if(is_already_loaded)
            need_sides = true;
        if(is_divided)
        {
            need_sides = false;
            need_diagonals = true;
            need_division = true;
        }

        repaint();
        break;

    case Qt::Key_Minus:

        world_width += world_width / 20;
        resizeGL(window_width, window_height);

        if(is_already_loaded)
            need_sides = true;
        if(is_divided)
        {
            need_sides = false;
            need_diagonals = true;
            need_division = true;
        }

        repaint();
        break;

    case Qt::Key_A:

        if(is_already_loaded == true &&  is_divided == true)
        {
            std::vector<Geometry::my_Polygon> answer = polygon.make_pair(res);
            area2.clear();
            double a = (answer.front()).numerical_area();
            double b = (answer.back()).numerical_area();

            QTextStream(&area2) << "S_first = " << (int64_t)a << ", S_second = " << (int64_t)b;

            need_sides = false;
            need_diagonals = true;
            need_division = true;
        }
        else
        {
            message.clear();
            QTextStream(&message) << "You need to load/divide polygon first.";
        }

        repaint();

        break;

    case Qt::Key_Escape:

        close();
        break;

    default:

        event -> ignore();
        break;
    }
}
