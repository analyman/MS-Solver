#ifndef PRESENTATION_BEAM_HPP
#define PRESENTATION_BEAM_HPP

#include "windows_helper.hpp"
#include "generic_entity.hpp"
#include "node.hpp"

template<typename T>
class SMBeam: DrawableEntity //{
{
public:
	typedef T      value_type;
    typedef size_t beam_id;

private:
	SMNode<value_type>&    m_start;
	SMNode<value_type>&    m_end;
	D_RGBColor<value_type> m_color;
    beam_id                m_id;

public:
	SMBeam(SMNode<value_type>& s, SMNode<value_type>& e, 
           D_RGBColor<value_type> color = D_RGBColor<value_type>::blue):
		m_start(s), m_end(e), m_color(color) {}

    beam_id& GetID(){return this->m_id;}

#ifdef __WINDOWS__
	bool draw_with(ID2D1Factory* factory, ID2D1RenderTarget* target) {
		if (target == nullptr || factory == nullptr) { return false; }
		ID2D1SolidColorBrush* brush = nullptr;
		if (!SUCCEEDED(target->CreateSolidColorBrush(this->m_color, &brush))) return false;
		target->DrawLine(this->m_start, this->m_end, brush, 2.0f);
		SafeReleaseX(&brush);
		return true;
	}
#endif // __WINDOWS__
}; //}

#endif // PRESENTATION_BEAM_HPP
