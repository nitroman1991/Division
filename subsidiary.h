#ifndef SUBSIDIARY
#define SUBSIDIARY

#include <stdint.h>
#include "GLWidget.h"

int64_t gcd(int64_t a, int64_t b);

int64_t _abs(int64_t a);

long double __abs(long double a);

namespace Geometry {

struct Fraction {

public:

    Fraction();

    Fraction(int64_t upper, int64_t lower);

    Fraction(const int64_t &i);

    Fraction(const Fraction &f);
	
	Fraction(double f, int digits);

	Fraction upload(int64_t upper, int64_t lower);

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
    
    int64_t getU() const;

    int64_t getL() const;

    int sign() const;

private:

    int64_t u;
    int64_t l;
	double lg_u;
	double lg_l;
	bool is_sharp;

};

struct Point {

public:

    Point();

    Point(int64_t const &x0, int64_t const &y0);

    Point(int64_t const &x0, int64_t const &y0, int64_t const &s0);
    
    Point(const Point &p);
    
    Point(Fraction X, Fraction Y);

    Point upload(Fraction const &X, Fraction const &Y);
    
    Point upload(int64_t const &x0, int64_t const &y0);
    
    Point upload(int64_t const &x0, int64_t const &y0, int64_t const &s0);
    
    void smart();

    bool sharpness() const;
	
	friend Point operator - (Point a, Point b);
    
    friend bool operator == (Point a, int i);
    
    friend bool operator != (Point a, int i);
    
    friend bool operator == (Point a, Point b);
    
    friend bool operator != (Point a, Point b);
    
    Fraction getX ();

    Fraction getY ();

    int64_t &X ();

    int64_t &Y ();

    int64_t &S ();
    
    double x_num();
    
    double y_num();
    
    friend void GLWidget::mouseMoveEvent(QMouseEvent *event);

    friend void GLWidget::keyPressEvent(QKeyEvent* event);
    
    ~Point();
    
private:
    
    int64_t x;
    int64_t y;
    int64_t s;
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
