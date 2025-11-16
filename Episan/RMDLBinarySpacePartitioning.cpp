/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLBinarySpacePartitioning.cpp        +++	**/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 05/03/2025 15:20:19      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "RMDLBinarySpacePartitioning.hpp"

Fixed::Fixed() : _fixedPointValue(0)
{
}

Fixed::Fixed(const int n) : _fixedPointValue(n <<  _fractionalBits)
{
}

Fixed::Fixed(const float n) : _fixedPointValue( std::roundf(n * (1 << _fractionalBits)) )
{
}

Fixed::Fixed(const Fixed &src)
{
    *this = src;
}

Fixed& Fixed::operator = (const Fixed &original)
{
    if (this != &original)
        this->_fixedPointValue = original.getRawBits();
    return (*this);
}

Fixed::~Fixed()
{
}

int     Fixed::getRawBits() const
{
    return (this->_fixedPointValue);
}

void    Fixed::setRawBits(int const raw)
{
    this->_fixedPointValue = raw;
}

float   Fixed::toFloat() const
{
    // (static_cast<float>(this->getRawBits()) / ( 1 << _fractionalBits ));
    return ((float)this->_fixedPointValue / (1 << _fractionalBits));
}

int     Fixed::toInt() const
{
    return (this->_fixedPointValue >> _fractionalBits);
}

bool    Fixed::operator > (const Fixed &rhs) const
{
    return (this->getRawBits() > rhs.getRawBits());
}

bool    Fixed::operator < (const Fixed &rhs) const
{
    return (this->getRawBits() < rhs.getRawBits());
}

bool    Fixed::operator >= (const Fixed &rhs) const
{
    return (this->getRawBits() >= rhs.getRawBits());
}

bool    Fixed::operator <= (const Fixed &rhs) const
{
    return (this->getRawBits() <= rhs.getRawBits());
}

bool    Fixed::operator == (const Fixed &rhs) const
{
    return (this->getRawBits() == rhs.getRawBits());
}

bool    Fixed::operator != (const Fixed &rhs) const
{
    return (this->getRawBits() != rhs.getRawBits());
}

Fixed   Fixed::operator + (const Fixed &rhs) const
{
    return (Fixed(this->toFloat() + rhs.toFloat()));
}

Fixed   Fixed::operator - (const Fixed &rhs) const
{
    return (Fixed(this->toFloat() - rhs.toFloat()));
}

Fixed   Fixed::operator * (const Fixed &rhs) const
{
    return (Fixed(this->toFloat() * rhs.toFloat()));
}

Fixed   Fixed::operator / (const Fixed &rhs) const
{
    return (Fixed(this->toFloat() / rhs.toFloat()));
}

Fixed   &Fixed::operator ++ ()
{
    this->_fixedPointValue++;
    return (*this);
}

Fixed   Fixed::operator ++ (int)
{
    Fixed temp = *this;
    ++(*this);
    return (temp);
}

Fixed   &Fixed::operator -- ()
{
    --this->_fixedPointValue;
    return (*this);
}

Fixed   Fixed::operator -- (int)
{
    Fixed temp(*this);
    temp._fixedPointValue = this->_fixedPointValue--;
    return (temp);
}

Fixed&  Fixed::min(Fixed &a, Fixed &b)
{
    return (a < b ? a : b);
}

const Fixed& Fixed::min(const Fixed & a, const Fixed &b)
{
    return (a < b ? a : b);
}

Fixed   &Fixed::max(Fixed &a, Fixed &b)
{
    if (a.getRawBits() > b.getRawBits())
        return (a);
    return (b);
}

const Fixed &Fixed::max(const Fixed &a, const Fixed &b)
{
    if (a.getRawBits() > b.getRawBits())
        return (a);
    return (b);
}

std::ostream& operator << ( std::ostream &out, Fixed const &in)
{
    out << in.toFloat();
    return (out);
}

Point::Point() : _x(0), _y(0)
{
}

Point::Point(const float x, const float y) : _x(x), _y(y)
{
}

Point::Point(const Point &src) : _x(src._x), _y(src._y)
{
}

Point &Point::operator = (const Point &original)
{
    if (this != &original)
    {
        (Fixed) this->_x = original.getX();
        (Fixed)this->_y = original.getY();
    }
    return (*this);
}

Point::~Point()
{
}

Fixed   Point::getX(void) const
{
    return (this->_x);
}

Fixed   Point::getY() const
{
    return (this->_y);
}

Fixed   abs(Fixed x)
{
    if (x < 0)
        return (x * -1);
    return (x);
}

