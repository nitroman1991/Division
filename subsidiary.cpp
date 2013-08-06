#include "subsidiary.h"
#include <math.h>

typedef long long int int64;

int64 _abs(int64 a)
{
    if(a < 0)
        a = -a;
    return a;
}

long double __abs(long double a)
{
    if(a < 0)
        a = -a;
    return a;
}

int64 gcd(int64 a, int64 b)
{
    if(b == 0)
        return a;
    else
        return gcd(b, a%b);
}


namespace Geometry {

Point divide_interval(Interval tmp, Fraction K)
{
	Point tmp_point;

	Fraction S_lg_X = tmp.getS().getX();
	Fraction S_lg_Y = tmp.getS().getY();
	Fraction E_lg_X = tmp.getE().getX();
	Fraction E_lg_Y = tmp.getE().getY();

	Fraction rvec_X = E_lg_X - S_lg_X;
	Fraction rvec_Y = E_lg_Y - S_lg_Y;

	if(K.lg_u_() + rvec_X.lg_u_() + S_lg_X.lg_l_() > 17 ||
		K.lg_l_() + rvec_X.lg_l_() + S_lg_X.lg_u_() > 17 ||
		K.lg_u_() + rvec_Y.lg_u_() + S_lg_Y.lg_l_() > 17 ||
		K.lg_l_() + rvec_Y.lg_l_() + S_lg_Y.lg_u_() > 17 ||
		K.lg_l_() + rvec_X.lg_l_() + S_lg_X.lg_l_() > 18 ||
		K.lg_l_() + rvec_Y.lg_l_() + S_lg_Y.lg_l_() > 18)
	{
		double tmp_x = S_lg_X.numerical() + K.numerical() * (E_lg_X.numerical() - S_lg_X.numerical());
		double tmp_y = S_lg_Y.numerical() + K.numerical() * (E_lg_Y.numerical() - S_lg_Y.numerical());
		Fraction x_(tmp_x, 17);	
		Fraction y_(tmp_y, 17);
		return tmp_point.upload(x_, y_);
	}
	else
	{
		Fraction x = S_lg_X + K * rvec_X;
		Fraction y = S_lg_Y + K * rvec_Y;
		return tmp_point.upload(x, y);
	}
}

Point::Point() : x(0), y(0), s(1), is_sharp(true)
{

}

Point::Point(int64 const &x0, int64 const &y0) : x(x0), y(y0), s(1), is_sharp(true)
{

}

Point::Point(int64 const &x0, int64 const &y0, int64 const &s0) : x(x0), y(y0), s(s0), is_sharp(true)
{

}

Point::Point(const Point &p) : x(p.x), y(p.y), s(p.s), is_sharp(p.is_sharp)
{

}

Point::Point(Fraction X, Fraction Y)
{
    int64 tmp = gcd(X.getL(), Y.getL());

		x = X.getU() * (Y.getL() / tmp);
		y = Y.getU() * (X.getL() / tmp);
		s = X.getL() * (Y.getL() / tmp);
		is_sharp = true;


	smart();
}

Point Point::upload(int64 const &x0, int64 const &y0)
{
    x = x0;
    y = y0;
    s = 1;
	is_sharp = true;
    return *this;
}

Point Point::upload(int64 const &x0, int64 const &y0, int64 const &s0)
{
    x = x0;
    y = y0;
    s = s0;
	is_sharp = true;
	smart();
    return *this;
}

Point Point::upload(Fraction const &X, Fraction const &Y)
{
    int64 tmp = gcd(X.getL(), Y.getL());
	double log_tmp = log10((double)_abs(tmp));
	
	if(X.lg_l + Y.lg_u > 19 + log_tmp || X.lg_u + Y.lg_l > 19 + log_tmp || X.lg_l + Y.lg_l > 19 + log_tmp)
	{
		double tmp1 = (double) X.u / X.l;
		double tmp2 = (double) Y.u / Y.l;
        double log_tmp1 = log10(__abs((double)tmp1));
		double log_tmp2 = log10(__abs(tmp2));
		int counter = 0;
		s = 1;
		
		while(log_tmp1 < 18 && log_tmp2 < 18 && counter < 18)
		{
			tmp1 *= 10;
			tmp2 *= 10;
			log_tmp1 += 1;
			log_tmp2 += 1;
			counter += 1;
			s *= 10;
		}

        x = (int64)tmp1;
        y = (int64)tmp2;
		is_sharp = false;
	}
	else
	{
		x = X.getU() * (Y.getL() / tmp);
		y = Y.getU() * (X.getL() / tmp);
		s = X.getL() * (Y.getL() / tmp);
		is_sharp = X.is_sharp && Y.is_sharp;
	}

	smart();
    return *this;
}

void Point::smart()
{
    int64 local_gcd = gcd(_abs(s), gcd(_abs(x), _abs(y)));
    x /= local_gcd;
    y /= local_gcd;
    s /= local_gcd;
    if(s < 0)
    {
        x = -x;
        y = -y;
        s = -s;
    }
}


Point operator- (Point a, Point b)
{
    Point r;
    
	Fraction X_ = a.getX() - b.getX();
	Fraction Y_ = a.getY() - b.getY();
	r.upload(X_, Y_);

	r.smart();
    return r;
}

bool operator == (Point a, Point b)
{
    if (a.x * b.s == b.x * a.s && a.y * b.s == b.y * a.s)
        return true;
    else
        return false;
}

bool operator != (Point a, Point b)
{
    if (a.x * b.s == b.x * a.s && a.y * b.s == b.y * a.s)
        return false;
    else
        return true;
}

Fraction Point::getX ()
{
    Fraction tmp(x, s);
    return tmp;
}

Fraction Point::getY ()
{
    Fraction tmp(y, s);
    return tmp;
}

bool Point::sharpness() const
{
	return is_sharp;
}

int64 &Point::X ()
{
    return x;
}

int64 &Point::Y ()
{
    return y;
}

int64 &Point::S ()
{
    return s;
}

double Point::x_num()
{
    return (double) x / s;
}

double Point::y_num()
{
    return (double) y / s;
}

Point::~Point()
{
    x = 0;
    y = 0;
    s = 1;
}

//________________________________________________________________________________
Fraction::Fraction() : u(0), l(1), lg_u(0), lg_l(0), is_sharp(true)
{

}

Fraction::Fraction(int64 upper, int64 lower) : u(upper), lg_u(log10((double)_abs(u))), is_sharp(true)
{
    if(lower != 0)
	{
        l = lower;
		lg_l = log10((double)_abs(l));
	}
	else
        throw (char *) "Exception: divide on zero in fraction number.";
}

Fraction Fraction::upload(int64 upper, int64 lower)
{
	u = upper;
	l = lower;
	lg_u = log10((double)_abs(u));
	lg_l = log10((double)_abs(l));
	is_sharp = true;
	return *this;
}

Fraction::Fraction(const int64 &i) : u(i), l(1), is_sharp(true)
{
	lg_u = log10((double)_abs(u));
	lg_l = 0;

}

Fraction::Fraction(const Fraction &f)
{
    u = f.u;
	lg_u = f.lg_u;
    l = f.l;
	lg_l = f.lg_l;
	is_sharp = f.is_sharp;
}

Fraction::Fraction(double f, int digits)
{
	double log_f = log10(__abs(f));
	int counter = 0;
	l = 1;
		
	while(log_f < digits && counter < digits)
	{
		f *= 10;
		log_f += 1;
		counter += 1;
		l *= 10;
	}
	
	u = f;
	lg_u = log10(__abs((double)u));
	lg_l = counter;
	is_sharp = false;
}

Fraction Fraction::smart()
{
    int64 local_gcd = gcd(_abs(l), _abs(u));
	double tmp = log10((double)local_gcd);
    l /= local_gcd;
    u /= local_gcd;
	lg_u -= tmp;
	lg_l -= tmp;

	if(l < 0)
    {
        l = -l;
        u = -u;
    }
    return *this;
}

Fraction Fraction::to_decimal()
{
	long double tmp = (long double)u / l;
	long double log_tmp = log10(__abs(tmp));
	int counter = 0;
	l = 1;
		
	while(log_tmp < 8 && counter < 8)
	{
		tmp *= 10;
		log_tmp += 1;
		counter += 1;
		l *= 10;
	}
		
    u = (int64)tmp;
	lg_u = log10(__abs((double)u));
	lg_l = counter;
	is_sharp = false;
	
	return *this;
}

long double Fraction::numerical() const
{
	long double tmp = (long double)u / l;
	return tmp;
}

void Fraction::clear()
{
    u = 0;
    l = 1;
	lg_u = 0;
	lg_l = 0;
	is_sharp = true;
}

Fraction Fraction::frac_abs()
{
	Fraction tmp;
    tmp.l = _abs(l);
    tmp.u = _abs(u);
	tmp.lg_u = lg_u;
    tmp.lg_l = lg_l;
    return tmp;
}

bool Fraction::sharpness() const
{
	return is_sharp;
}

Fraction operator + (Fraction const &a, Fraction const &b)
{
    Fraction r;

	if(a.lg_l + b.lg_u > 18 || a.lg_u + b.lg_l > 18 || a.lg_l + b.lg_l > 19)
	{
		long double tmp1 = (long double) a.u / a.l;
		long double tmp2 = (long double) b.u / b.l;
		long double tmp = tmp1 + tmp2;
		long double log_tmp = log10(__abs(tmp));
		int counter = 0;
		r.l = 1;
		
		while(log_tmp < 18 && counter < 18)
		{
			tmp *= 10;
			log_tmp += 1;
			counter += 1;
			r.l *= 10;
		}
		
        r.u = (int64)tmp;
		r.lg_u = log10(__abs((double)r.u));
		r.lg_l = counter;
		r.is_sharp = false;
	}
	else
	{
		r.u = a.u * b.l + b.u * a.l;
		r.l = a.l * b.l;
		r.lg_u = log10(__abs((double)r.u));
		r.lg_l = a.lg_l + b.lg_l;
		r.is_sharp = a.is_sharp && b.is_sharp;
	}
	
    
    r.smart();
    return r;
}

Fraction operator - (Fraction const &a, Fraction const &b)
{
    Fraction r;
    if(a.lg_l + b.lg_u > 18 || a.lg_u + b.lg_l > 18 || a.lg_l + b.lg_l > 19)
	{
		long double tmp1 = (long double) a.u / a.l;
		long double tmp2 = (long double) b.u / b.l;
		long double tmp = tmp1 - tmp2;
		long double log_tmp = log10(__abs(tmp));
		int counter = 0;
		r.l = 1;
		
		while(log_tmp < 18 && counter < 18)
		{
			tmp *= 10;
			log_tmp += 1;
			counter += 1;
			r.l *= 10;
		}
		
        r.u = (int64)tmp;
		r.lg_u = log10(__abs((double)r.u));
		r.lg_l = counter;
		r.is_sharp = false;
	}
	else
	{
		r.u = a.u * b.l - b.u * a.l;
		r.l = a.l * b.l;
		r.lg_u = log10(__abs((double)r.u));
		r.lg_l = a.lg_l + b.lg_l;
		r.is_sharp = a.is_sharp && b.is_sharp;
	}
	
    r.smart();
    return r;
}

Fraction operator * (Fraction const &a, Fraction const &b)
{
    Fraction r;
	if(a.lg_l + b.lg_l > 19 || a.lg_u + b.lg_u > 19)
	{
		long double tmp1 = (long double) a.u / a.l;
		long double tmp2 = (long double) b.u / b.l;
		long double tmp = tmp1 * tmp2;
		long double log_tmp = log10(__abs(tmp));
		int counter = 0;
		r.l = 1;
		
		while(log_tmp < 18 && counter < 18)
		{
			tmp *= 10;
			log_tmp += 1;
			counter += 1;
			r.l *= 10;
		}
		
        r.u = (int64)tmp;
		r.lg_u = log10(__abs((double)r.u));
		r.lg_l = counter;
		r.is_sharp = false;
	}
	else
	{
		r.u = a.u * b.u;
		r.l = a.l * b.l;
		r.lg_u = a.lg_u + b.lg_u;
		r.lg_l = a.lg_l + b.lg_l;
		r.is_sharp = a.is_sharp && b.is_sharp;
	}
	
    r.smart();
    return r;
}


Fraction operator / (Fraction const &a, Fraction const &b)
{
    
	Fraction r;
	if(a.lg_l + b.lg_u > 19 || a.lg_u + b.lg_l > 19)
	{
		long double tmp1 = (long double) a.u / a.l;
		long double tmp2 = (long double) b.l / b.u;
		long double tmp = tmp1 * tmp2;
		long double log_tmp = log10(__abs(tmp));
		int counter = 0;
		r.l = 1;
		
		while(log_tmp < 18 && counter < 18)
		{
			tmp *= 10;
			log_tmp += 1;
			counter += 1;
			r.l *= 10;
		}
		
        r.u = (int64)tmp;
		r.lg_u = log10(__abs((double)r.u));
		r.lg_l = counter;
		r.is_sharp = false;
	}
	else
	{
		r.u = a.u * b.l;
		r.l = a.l * b.u;
		r.lg_u = a.lg_u + b.lg_l;
		r.lg_l = a.lg_l + b.lg_u;
		r.is_sharp = a.is_sharp && b.is_sharp;
	}
	
    r.smart();
    return r;
}


bool operator < (Fraction const &a, Fraction const &b)
{
    Fraction r;
    r = a - b;
    r.smart();
    if(r.getU() < 0)
        return true;
    else
        return false;
}

bool operator < (Fraction const &a, int f)
{
    Fraction r;
    r.u = a.u - f * a.l;
    r.l = a.l;
    r.smart();
    if(r.getU() < 0)
        return true;
    else
        return false;
}

bool operator <= (Fraction const &a, Fraction const &b)
{
    Fraction r;
    r = a - b;
    r.smart();
    if(r.getU() <= 0)
        return true;
    else
        return false;
}

bool operator <= (Fraction const &a, int f)
{
	Fraction tmp(f);
    Fraction r;
    r = a - tmp;
    r.smart();
    if(r.getU() <= 0)
        return true;
    else
        return false;
}

bool operator > (Fraction const &a, Fraction const &b)
{
    return !(a <= b);
}

bool operator > (Fraction const &a, int f)
{
    return !(a <= f);
}

bool operator >= (Fraction const &a, Fraction const &b)
{
    return !(a < b);
}

bool operator >= (Fraction const &a, int f)
{
    return !(a < f);
}

bool operator == (Fraction a, Fraction b)
{
	if(a.lg_u + b.lg_l <= 19 && a.lg_l + b.lg_u <= 19)
	{
		if (a.u * b.l == b.u * a.l)
			return true;
		else
			return false;
	}
	else
	{
		Fraction tmp = a - b;
		if(a == 0)
			return true;
		else
			return false;
	}
}

bool operator != (Fraction a, Fraction b)
{
    if (a.u * b.l != b.u * a.l)
        return true;
    else
        return false;
}

bool operator == (Fraction a, int b)
{
	if (a.u == b * a.l)
        return true;
    else
        return false;
}

bool operator != (Fraction a, int b)
{
	return !(a == b);
}

int64 Fraction::getU() const
{
    return u;
}

int64 Fraction::getL() const
{
    return l;
}

double Fraction::lg_u_() const
{
    return lg_u;
}

double Fraction::lg_l_() const
{
    return lg_l;
}

int Fraction::sign() const
{
    int count = 0;
    if (u == 0 && l != 0)
        return 0;
    if (u < 0)
        count += 1;
    if (l < 0)
        count += 1;
    if (count % 2 == 0)
        return 1;
    else
        return -1;
}

//____________________________________________________________________________
Interval::Interval() : s(), e()
{

}

Interval::Interval(Point a, Point b) : s(a), e(b)
{

}

Interval Interval::upload(Point a, Point b)
{
    s = a;
    e = b;
    return *this;
}

Interval::Interval(Interval const &tmp) : s(tmp.getS()), e(tmp.getE())
{

}

Interval Interval::operator= (Interval const &tmp)
{
	s = tmp.s;
	e = tmp.e;
	return *this;
}



void Interval::clear()
{
    s.~Point();
    e.~Point();
}

Interval Interval::swap()
{
    Point tmp = s;
    s = e;
    e = tmp;
	return *this;
}

Point Interval::getS () const
{
    return s;
}

Point Interval::getE () const
{
    return e;
}

bool operator == (Interval const &a, Interval const &b)
{
    if((a.s == b.s && a.e == b.e) || (a.s == b.e && a.e == b.s))
        return true;
    else
        return false;
}

bool operator != (Interval const &a, Interval const &b)
{
    if((a.s == b.s && a.e == b.e) || (a.s == b.e && a.e == b.s))
        return false;
    else
        return true;
}

Fraction Interval::product (Point p)
{
    Point tmp_rvec = p - s;
    Point rvec = e - s;
	Fraction r1 = tmp_rvec.getX() * rvec.getY();
	Fraction r2 = tmp_rvec.getY() * rvec.getX();
	Fraction result = (r1 - r2);
    return result;
}

bool Interval::intersect_strong (Interval tmp)
{
	int sign1 = product(tmp.s).sign();
	int sign2 = product(tmp.e).sign();
	
	if (sign1 == -sign2 && sign1 != 0)
    {
		sign1 = tmp.product(s).sign();
		sign2 = (tmp.product(e)).sign();
        if (sign1 == -sign2 && sign1 != 0)
            return true;
        else
            return false;
    }
    else
        return false;
}

bool Interval::intersect_soft (Interval tmp)
{
    if ((product(tmp.s).sign() == product(tmp.e).sign() && (product(tmp.s)).sign() != 0) ||
		(tmp.product(s).sign() == (tmp.product(e)).sign() && (tmp.product(s)).sign() != 0))
		return false;
	if (product(tmp.s).sign() == product(tmp.e).sign() && (product(tmp.s)).sign() == 0)
	{
		Fraction max1;
		Fraction max2;
		Fraction min1;
		Fraction min2;
		if(s.getX() != e.getX())
		{
			max1 = (s.getX() <= e.getX() ? e.getX() : s.getX());
			max2 = (s.getX() <= e.getX() ? e.getX() : s.getX());
			min1 = (s.getX() <= e.getX() ? s.getX() : e.getX());
			min2 = (s.getX() <= e.getX() ? e.getX() : s.getX());
		}
		else
		{
			max1 = (s.getY() <= e.getY() ? e.getY() : s.getY());
			max2 = (s.getY() <= e.getY() ? e.getY() : s.getY());
			min1 = (s.getY() <= e.getY() ? s.getY() : e.getY());
			min2 = (s.getY() <= e.getY() ? e.getY() : s.getY());
		}
		return (max1 >= min2 && max2 >= min1);
	}
	if(product(tmp.s).sign() != product(tmp.e).sign() && tmp.product(s).sign() != (tmp.product(e)).sign())
		return true;
	return false;
}

Interval::~Interval()
{
    s.~Point();
    e.~Point();
}

}
