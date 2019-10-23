#ifndef PRESENTATION_SUPPORT_HPP
#define PRESENTATION_SUPPORT_HPP

#include "windows_helper.hpp"
#include "generic_entity.hpp"
#include "node.hpp"
#include "line_segment.hpp"

#include <vector>

template<typename T>
class SMSupport: DrawableEntity //{
{
public:
	typedef T value_type;

private:
	SMNode<value_type>&                    m_node;
	std::vector<SMLineSegment<value_type>> m_lines;

	SMSupport(SMNode<value_type>& node): m_node(node), m_lines(){}

public:
	SMSupport() = delete;
	std::vector<SMLineSegment<value_type>>& LineSegment() { return this->m_lines; }

	static SMSupport CreateHingeSupport(SMNode<value_type>& node) {
		SMSupport ret(node);
		ret.m_lines.push_back(SMLineSegment<value_type>(0.3, -0.7, -0.3, -0.7, 1.5));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.3, -0.7, 0.0, 0.0, 1.5));
		ret.m_lines.push_back(SMLineSegment<value_type>(-0.3, -0.7, 0.0, 0.0, 1.5));
		for (auto bi = ret.m_lines.begin(); bi != ret.m_lines.end(); bi++)
			bi->ScaleBy(10);
		return ret;
	}

#ifdef __WINDOWS__
	bool draw_with(ID2D1Factory* factory, ID2D1RenderTarget* target) {
		if (target == nullptr || factory == nullptr) { return false; }
		ID2D1SolidColorBrush* brush = nullptr;
		for (auto bi = this->m_lines.begin(); bi != this->m_lines.end(); bi++) {
			if (!SUCCEEDED(target->CreateSolidColorBrush(bi->m_color, &brush))) return false;
			auto p1 = D2D1::Point2F(this->m_node.GetX() + bi->m_x_off, this->m_node.GetY() + bi->m_y_off);
			auto p2 = D2D1::Point2F(this->m_node.GetX() + bi->m_dx, this->m_node.GetY() + bi->m_dy);
			target->DrawLine(p1, p2, brush, bi->m_bold);
			SafeReleaseX(&brush);
		}
		return true;
	}
#endif // __WINDOWS__
}; //}

#endif // PRESENTATION_SUPPORT_HPP
