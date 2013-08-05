#ifndef POLYGON
#define POLYGON

#include <vector>
#include <list>

#include "subsidiary.h"
#include "GLWidget.h"

class GLWidget;

namespace Geometry {

Fraction triangle_area(Point const &p1, Point const &p2, Point const &p3);

bool is_into_triangle(Point const &candidate, Point const &p1, Point const &p2, Point const &p3, bool is_strong_front_side);

bool is_any_point_in (std::list<Point> list, Point const &p1, Point const &p2, Point const &p3, bool is_strong_front_side);

class Diag;



class my_Polygon {

public:

    my_Polygon();

    my_Polygon(std::vector<Point> pts);

    my_Polygon(std::list<Point> pts);
    
    my_Polygon(my_Polygon const &p);
    
    my_Polygon operator = (my_Polygon const &src);
    
    int size() const;

    std::list<Point> _points() const;
    
    void clear();
    
    Fraction area();

	long double numerical_area();

	bool is_correct_rotation();

	std::list<Point>::iterator find_point(Point p);
    
	std::list<Point>::iterator prev(std::list<Point>::iterator r);

	std::list<Point>::iterator next(std::list<Point>::iterator r);

    Point return_third(Point const &p1, Point const &p2);
    
	bool is_intersects_sides (std::list<Point>::iterator begin, std::list<Point>::iterator end, Interval i);

	Fraction right_diagonal_area(Interval in);

    void save(const char* filename);
    
    void load_2d(const char *filename);
    
    void load_3d(const char *filename);

    std::list<Point> division ();

	std::vector<my_Polygon> make_pair(std::list<Point> result);
    
    ~my_Polygon();

private:

    std::list<Point> points;
    
};

class Diag
{
public:
	Diag();

	Diag(Point &s, Point &e);
	
	void upload_point(std::list<Point>::iterator s_iter, std::list<Point>::iterator e_iter);
	
    Diag operator= (Diag const &d);

	bool operator== (Diag const &d) const;

	bool operator!= (Diag const &d) const;


	Point getS() const;

	Point getE() const;
    
    
    Interval _diagonal(){ return diagonal;}
    
    Fraction _area() const;
    
    Fraction _triangle_area() const;
    
    Fraction _tail_area() const;
    
    int _dir_of_side() const;		//0 - с обоих сторон(вершина хвоста), -1 - слева, 1 - справа.
    
    int _rank() const{ return rank;}

    std::list<Geometry::Diag*> _next() const;

    std::list<Geometry::Diag*> _side_l() const;
    
    std::list<Geometry::Diag*> _side_r() const;
    
    std::list<Geometry::Point>::iterator _third_vertex() const;
    
    std::list<Geometry::Point>::iterator _tail_end() const;
    
    void clear();

	~Diag();

    friend class my_Polygon;
    
    friend std::list<Point> tail_partition(Diag *start, Diag *end, Fraction K, int side_or_vertex, int left_or_right);

	friend std::list<Point> find_division(std::list<Diag*> &diagonals, Fraction total_area);

	friend std::list<Point> make_division(Diag *target_diag, Fraction need_area, Point pivot);
    
    friend void collect_diagonals();
    
private:
	Interval diagonal;
	std::list<Point>::iterator start;
	std::list<Point>::iterator end;

	Fraction area;
	Fraction triangle_area;
	Fraction tail_area;
	int dir_of_side;		//0 - с обоих сторон(вершина хвоста), -1 - слева, 1 - справа.
    int rank;
    std::list<Geometry::Diag*> next;

	std::list<Geometry::Diag*> side_l;
	std::list<Geometry::Diag*> side_r;
	std::list<Geometry::Point>::iterator third_vertex;
	std::list<Geometry::Point>::iterator tail_end;
	
};

}

#endif
