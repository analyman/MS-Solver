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

public:
	SMLineSegment(value_type dx,        value_type dy,  
                  value_type x_off = 0, value_type y_off = 0, 
                  value_type bold = 1, 
                  D_RGBColor<value_type> color = D_RGBColor<value_type>::black):
		 m_dx(dx), m_dy(dy), m_color(color), m_bold(bold), m_x_off(x_off), m_y_off(y_off){}

	void ScaleBy(value_type scale) { this->m_dx *= scale; this->m_dy *= scale; this->m_x_off *= scale; this->m_y_off *= scale; }

	static SMLineSegment CreateLineSegment(value_type dx, value_type dy, 
                                           SMNode<value_type>& origin, 
                                           D_RGBColor<value_type> color = D_RGBColor<value_type>::black)
	{
		SMLineSegment ret(dx, dy, origin, color);
		return ret;
	}
	static SMLineSegment CreateLineSegmentPolar(value_type theta, 
                                                value_type rho, 
                                                SMNode<value_type>& origin, 
                                                D_RGBColor<value_type> color = D_RGBColor<value_type>::black)
	{
		SMLineSegment ret(::cos(theta) * rho, ::sin(theta) * rho, origin, color);
		return ret;
	}
}; //}

#endif // PRESENTATION_LINE_SEGMENT_HPP
