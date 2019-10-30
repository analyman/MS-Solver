#ifndef PRESENTATION_LINE_SEGMENT_HPP
#define PRESENTATION_LINE_SEGMENT_HPP

#include "windows_helper.hpp"
#include "generic_entity.hpp"
#include "node.hpp"

#include <cmath>

template<typename T> class SMSupport;

template<typename T>
class SMLineSegment //{
{
public:
	typedef T value_type;
private:
	friend class SMSupport<value_type>;
	value_type             m_dx;
	value_type             m_dy;
	value_type             m_x_off;
	value_type             m_y_off;
	D_RGBColor<value_type> m_color;
	value_type             m_bold;
	bool                   m_is_ellipse;
	bool                   m_is_fill;

public:
	SMLineSegment(value_type dx_ellipsex,        value_type dy_ellipse_y,  
                  value_type x_off = 0, value_type y_off = 0, 
                  value_type bold = 1, 
                  D_RGBColor<value_type> color = D_RGBColor<value_type>::black, bool ellipse = false, bool fill = false):
	   	 m_dx(dx_ellipsex), m_dy(dy_ellipse_y), m_color(color), m_bold(bold), 
	    	m_x_off(x_off), m_y_off(y_off), m_is_ellipse(ellipse), m_is_fill(fill){}

	void ScaleBy(value_type scale) { this->m_dx *= scale; this->m_dy *= scale;  this->m_x_off *= scale; this->m_y_off *= scale; }
	void MoveXY(value_type x, value_type y) { if (m_is_ellipse == false) { this->m_dx += x; this->m_dy += y; } this->m_x_off += x; this->m_y_off += y; }

	bool& Fill() { return this->m_is_fill; }
	bool  IsEllipse() { return this->m_is_ellipse; }

	static SMLineSegment CreateLine(value_type dx, value_type dy, 
                                           value_type off_x = 0, value_type off_y = 0,
                                           value_type bold = 1,
                                           D_RGBColor<value_type> color = D_RGBColor<value_type>::black)
	{
		SMLineSegment ret(dx, dy, off_x, off_y, bold, color, false);
		return ret;
	}

	static SMLineSegment CreateEllipse(value_type rx, value_type ry = rx,
									   value_type off_x = 0, value_type off_y = 0,
									   value_type bold = 1,
									   D_RGBColor<value_type> color = D_RGBColor<value_type>::black)
	{
		SMLineSegment ret(rx, ry, off_x, off_y, bold, color, true);
		return ret;
	}

	static SMLineSegment CreateLinePolar(value_type theta, value_type rho, 
                                         value_type off_x = 0, value_type off_y = 0,
                                         value_type bold = 1,
                                         D_RGBColor<value_type> color = D_RGBColor<value_type>::black)
	{
		SMLineSegment ret(::cos(theta) * rho, ::sin(theta) * rho, off_x, off_y, bold, color, false);
		return ret;
	}
}; //}

#endif // PRESENTATION_LINE_SEGMENT_HPP
