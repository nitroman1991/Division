#ifndef SUBSIDIARY
#define SUBSIDIARY

#include "GLWidget.h"

typedef long long int int64;

int64 gcd(int64 a, int64 b);

int64 _abs(int64 a);


long double __abs(long double a);

namespace Geometry {

struct Fraction {

public:

    Fraction();

    Fraction(int64 upper, int64 lower);

    Fraction(const int64 &i);

    Fraction(const Fraction &f);
	
	Fraction(double f, int digits);

    Fraction upload(int64 upper, int64 lower);

    Fraction smart();

    void clear();
    
    Fraction frac_abs();

	double lg_u_() const;

	double lg_l_() const;
    
	Fraction to_decimal();

	bool sharpness() const;

    long double numerical() const;

    friend Fraction operator + (Fraction const &a, Fraction const &b);
    friend Fraction operator - (Fraction const &a, Fraction const &b);
    friend Fraction operator * (Fraction const &a, Fraction const &b);
    friend Fraction operator / (Fraction const &a, Fraction const &b);
    
	friend bool operator < (Fraction const &a, Fraction const &b);
    friend bool operator < (Fraction const &a, int f);
    friend bool operator <= (Fraction const &a, Fraction const &b);
    friend bool operator <= (Fraction const &a, int f);
    friend bool operator > (Fraction const &a, Fraction const &b);
    friend bool operator > (Fraction const &a, int f);
    friend bool operator >= (Fraction const &a, Fraction const &b);
    friend bool operator >= (Fraction const &a, int f);
    friend bool operator == (Fraction a, Fraction b);
    friend bool operator != (Fraction a, Fraction b);
	friend bool operator == (Fraction a, int b);
	friend bool operator != (Fraction a, int b);
	friend struct Point;
    
    int64 getU() const;

    int64 getL() const;

    int sign() const;

private:

    int64 u;
    int64 l;
	double lg_u;
	double lg_l;
	bool is_sharp;

};

struct Point {

public:

    Point();

    Point(int64 const &x0, int64 const &y0);

    Point(int64 const &x0, int64 const &y0, int64 const &s0);
    
    Point(const Point &p);
    
    Point(Fraction X, Fraction Y);

    Point upload(Fraction const &X, Fraction const &Y);
    
    Point upload(int64 const &x0, int64 const &y0);
    
    Point upload(int64 const &x0, int64 const &y0, int64 const &s0);
    
    void smart();

    bool sharpness() const;
	
	friend Point operator - (Point a, Point b);
    
    friend bool operator == (Point a, int i);
    
    friend bool operator != (Point a, int i);
    
    friend bool operator == (Point a, Point b);
    
    friend bool operator != (Point a, Point b);
    
    Fraction getX ();

    Fraction getY ();

    int64 &X ();

    int64 &Y ();

    int64 &S ();
    
    double x_num();
    
    double y_num();
    
    friend void GLWidget::mouseMoveEvent(QMouseEvent *event);

    friend void GLWidget::keyPressEvent(QKeyEvent* event);
    
    ~Point();
    
private:
    
    int64 x;
    int64 y;
    int64 s;
	bool is_sharp;

};

struct Interval{

public:

    Interval();

    Interval(Point a, Point b);

    Interval(Interval const &tmp);

	Interval operator= (Interval const &tmp);

    Interval upload(Point a, Point b);
    
    void clear();
    
    Interval swap();

    Point getS() const;

    Point getE() const;
    
    friend bool operator == (Interval const &a, Interval const &b);
    
    friend bool operator != (Interval const &a, Interval const &b);

    Fraction product (Point p);

    bool intersect_strong (Interval tmp);

	bool intersect_soft (Interval tmp);
    
    ~Interval();


private:
    Point s;
    Point e;
};

Point divide_interval(Interval tmp, Fraction K);

}

#endif
