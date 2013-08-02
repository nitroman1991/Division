#define TEST_MODE 1
#include "Polygon.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
#include <list>

extern int64_t max_width;
extern std::list<Geometry::Diag*> diagonals;
extern std::list<Geometry::Point> res;
std::vector<Geometry::Diag> true_diagonals;

int num_points;


namespace Geometry {
	
std::list<Point>& concat (std::list<Point> &a, std::list<Point> b)
{
	std::list<Point>::iterator r = b.begin();

	while(b.empty() == false)
	{
		a.push_back(b.front());
		b.pop_front();
	}
	return a;
}

template <typename Type, typename Type_iterator>
Type_iterator _prev(Type &pts, Type_iterator r)
{
	if(r == pts.begin())
		return --pts.end();
	else
	{
		Type_iterator tmp = r;
		return --tmp;
	}
}

template <typename Type, typename Type_iterator>
Type_iterator _next(Type &pts, Type_iterator r)
{
	if(r == --pts.end())
		return pts.begin();
    else
	{
		Type_iterator tmp = r;
		return ++tmp;
	}
}


Fraction triangle_area(Point const &p1, Point const &p2, Point const &p3)
{
    Interval i(p2, p3);
    Fraction tmp = i.product(p1) / 2;
    return tmp.frac_abs();
}


bool my_Polygon::is_intersects_sides (std::list<Point>::iterator begin, std::list<Point>::iterator end, Interval i)
{
	Interval tmp;
	for(std::list<Point>::iterator r = begin; r != end; r = next(r))
	{
		tmp.upload(*r, *next(r));
        if(i.intersect_soft(tmp) == true)
			return  true;
	}
	return false;
}


bool is_into_triangle(Point const &candidate, Point const &p1, Point const &p2, Point const &p3, bool is_strong_front_side)
{
    Interval i1(p1, p2);
    Interval i2(p2, p3);
    Interval i3(p3, p1);
	if(is_strong_front_side == false)
	{
		if(i1.product(candidate).sign() < 0 && i2.product(candidate).sign() < 0 && i3.product(candidate).sign() < 0)
			return true;
		else
			return false;
	}
	else
	{
		if(i1.product(candidate).sign() < 0 && i2.product(candidate).sign() < 0 && i3.product(candidate).sign() <= 0)
			return true;
		else
			return false;
	}
}

bool is_any_point_in (std::list<Point> list, Point const &p1, Point const &p2, Point const &p3, bool is_strong_front_side)
{
    for(std::list<Point>::iterator r = list.begin(); r != list.end(); r++)
    	if(is_into_triangle(*r, p1, p2, p3, is_strong_front_side) == true)
            return true;
    return false;
}

//------------------------------------------------------------------------------------------
Diag::Diag() : diagonal(), area(0, 1), triangle_area(0, 1), tail_area(0, 1), rank(0)
{

} 

Diag::Diag(Point &s, Point &e) : diagonal(s, e), area(0, 1), rank(0)
{
		
}

void Diag::upload_point(std::list<Point>::iterator s_iter, std::list<Point>::iterator e_iter)
{
	diagonal.upload(*s_iter, *e_iter);
	start = s_iter;
	end = e_iter;
    area = 0;
}

Diag Diag::operator= (Diag const &d)
{
	diagonal = d.diagonal;
	start = d.start;
	end = d.end;
	area = d.area;
	triangle_area = d.triangle_area;
	tail_area = d.tail_area;
	dir_of_side = d.dir_of_side;
    rank = d.rank;
	next = d.next;
	side_l = d.side_l;
	side_r = d.side_r;
	third_vertex = d.third_vertex;
	tail_end = d.tail_end;
	return *this;
}

bool Diag::operator== (Diag const &d) const
{
	return diagonal == d.diagonal;
}

bool Diag::operator!= (Diag const &d) const
{
	return !(diagonal == d.diagonal);
}


Point Diag::getS() const
{
	return diagonal.getS();
}

Point Diag::getE() const
{
	return diagonal.getE();
}

Fraction Diag::_area() const
{
    return area;
}

Fraction Diag::_triangle_area() const
{
    return triangle_area;
}

Fraction Diag::_tail_area() const
{
    return tail_area;
}

int Diag::_dir_of_side() const
{
    return dir_of_side;
}

std::list<Geometry::Diag*> Diag::_next() const
{
    return next;
}

std::list<Geometry::Diag*> Diag::_side_l() const
{
    return side_l;
}

std::list<Geometry::Diag*> Diag::_side_r() const
{
    return side_r;
}

std::list<Geometry::Point>::iterator Diag::_third_vertex() const
{
    return third_vertex;
}

std::list<Geometry::Point>::iterator Diag::_tail_end() const
{
    return tail_end;
}


void Diag::clear()
{
    diagonal.clear();
	side_l.clear();
	side_r.clear();

	area = 0;
    rank = 0;
	triangle_area = 0;
	tail_area = 0;
	dir_of_side = 0;
	next.clear();
}

Diag::~Diag()
{

}

//-------------------------------------------------------------------------------------

my_Polygon::my_Polygon()
{

}

my_Polygon::my_Polygon(std::vector<Point> pts)
{
    clear();
    for (std::vector<Point>::iterator it = pts.begin(); it != pts.end(); it++)
        points.push_back(*it);
}

my_Polygon::my_Polygon(std::list<Point> pts)
{
    clear();
    for (std::list<Point>::iterator it = pts.begin(); it != pts.end(); it++)
        points.push_back(*it);
}

my_Polygon::my_Polygon(my_Polygon const &p)
{
    points = p._points();
}

my_Polygon my_Polygon::operator = (my_Polygon const &src)
{
    points = src._points();
    return *this;
}


int my_Polygon::size() const
{
    return points.size();
}

std::list<Point> my_Polygon::_points() const
{
    return points;
}

void my_Polygon::clear()
{
    points.clear();
}

long double my_Polygon::numerical_area()
{
    long double area = 0;
    Fraction a_x;
    Fraction a_y;
    Fraction b_x;
    Fraction b_y;

    Point p1;
    Point p2;
    if(points.size() > 2)
    {
        for (std::list<Point>::iterator it = points.begin(); it != points.end(); it++)
        {
            p1 = *it;
			p2 = *next(it);
			
			a_x = p1.getX();
            a_y = p1.getY();
            b_x = p2.getX();
            b_y = p2.getY();
			
            area = area + ((long double)(b_y.numerical() + a_y.numerical()) / 2) * (b_x.numerical() - a_x.numerical());
        }
    }

    return __abs(area);
}

Fraction my_Polygon::area()
{
    Fraction area;
    Fraction a_x;
    Fraction a_y;
    Fraction b_x;
    Fraction b_y;

    Point p1;
    Point p2;
    if(points.size() > 2)
    {
        for (std::list<Point>::iterator it = points.begin(); it != points.end(); it++)
        {
            p1 = *it;
            p2 = *next(it);

            a_x = p1.getX();
            a_y = p1.getY();
            b_x = p2.getX();
            b_y = p2.getY();

            area = area + ((b_y + a_y) / 2) * (b_x - a_x);
        }
    }

    return area;
}

bool my_Polygon::is_correct_rotation()
{
	if(area() <= 0)
		return true;
	else
		return false;
}

std::list<Point>::iterator my_Polygon::prev(std::list<Point>::iterator r)
{
	if(r == points.begin())
		return --points.end();
    else
	{
		std::list<Point>::iterator tmp = r;
		return --tmp;
	}
}
     
std::list<Point>::iterator my_Polygon::next(std::list<Point>::iterator r)
{
	if(r == --points.end())
		return points.begin();
    else
	{
		std::list<Point>::iterator tmp = r;
		return ++tmp;
	}
}
std::list<Point>::iterator my_Polygon::find_point(Point p)
{
    for (std::list<Point>::iterator iter = points.begin(); iter != points.end(); iter++)
        if(*iter == p)
            return iter;
    return points.end();
}

Point my_Polygon::return_third(Point const &p1, Point const &p2)
{
	std::list<Point>::iterator result;
	int count = 0;
	if(points.size() == 3)
    {
        for (std::list<Point>::iterator it = points.begin(); it != points.end(); it++)
		{
			if(*it == p1)
				count += 1;
			if(*it == p2)
				count += 2;
			if(*it != p1 && *it != p2)
				result = it;
		}
    }
	if(count == 3)
		return *result;
	else
		return p1;
}

void my_Polygon::save(const char* filename)
{
    std::ofstream ofile;
	ofile.open(filename, std::ios::out);
    ofile << size() << std::endl;
    for (std::list<Point>::iterator p = points.begin(); p != points.end(); p++)
    {
        ofile << p -> X() << " " << p -> Y() << " " << p -> S() << std::endl;
    }
    ofile.close();
}

void my_Polygon::load_2d(char const *filename)
{
    num_points = 0;
    int64_t k = 1;
    int64_t a = 0;
    int64_t b = 0;
    Point tmp_point;
    std::ifstream ifile;
        
    ifile.open(filename, std::ios::in);
    if(ifile.is_open() == false)
        throw (char *)"No such file";

    ifile >> num_points >> k;
    for (int i = 0; i < num_points; i++)
    {
        ifile >> a >> b;
        if(_abs(a) * k > 1000000000 || _abs(b) * k > 1000000000)
            throw (char *) "Input data is incorrect: some limit was exceeded.";
		else
        {
            if(_abs(a) * k > max_width)
                max_width = _abs(a) * k;
            tmp_point.upload(a * k, b * k);
            points.push_back(tmp_point);
        }
    }
	if(is_correct_rotation() == false)
        throw (char *)"Error: not correct rotation.";
}

void my_Polygon::load_3d(const char *filename)
{
    num_points = 0;
    long int a = 0;
    long int b = 0;
    long int c = 0;
    Point tmp_point;
    std::ifstream ifile;
    
    ifile.open(filename, std::ios::in);
    if(ifile.is_open() == false)
        throw (char *)"No such file";

    ifile >> num_points;
    for (int i = 0; i < num_points; i++)
    {
        ifile >> a >> b >> c;
        if(_abs(a/c) > 1000000000 || _abs(b/c) > 1000000000)
            throw (char *) "Input data is incorrect: some limit was exceeded.";
        else
        {
            if(_abs(a) > max_width * _abs(c))
                max_width = _abs(a) /  _abs(c);
            tmp_point.upload(a, b, c);
            points.push_back(tmp_point);
        }
    }
    if(is_correct_rotation() == false)
        throw (char *) "Error: not correct rotation.";
}

my_Polygon::~my_Polygon()
{
    points.clear();
}


std::list<Point> tail_partition(Diag *start, Diag *end, Fraction K, int side_or_vertex, int left_or_right)
{
    Point tmp_point;        //если side_or_vertex = 0, делим последний треуг. отрезком через противоположную вершину
    Fraction K_1;           //если side_or_vertex = 1, ныряем в соседнюю сторону в конце, а не в вершину
    Fraction K_1_second;    //left_or_right - определяем, ныряем вправо или влево в конце(если вообще ныряем)
	Fraction K_2;
	Fraction K_2_second;
    Fraction x;
    Fraction y;
	
	std::list<Point> result;
	Diag *tmp = start;
	int64_t upper = 0;
	int64_t lower = 0;

		
		
	upper = (int64_t) sqrt((long double)K.getU());			//находим коэффициенты, где должны лежать рассекающие точки на диагоналях.
    lower = (int64_t)sqrt((long double)K.getL()) + 1;       //можно было бы проще, но так получаем точные, рациональные значения вместо приближенных
	K_1.upload(upper, lower);
	int64_t temp = 1;
	while(K >= K_1 * K_1)
	{
		K_1.upload(K_1.getU() + temp, K_1.getL() + temp);
		//temp *= 2;
	}
	K_1_second = K / K_1;
	if(K_1 > 1 || K_1 < 0 || K_1_second > 1 || K_1_second < 0)
        throw (char *) "Error - K_1 or K_1_second not in bounds";
	


	upper = (int64_t) sqrt((long double)(1 - K).getU());
	lower = (int64_t) sqrt((long double)(1 - K).getL()) + 1;
	K_2.upload(upper, lower);
	temp = 1;
	while(1 - K >= K_2 * K_2)
	{
		K_2.upload(K_2.getU() + temp, K_2.getL() + temp);
		//temp *= 2;
	}

	K_2_second = 1 - (1 - K) / K_2;
	K_2 = 1 - K_2;
	if(K_2 > 1 || K_2 < 0 || K_2_second > 1 || K_2_second < 0)
        throw (char *) "Error - K_2 or K_2_second not in bounds";

	std::list<Point> examine;
	while((*tmp) != *end)
	{
        if(tmp -> dir_of_side == -1)
		{
			if(TEST_MODE == 1)
			{
                examine.push_back((tmp -> side_l.front()) -> getS());
                examine.push_back((tmp -> side_l.front()) -> getE());
			}

            tmp_point = divide_interval(tmp -> diagonal, K_1_second);       //делим условно нижнюю(если идем вверх по диагоналям) диагональ
			if(result.empty() == true || tmp_point != result.back())
				result.push_back(tmp_point);
			
			if(TEST_MODE == 1)
			{
				examine.push_back(tmp_point);
                double tmp_x = tmp -> getS().getX().numerical() + K_1_second.numerical() * (tmp -> getE().getX().numerical() - tmp -> getS().getX().numerical());
                double tmp_y = tmp -> getS().getY().numerical() + K_1_second.numerical() * (tmp -> getE().getY().numerical() - tmp -> getS().getY().numerical());
				double tmp_x_2 = tmp_point.getX().numerical();
				double tmp_y_2 = tmp_point.getY().numerical();
				if(__abs(tmp_x - tmp_x_2) > 0.01 || __abs(tmp_y - tmp_y_2) > 0.01)	//тест на верность приближения
                    throw (char *) "Error in calculations";
				
                Fraction ttmp = tmp -> diagonal.product(tmp_point);
				if(tmp_point.sharpness() == true && ttmp.sharpness() == true && ttmp != 0)	//тест на принадлежность точки отрезку в случае точного решения
                    throw (char *) "Error in calculations";
			}


            long double tra = tmp -> triangle_area.numerical();
			
            tmp = tmp -> next.front();
            tmp_point = divide_interval(tmp -> diagonal, K_1);  //теперь - верхнюю, уже с другим коэффициентом
			result.push_back(tmp_point);

			
			if(TEST_MODE == 1)
			{
                double tmp_x = tmp -> getS().getX().numerical() + K_1.numerical() * (tmp -> getE().getX().numerical() - tmp -> getS().getX().numerical());
                double tmp_y = tmp -> getS().getY().numerical() + K_1.numerical() * (tmp -> getE().getY().numerical() - tmp -> getS().getY().numerical());
				double tmp_x_2 = tmp_point.getX().numerical();
				double tmp_y_2 = tmp_point.getY().numerical();
				if(__abs(tmp_x - tmp_x_2) > 0.01 || __abs(tmp_y - tmp_y_2) > 0.01)	//тест на верность приближения
                    throw (char *) "Error in calculations.";

                Fraction ttmp = tmp -> diagonal.product(tmp_point);
				if(tmp_point.sharpness() == true && ttmp.sharpness() == true && ttmp != 0)	//тест на принадлежность точки отрезку в случае точного решения
                    throw (char *) "Error in calculations.";

				examine.push_back(tmp_point);
				my_Polygon ex(examine);
				long double ex_K_ = (ex.numerical_area());
				long double ex_K = ex_K_ / tra;
				long double K_ = (1 - K.numerical());
				if(__abs(ex_K - K_) > 0.00001)
                    throw (char *)"Error in calculations";		//тест на верность коэффициента разбиения
				examine.clear();
			}
		}
		else
		{
			if((*tmp).dir_of_side == 1)
			{
				if(TEST_MODE == 1)
				{
                    examine.push_back((tmp -> side_r.front()) -> getE());
                    examine.push_back((tmp -> side_r.front()) -> getS());
				}
				
                tmp_point = divide_interval(tmp -> diagonal, K_2_second);
				if(result.empty() == true || tmp_point != result.back())
					result.push_back(tmp_point);
				
				if(TEST_MODE == 1)
				{
					examine.push_back(tmp_point);
                    double tmp_x = tmp -> getS().getX().numerical() + K_2_second.numerical() * (tmp -> getE().getX().numerical() - tmp -> getS().getX().numerical());
                    double tmp_y = tmp -> getS().getY().numerical() + K_2_second.numerical() * (tmp -> getE().getY().numerical() - tmp -> getS().getY().numerical());
					double tmp_x_2 = tmp_point.getX().numerical();
					double tmp_y_2 = tmp_point.getY().numerical();
					if(__abs(tmp_x - tmp_x_2) > 0.01 || __abs(tmp_y - tmp_y_2) > 0.01)	//тест на верность приближения
                        throw (char *) "Error in calculations";

                    Fraction ttmp = tmp -> diagonal.product(tmp_point);
					if(tmp_point.sharpness() == true && ttmp.sharpness() == true && ttmp != 0)	//тест на принадлежность точки отрезку в случае точного решения
                        throw (char *) "Error in calculations";
				}

                long double tra = tmp -> triangle_area.numerical();
				
                tmp = tmp -> next.front();
                tmp_point = divide_interval(tmp -> diagonal, K_2);
				result.push_back(tmp_point);

				if(TEST_MODE == 1)
				{
                    double tmp_x = tmp -> getS().getX().numerical() + K_2.numerical() * (tmp -> getE().getX().numerical() - tmp -> getS().getX().numerical());
                    double tmp_y = tmp -> getS().getY().numerical() + K_2.numerical() * (tmp -> getE().getY().numerical() - tmp -> getS().getY().numerical());
					double tmp_x_2 = tmp_point.getX().numerical();
					double tmp_y_2 = tmp_point.getY().numerical();
					if(__abs(tmp_x - tmp_x_2) > 0.01 || __abs(tmp_y - tmp_y_2) > 0.01)	//тест на верность приближения
                        throw (char *) "Error in calculations";

					Fraction ttmp = (*tmp).diagonal.product(tmp_point);	
					if(tmp_point.sharpness() == true && ttmp.sharpness() == true && ttmp != 0)	//тест на принадлежность точки отрезку в случае точного решения
                        throw (char *) "Error in calculations";

					examine.push_back(tmp_point);
					my_Polygon ex(examine);
					long double ex_K_ = (ex.numerical_area());
					long double ex_K = ex_K_ / tra;
					long double K_ = K.numerical();
					if(__abs(ex_K - K_) > 0.00001)
                        throw (char *) "Error in calculations";		//тест на верность коэффициента разбиения
					examine.clear();
                }
			}
			else
                throw (char *) "Error in calculations";
		}
	}

    if(side_or_vertex == 0) //разбираемся с последней точкой - берем в качестве ее или вершину...
	{
        tmp_point = divide_interval(tmp -> diagonal, K);
		if(result.empty() == true || tmp_point != result.back())
			result.push_back(tmp_point);
        result.push_back(*(tmp -> third_vertex));
	}
    if(side_or_vertex == 1) // ..или делим смежную сторону
	{
		if((*end).dir_of_side == -1 || left_or_right == -1)
		{
            tmp_point = divide_interval(end -> diagonal, K_2_second);
			if(result.empty() == true || tmp_point != result.back())
				result.push_back(tmp_point);
			
            tmp_point = divide_interval((tmp -> side_l.front()) -> diagonal, K_2);
			result.push_back(tmp_point);
		}

        if(end -> dir_of_side == 1 || left_or_right == 1)
		{
            tmp_point = divide_interval(end -> diagonal, K_1_second);
			if(result.empty() == true || tmp_point != result.back())
				result.push_back(tmp_point);
			
            tmp_point = divide_interval((tmp -> side_r.front()) -> diagonal, K_1);
			result.push_back(tmp_point);
		}
	}
	return result;
}
std::list<Point> make_division(Diag *target_diag, Fraction need_area, Point pivot)
{
    std::list<Point> result;
	Point tmp_point;
	Fraction K = 0;

    Fraction tr_area = triangle_area(target_diag -> getS(), target_diag -> getE(), pivot);
    if(tr_area + target_diag -> area < need_area)
        throw (char *) "Error: incorrectly division triangle found.";
	
	//сначала халявные случаи

	if(need_area == 0)		//если уже сидим на нужной диагонали справа
	{
        result.push_back(target_diag -> getS());
		return result;
	}
	
    if(tr_area + target_diag -> area == need_area) //если нужная - левая
	{
        result.push_back(target_diag -> getE());
		return result;
	}

    if(target_diag -> area == 0 || need_area < tr_area)	//если а) мы встали на сторону исходного б)нам хватит куска опорного треугольника
	{
        K = need_area / tr_area;
        tmp_point = divide_interval(target_diag -> diagonal, K);
        result.push_back(tmp_point);
        result.push_back(target_diag -> getS());
		return result;
	}

    if(need_area > target_diag -> area)	//если без опорного треугольника не хватает площади - берем её всю, и добираем площадь от треугольника.
	{
        K = (need_area - target_diag -> area) / tr_area;
        tmp_point = divide_interval(target_diag -> diagonal, K);
        result.push_back(tmp_point);
        result.push_back(target_diag -> getE());
		return result;
	}
	

	//для сложного случая нужно делить текущий хвост и "нырять" в какую то внутреннюю диагональ/сторону/вершину и там продолжат делить.
	Fraction sum_tr = tr_area;
	Fraction sum_right = 0;
	Fraction sum_left = 0;
	Fraction sum_tr_tmp = tr_area;
	Fraction sum_right_tmp = 0;
	Fraction sum_left_tmp = 0;

	int guard = 0;
	Diag *tmp = target_diag;

    while(tmp -> next.empty() == false)		//выяснили, искомая диагональ лежит справа от конца хвоста, или слева - от этого зависит, откуда осуществляем обход.
	{
        if(tmp -> dir_of_side == 1)
            sum_right = sum_right + (tmp -> side_r.front()) -> area;
        tmp = tmp -> next.front();
	}
	
	Diag *previous = target_diag;
    if(sum_right + target_diag -> tail_area + (tmp -> side_r.front()) -> area > need_area)		//	обходим справа
	{
		sum_right = 0;
		tmp = target_diag;
		while(guard == 0 || sum_tr + sum_right <= need_area)		//проверка условия на площади происходит, только когда добавляем что то справа
		{
			if(guard == 1)
			{
				sum_tr_tmp = sum_tr;
				sum_right_tmp = sum_right;
				guard = 0;
			}
			
            sum_tr = sum_tr + tmp -> triangle_area;
            if(tmp -> dir_of_side == 1 || tmp -> dir_of_side == 0)
			{
                sum_right = sum_right + (tmp -> side_r.front()) -> area;
				guard = 1;
			}
            if(tmp -> next.empty() == false)
			{
				previous = tmp;
                tmp = tmp -> next.front();
			}
			else
			{
				previous = tmp;
				break;
			}
		}
        if(sum_right < need_area) //относительно просто - если не нужно рекурсивно делить еще что-то, а достаточно просто взять определенный кусок хвоста
		{
			K = (need_area - sum_right) / sum_tr;
			
			if(tr_area == 0)
				return tail_partition(target_diag, previous, K, 0, 1);
			else
			{
                tmp_point = divide_interval(target_diag -> diagonal, K);
                result = tail_partition(target_diag, previous, K, 0, 1);

                if(result.empty() == true || result.front() != tmp_point)
					result.push_front(tmp_point);
			}
		}
		else
        {       //самое сложное - если делением хвоста не справиться. Рассчитываем новые коэффициенты для хвоста и рекурсивного деления куска полигона, берущего начало в какой то диагонали
			Fraction K_side = (need_area - (sum_tr_tmp + sum_right_tmp)) / ((sum_tr + sum_right) - (sum_tr_tmp + sum_right_tmp));
			K = (K_side * (sum_tr - sum_tr_tmp) + sum_tr_tmp) / sum_tr;
			
            std::list<Point> l1 = tail_partition(target_diag, previous, K, 1, 1);
            std::list<Point> l2 = make_division(previous -> side_r.front(), K_side * (previous -> side_r.front()) -> area, previous -> getS());
			if(l1.empty() == false && l2.empty() == false && l1.back() == l2.front())	//если есть пересечение списков - исправляем
				l2.pop_front();
            result = concat(l1, l2);        //сшиваем ломаные
			if(tr_area != 0)
			{
                tmp_point = divide_interval(target_diag -> diagonal, K);

                if(result.empty() == true || result.front() != tmp_point)	//если нет пересечения списков - добавляем точку
					result.push_front(tmp_point);
			}
		}
	}
    else        //если это все где то слева от вершины хвоста, а не справа, как выше. Все протекает аналогично
	{
		sum_right = 0;
		tmp = target_diag;
        need_area = target_diag -> area + tr_area - need_area;
		while(guard == 0 || sum_tr + sum_left <= need_area)		//проверка условия на площади происходит, только когда добавляем что то справа
		{
			if(guard == 1)
			{
				sum_tr_tmp = sum_tr;
				sum_left_tmp = sum_left;
				guard = 0;
			}
			sum_tr = sum_tr + (*tmp).triangle_area;
            if(tmp -> dir_of_side == -1 || tmp -> dir_of_side == 0)
			{
                sum_left = sum_left + (tmp -> side_l.front()) -> area;
				guard = 1;
			}
            if(tmp -> next.empty() == false)
			{
				previous = tmp;
                tmp = tmp -> next.front();
			}
			else
			{
				previous = tmp;
				break;
			}
		}
		if(sum_left < need_area)
		{
			K = 1 - (need_area - sum_left) / sum_tr;
			result = tail_partition(target_diag, previous, K, 0, -1);
				
			if(tr_area == 0)
				return result;
			else
			{
                tmp_point = divide_interval(target_diag -> diagonal, K);
				if(result.empty() == true || result.front() != tmp_point)
					result.push_front(tmp_point);
			}
		}
		else
		{
			Fraction K_side = (need_area - (sum_tr_tmp + sum_left_tmp)) / ((sum_tr + sum_left) - (sum_tr_tmp + sum_left_tmp));
			K = (K_side * (sum_tr - sum_tr_tmp) + sum_tr_tmp) / sum_tr;
			K_side = 1 - K_side;
			K = 1 - K;
			
			std::list<Point> l1 = tail_partition(target_diag, previous, K, 1, -1);
            std::list<Point> l2 = make_division(previous -> side_l.front(),  K_side * (previous -> side_l.front()) -> area, previous -> getE());
			if(l1.empty() == false && l2.empty() == false && l1.back() == l2.front())	//если есть пересечение списков - исправляем
				l2.pop_front();
			result = concat(l1, l2);
			if(tr_area != 0)
			{
                tmp_point = divide_interval(target_diag -> diagonal, K);
                if(result.empty() == true || result.front() != tmp_point)	//если нет пересечения списков - добавляем точку
					result.push_front(tmp_point);
			}
		}
	}


	return result;

}

std::list<Point> find_division(std::list<Diag*> &diagonals, Fraction total_area)
{
	Diag *prev_diag = NULL;
	
    if(diagonals.front() -> area + diagonals.back() -> area != total_area)
        throw (char *) "Exception: incorrectly calculated areas.";
    if(2 * (diagonals.front() -> area) >= total_area)	//выбираем нужную половину, где будем искать место разреза
		prev_diag = diagonals.front();
	else
		prev_diag = diagonals.back();
	
	Point pivot;
	Fraction need_area = 0;
	Diag *target_diag = NULL;
	Diag *tmp = prev_diag;

    std::cout << "Start searching the division:" << std::endl;
    int count = 1;

    while(2 * tmp -> area >= total_area)
	{
        if(tmp -> next.empty() == false)
		{
			prev_diag = tmp;
            tmp = tmp -> next.front();

            std::cout << count++ << std::endl;
		}
        else		// перешли в самую верхнюю диагональ хвоста( где нет следующей, а есть соседи, правый и левый
		{
            if(tmp -> dir_of_side != 0)
                throw (char *) "Error in constructing hierarchy.";

            if(2 * (tmp -> side_l.front()) -> area > total_area)	//можем уйти влево в поиске нужной диагонали - уходим
			{
				prev_diag = tmp;
                tmp = tmp -> side_l.front();

                std::cout << count++ << std::endl;
			}
			else
			{
                if(2 * (tmp -> side_r.front()) -> area > total_area)	//аналогично для направления вправо
				{
					prev_diag = tmp;
                    tmp = tmp -> side_r.front();

                    std::cout << count++ << std::endl;
				}
				else	//режем здесь(выясним только, справа или слева выгодней резать)
				{
                    if( (tmp -> side_r.front()) -> area >= (tmp -> side_l.front()) -> area)
					{
                        pivot = tmp -> getS();
                        need_area = total_area / 2 - (tmp -> side_r.front()) -> area;
                        target_diag = tmp -> side_l.front();
					}
					else
					{
                        pivot = tmp -> getE();
                        need_area = tmp -> area - total_area / 2;
                        target_diag = tmp -> side_r.front();
					}
					
                    res = make_division(target_diag, need_area, pivot);
					res.push_front(pivot);
					return res;
				}
			}
		}
	}
	
    if(prev_diag -> dir_of_side == -1)	//эти случаи - если нужный разрез лежит между какими-то обычными диагоналями, а не в хвосте
    {                                   //тогда режем кусок полигона, зажатый между этими диагоналями
        pivot = prev_diag -> getS();
        target_diag = prev_diag -> side_l.front();
        need_area = total_area / 2 - (tmp -> area);
	}
    if(prev_diag -> dir_of_side == 1)
	{
        pivot = prev_diag -> getE();
        target_diag = prev_diag -> side_r.front();
        need_area = (prev_diag -> area) - total_area / 2;
	}

    res = make_division(target_diag, need_area, pivot);
	res.push_front(pivot);
	return res;
	
}


std::vector<my_Polygon> my_Polygon::make_pair(std::list<Point> res)
{
	std::list<Point>::iterator j = find_point(res.front());

	std::list<Point> poly1;

	while(j != find_point(res.back()) )
	{
		poly1.push_back(*j);
		j = next(j);
	}

	for(std::list<Point>::iterator t = --res.end(); t != res.begin(); t--)
		poly1.push_back(*t);

	res.pop_back();
	Interval i(*j, *next(j));
	Fraction r = i.product(res.back());
	if(r.sharpness() == true && r == 0)
		j = next(j);
	while(j != find_point(res.front()) )
	{
		res.push_back(*j);
		j = next(j);
	}
	my_Polygon my_Polygon1(poly1);
	my_Polygon my_Polygon2(res);

	std::vector<my_Polygon> answer;
	answer.push_back(my_Polygon1);
	answer.push_back(my_Polygon2);
	

	return answer;
}

std::list<Point> my_Polygon::division ()
{
	if(is_correct_rotation() == false)
        throw (char *) "Error: rotation is not correct";

    true_diagonals.resize(2 * points.size() - 2);	//резервируем место под диагонали
	
    std::list<Point> ban_list;
    std::list<Diag*> buffer;
	std::list<Diag*> tmp_diag;
	
	Diag tmp;
	Diag *next_diag = NULL;
	
	Fraction triangle;
	int guard = 0;
	
	int i = 0;
    int now_rank = 0;
    int count = 1;

	for(std::list<Point>::iterator t = points.begin(); t != points.end(); t++)
	{
        tmp.upload_point(t, next(t));
		true_diagonals[i] = tmp;		//добавлем стороны полигона
		diagonals.push_back(&true_diagonals[i]);
		i++;
	}

    std::cout << "Start constructing hierarchy" << std::endl;

	std::list<Diag*>::iterator r;
	std::list<Diag*>::iterator left;
    std::list<Diag*>::iterator right;
    while(diagonals.size() > 2)		//строим структуру полигона так - на текущем уровне срезаем уши в одном месте(справа от текущей диагонали, если можем,  и слева),
    {								//до тех пор, пока не можем этого сделать ни справа, ни слева. Тогда запоминаем последнюю проведенную диагональ как диагональ нового "уровня",
		r = diagonals.begin();		//и переходим к соседним. Получаем новый полигон, построенный из диагоналей нового уровня. Исходный получается из этого пришиванием всех ушей к диагоналям нового уровня.
		while(tmp_diag.size() < diagonals.size() && diagonals.size() > 2)	//диагонали несут информацию о следующей при обходе, о боковых сторонах/диагоналях. Полученная структура наилучшим образом позволяет 
		{															//найти место, где нужно резать, и эффективно осуществить этот разрез.
			left = _prev(diagonals, r);
			right = _next(diagonals, r);
			if(((*r) -> diagonal).product(((*right) -> diagonal).getE()) < 0 &&
				is_any_point_in(points, ((*r) -> diagonal).getS(), ((*r) -> diagonal).getE(), ((*right) -> diagonal).getE(), true) == false &&
                (ban_list.empty() == true || ((*r) -> getE() != ban_list.front() && (*r) -> getE() != ban_list.back())))
			{
				triangle = triangle_area(((*r) -> diagonal).getS(), ((*r) -> diagonal).getE(), ((*right) -> diagonal).getE());
				true_diagonals[i].upload_point((*r) -> start,(*right) -> end);
				true_diagonals[i].area = triangle + (*r) -> area + (*right) -> area;
				true_diagonals[i].triangle_area = triangle;
                true_diagonals[i].rank = now_rank;
				true_diagonals[i].side_l.push_back(*right);
				true_diagonals[i].third_vertex = (*r) -> end;
				
				if(guard == 0)
				{
					true_diagonals[i].side_r.push_back(*r);
					true_diagonals[i].dir_of_side = 0;
					true_diagonals[i].tail_end = (*r) -> end;
					true_diagonals[i].tail_area = triangle;
					guard = 1;
				}
				else
				{
					true_diagonals[i].next.push_back(next_diag);
					true_diagonals[i].dir_of_side = -1;
					true_diagonals[i].tail_end = next_diag -> tail_end;
					true_diagonals[i].tail_area = triangle + next_diag -> tail_area;
				}
				next_diag = &true_diagonals[i];

				diagonals.erase(r);
				diagonals.erase(right);
				diagonals.insert(_next(diagonals, left), &true_diagonals[i++]);
				r = _next(diagonals, left);

                std::cout << count++ << std::endl;
			}
			else
			{
				if(((*r) -> diagonal).product(((*left) -> diagonal).getS()) < 0 &&
					is_any_point_in(points, ((*left) -> diagonal).getS(), ((*left) -> diagonal).getE(), ((*r) -> diagonal).getE(), true) == false &&
                (ban_list.empty() == true || ((*r) -> getS() != ban_list.front() && (*r) -> getS() != ban_list.back())))
				{
					if(buffer.empty() == false && buffer.back() == *left)
						buffer.pop_back();
					triangle = triangle_area(((*left) -> diagonal).getS(), ((*left) -> diagonal).getE(), ((*r) -> diagonal).getE());
					true_diagonals[i].upload_point((*left) -> start, (*r) -> end);
					true_diagonals[i].area = triangle + (*left) -> area + (*r) -> area;
					true_diagonals[i].triangle_area = triangle;
                    true_diagonals[i].rank = now_rank;
					true_diagonals[i].side_r.push_back(*left);
					true_diagonals[i].third_vertex = (*r) -> start;
					if(guard == 0)
					{
						true_diagonals[i].side_l.push_back(*r);
						true_diagonals[i].dir_of_side = 0;
						true_diagonals[i].tail_end = (*r) -> start;
						true_diagonals[i].tail_area = triangle;
						guard = 1;
					}
					else
					{
						true_diagonals[i].next.push_back(next_diag);
						true_diagonals[i].dir_of_side = 1;
						true_diagonals[i].tail_end = next_diag -> tail_end;
						true_diagonals[i].tail_area =  triangle + next_diag -> tail_area;
					}
					next_diag = &true_diagonals[i];

					diagonals.erase(r);
					diagonals.erase(left);
					diagonals.insert(right, &true_diagonals[i++]);
					r = _prev(diagonals, right);

                    std::cout << count++ << std::endl;
				}
				else
                {
					if(next_diag == NULL)
                    {
                        (*r) -> rank = now_rank;
						buffer.push_back(*r);
                    }
					else
                    {           //здесь мы окажемся, когда не можем срезать ухо и уйти ни вправо, ни влево. тогда это - диагональ нового уровня
						for(std::list<Diag*>::iterator l = buffer.begin(); l != buffer.end(); l++)
						{
							if(tmp_diag.back() -> getE() == (*l) -> getS())
								tmp_diag.push_back(*l);
							else
                                throw (char *) "Error in constructing hierarchy.";
							if(ban_list.empty() == true || ban_list.front() != (*l) -> getS())
								ban_list.push_front((*l) -> getS());
							if(ban_list.empty() == true || ban_list.back() != (*l) -> getE())
								ban_list.push_front((*l) -> getE());
						}
						buffer.clear();
						
                        (*r) -> rank += 1;
						tmp_diag.push_back(*r);
						next_diag = NULL;

						if(ban_list.empty() == true || ban_list.front() != (*r) -> getS())
							ban_list.push_front((*r) -> getS());
						if(ban_list.empty() == true || ban_list.back() != (*r) -> getE())
							ban_list.push_front((*r) -> getE());

						guard = 0;
					}

					r = right;
				}
			}
		}

		if(diagonals.size() > 2)
		{
            now_rank += 1;
			ban_list.clear();
			next_diag = NULL;
			diagonals = tmp_diag;
			tmp_diag.clear();
		}
	}

    if(diagonals.size() != 2)
        throw (char *) "Error: not two diagonals on input.";

    diagonals.front() -> rank += 1;
    diagonals.back() -> rank += 1;  //подсвечиваем зеленым последние диагонали, в которые свернулся весь полигон




    return find_division(diagonals, area().frac_abs());
}


}