Fixed   area(Point const a, Point const b, Point const c)
{
    return (((a.getX() * (b.getY() - c.getY())) +
             (b.getX() * (c.getY() - a.getY())) +
             (c.getX() * (a.getY() - b.getY()))) / 2);
}

bool bsp(Point const a, Point const b, Point const c, Point const point)
{
    Fixed   abcArea = abs(area(a, b, c));
    Fixed   pabArea = abs(area(point, a, b));
    Fixed   pbcArea = abs(area(point, b, c));
    Fixed   pcaArea = abs(area(point, c, a));
    std::cout << "abcArea: " << abcArea << ", pabArea: " << pabArea
              << ", pbcArea: " << pbcArea << ", pcaArea: " << pcaArea << std::endl << std::endl;

    return (abcArea == pabArea + pbcArea + pcaArea);
}

/*int main(void)
{
    Fixed a;
    std::cout << a << std::endl;
    std::cout << ++a << std::endl; // précision en 8 bits 0.00390625 / en 4 bits 0.0625

    Fixed const b(Fixed(5.05f) * Fixed(2));
    std::cout << "\033[31mb is 5,05 x 2 = " << b << "\033[0m" << std::endl; // 14 bits 6.10352e-05
    
    if (bsp(Point(-0, -0), Point(-10, -30), Point(-20, -0), Point(-10.66666666, -1.5)) == true )
    {
        std::cout << "Point3 is in the triangle" << std::endl;
    } else {
        std::cout << "Point3 is not in the triangle" << std::endl;
    }
    Fixed c(Fixed(5.05f) + Fixed(5.1f));
    std::cout << ++c << std::endl;
    std::cout << "\033[31mc préincr' is " << c << "\033[0m" << std::endl;

    Fixed d(Fixed(15.3f) - Fixed(5.1f));
    std::cout << d << std::endl;
    std::cout << ++d << std::endl;
    std::cout << d << '\n' << std::endl;

    std::cout << "\033[33m" << Fixed::max(a, b) << "\033[0m\n" << std::endl;
    std::cout << "\033[33m" << Fixed::min(a, b) << "\033[0m\n" << std::endl;
    std::cout << "\033[33m" << Fixed::max(c, b) << "\033[0m\n" << std::endl;
    std::cout << "\033[33m" << Fixed::max(a, c) << "\033[0m\n" << std::endl;

    Point aaaa(0, 0);
    Point bbbb(10, 0);
    Point cccc(5, 10);
    Point p1(5, 5);   // int
    Point p2(10, 5);  // ext
    std::cout << "\033[32mPoint p1 (5, 5) is inside: " << bsp(aaaa, bbbb, cccc, p1) << "\033[0m" << std::endl;
    std::cout << "\033[32mPoint p2 (10, 5) is outside: " << bsp(aaaa, bbbb, cccc, p2) << "\033[0m" << std::endl;

    // True, Thus, if the point is a vertex or on edge, it will return False
    if (bsp(Point(0, 0), Point(10, 30), Point(20, 0), Point(30, 15)) == true )
    {
        std::cout << "Point1 is in the triangle" << std::endl;
    } else {
        std::cout << "Point1 is not in the triangle" << std::endl;
    }

    if (bsp(Point(0, 0), Point(10, 30), Point(20, 0), Point(10, 15)) == true )
    {
        std::cout << "Point2 is in the triangle" << std::endl;
    } else {
        std::cout << "Point2 is not in the triangle" << std::endl;
    }

    return (0);
}*/
/*
Input: A = (0, 0), B = (10, 30), C = (20, 0), P(10, 15)
Output: Inside
Explanation:
              B(10,30)
                / \
               /   \
              /     \
             /   P   \      P'
            /         \
     A(0,0) ----------- C(20,0)

Input: A = (0, 0), B = (10, 30), C = (20, 0), P(30, 15)
Output: Outside
Explanation:
              B(10,30)
                / \
               /   \
              /     \
             /       \      P
            /         \
     A(0,0) ----------- C(20,0)

Solution:
Let the coordinates of three corners be (x1, y1), (x2, y2) and (x3, y3). And coordinates of the given point P be (x, y)

Calculate area of the given triangle, i.e., area of the triangle ABC in the above diagram.
Area A = [ x1(y2 – y3) + x2(y3 – y1) + x3(y1-y2) ] / 2
Calculate area of the triangle PAB. We can use the same formula for this. Let this area be A1.
Calculate area of the triangle PBC. Let this area be A2.
Calculate area of the triangle PAC. Let this area be A3.
If P lies inside the triangle, then A1 + A2 + A3 must be equal to A.    */
