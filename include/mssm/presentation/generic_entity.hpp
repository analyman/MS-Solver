#ifndef GENERIC_ENTITY_HPP
#define GENERIC_ENTITY_HPP

#include "windows_helper.hpp"

class DrawableEntity //{
{
public:
#ifdef __WINDOWS__
	virtual bool draw_with(ID2D1Factory* factory, ID2D1RenderTarget* target) = 0;
#endif // __WINDOWS__
}; //}

template<typename T>
class D_RGBColor //{
{
public:
	typedef T value_type;

private:
#ifdef __WINDOWS__
	typedef D2D1::ColorF d2d1color;
#endif // __WINDOWS__
	value_type m_a;
	value_type m_r;
	value_type m_g;
	value_type m_b;

public:
	constexpr D_RGBColor(value_type r, value_type g, value_type b, value_type a = 1) :
		m_a(a), m_r(r), m_g(g), m_b(b) {}
	inline value_type& GetAlpha() { return this->m_a; }
	inline value_type& GetR() { return this->m_r; }
	inline value_type& GetG() { return this->m_g; }
	inline value_type& GetB() { return this->m_b; }

	static D_RGBColor red, green, blue, white, black;

#ifdef __WINDOWS__
	operator d2d1color() {
		d2d1color ret = d2d1color(m_r, m_g, m_b, m_a);
		return ret;
	}
#endif // __WINDOWS__
}; //}
template<typename T>
D_RGBColor<T> D_RGBColor<T>::red   = D_RGBColor(255, 0, 0, 1);
template<typename T>
D_RGBColor<T> D_RGBColor<T>::green = D_RGBColor(0, 255, 0, 1);
template<typename T>
D_RGBColor<T> D_RGBColor<T>::blue  = D_RGBColor(0, 0, 255, 1);
template<typename T>
D_RGBColor<T> D_RGBColor<T>::white = D_RGBColor(255, 255, 255, 1);
template<typename T>
D_RGBColor<T> D_RGBColor<T>::black = D_RGBColor(0, 0, 0, 1);

#endif // GENERIC_ENTITY_HPP
