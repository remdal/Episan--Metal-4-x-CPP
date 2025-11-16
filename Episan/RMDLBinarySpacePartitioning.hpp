/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLBinarySpacePartitioning.hpp   +++     	**/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 05/03/2025 15:20:18      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef BINARY_SPACE_PARTITIONING_HPP
# define BINARY_SPACE_PARTITIONING_HPP

# include <iostream>
# include <cmath>

class   Fixed {
public:
    Fixed();
    Fixed(const int n);
    Fixed(const float n);
    Fixed(const Fixed &src);
    Fixed& operator= (const Fixed &original);
    ~Fixed();

    int     getRawBits() const;
    void    setRawBits(int const raw);

    float   toFloat() const;
    int     toInt() const;

    Fixed   abs(Fixed x);

    bool operator> (const Fixed &rhs) const;
    bool operator< (const Fixed &rhs) const;
    bool operator>= (const Fixed &rhs) const;
    bool operator<= (const Fixed &rhs) const;
    bool operator== (const Fixed &rhs) const;
    bool operator!= (const Fixed &rhs) const;

    Fixed operator+ (const Fixed &rhs) const;
    Fixed operator- (const Fixed &rhs) const;
    Fixed operator* (const Fixed &rhs) const;
    Fixed operator/ (const Fixed &rhs) const;

    Fixed &operator++ ();
    Fixed operator++ (int);
    Fixed &operator-- ();
    Fixed operator-- (int);

    static Fixed& min(Fixed& a, Fixed& b);
    static const Fixed& min(const Fixed& a, const Fixed& b);
    static Fixed& max(Fixed& a, Fixed& b);
    static const Fixed& max(const Fixed& a, const Fixed& b);

private:
    static const int    _fractionalBits = 8; // 14 is better 113
    int                 _fixedPointValue;
};

std::ostream& operator<< ( std::ostream &out, const Fixed &in );

class   Point {
public:
    Point();
    Point(const Point &src);
    Point(const float x, const float y);
    Point& operator= (const Point &original);
    ~Point();
    
    Fixed   getX(void) const;
    Fixed   getY() const;

    Fixed   area(Point const a, Point const b, Point const c);
    
    bool    bsp(Point const a, Point const b, Point const c, Point const point);

private:
    const Fixed _x;
    const Fixed _y;
};

#endif /* BSP */