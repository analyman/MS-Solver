#ifndef PRESENTATION_NODE_HPP
#define PRESENTATION_NODE_HPP

#include "windows_helper.hpp"
#include "generic_entity.hpp"

template<typename T>
class SMNode: DrawableEntity //{
{
public:
	typedef T      value_type;
	typedef size_t node_id;
private:
#ifdef __WINDOWS__
	typedef decltype(D2D1::Point2F()) d2d1point2f;
#endif // __WINDOWS__
	value_type m_x;
	value_type m_y;
    node_id    m_id;
public:
	SMNode<value_type>(value_type x, value_type y) : m_x(x), m_y(y) {}

	value_type& GetX() { return m_x; }
	value_type& GetY() { return m_y; }
    node_id&    GetID(){return this->m_id;}

#ifdef __WINDOWS__
	bool draw_with(ID2D1Factory* factory, ID2D1RenderTarget* target) {
		if (target == nullptr || factory == nullptr) { return false; }
		auto ellipse = D2D1::Ellipse(D2D1::Point2F(m_x, m_y), 2.0, 2.0);
		ID2D1EllipseGeometry* ellipseGeo = nullptr;
		ID2D1SolidColorBrush* brush = nullptr;
		if (!SUCCEEDED(target->CreateSolidColorBrush(D2D1::ColorF(0.2, 0.2, 0.2), &brush))) return false;
		if (!SUCCEEDED(factory->CreateEllipseGeometry(ellipse, &ellipseGeo))) { SafeReleaseX(&brush); return false; }
		target->DrawGeometry(ellipseGeo, brush);
		SafeReleaseX(&ellipseGeo);
		SafeReleaseX(&brush);
	}
#endif // __WINDOWS__
#ifdef __WINDOWS__
	operator d2d1point2f() const
	{
		return D2D1::Point2F(this->m_x, this->m_y);
	}
#endif // __WINDOWS__
}; //}

#endif // PRESENTATION_NODE_HPP
