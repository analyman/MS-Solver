#ifndef PRESENTATION_SUPPORT_HPP
#define PRESENTATION_SUPPORT_HPP

#include "windows_helper.hpp"
#include "generic_entity.hpp"
#include "node.hpp"
#include "line_segment.hpp"

#include <wrl.h>

#include <vector>

template<typename T>
class SMSupport: DrawableEntity //{
{
public:
	typedef T value_type;

private:
	SMNode<value_type>&                    m_node;
	std::vector<SMLineSegment<value_type>> m_lines;
	value_type                             m_angle;

	SMSupport(SMNode<value_type>& node, value_type angle = 0): m_node(node), m_lines(), m_angle(angle){}

public:
	SMSupport() = delete;
	std::vector<SMLineSegment<value_type>>& LineSegment() { return this->m_lines; }
	value_type& Angle() { return this->m_angle; }

	static SMSupport CreateHingeSupport(SMNode<value_type>& node) {
		SMSupport ret(node);
		ret.m_lines.push_back(SMLineSegment<value_type>(0.43, -0.7, -0.43, -0.7, 2.0, D_RGBColor<value_type>::red));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.3, -0.7, 0.0, 0.0, 1.5, D_RGBColor<value_type>::red));
		ret.m_lines.push_back(SMLineSegment<value_type>(-0.3, -0.7, 0.0, 0.0, 1.5, D_RGBColor<value_type>::red));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.07, 0.07, 0.00, 0.0, 2.0, D_RGBColor<value_type>::black, true, false));
		SMLineSegment<value_type> _linex(-0.37, -0.7, -0.43, -0.78, 0.5, D_RGBColor<value_type>::black);
		for (int i = 13; i >= 0; i--) {
			ret.m_lines.push_back(_linex);
			_linex.MoveXY(0.06, 0);
		}
		for (auto bi = ret.m_lines.begin(); bi != ret.m_lines.end(); bi++)
			bi->ScaleBy(50);
		return ret;
	}
	static SMSupport CreateRollSupport(SMNode<value_type>& node) {
		SMSupport ret(node);
		ret.m_lines.push_back(SMLineSegment<value_type>(0.43, -0.7, -0.43, -0.7, 2.0, D_RGBColor<value_type>::red));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.00, -0.56, 0.0, -0.07, 2.0, D_RGBColor<value_type>::red));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.07, 0.07, 0.00, 0.0, 2.0, D_RGBColor<value_type>::black, true, false));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.07, 0.07, 0.00, -0.63, 2.0, D_RGBColor<value_type>::black, true, false));
		SMLineSegment<value_type> _linex(-0.37, -0.7, -0.43, -0.78, 0.5, D_RGBColor<value_type>::black);
		for (int i = 13; i >= 0; i--) {
			ret.m_lines.push_back(_linex);
			_linex.MoveXY(0.06, 0);
		}
		for (auto bi = ret.m_lines.begin(); bi != ret.m_lines.end(); bi++)
			bi->ScaleBy(50);
		return ret;
	}
	static SMSupport CreateFixedSupport(SMNode<value_type>& node) {
		SMSupport ret(node);
		ret.m_lines.push_back(SMLineSegment<value_type>(0.0, -0.7, 0.0, 0.0, 1.5, D_RGBColor<value_type>::red));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.43, -0.7, -0.43, -0.7, 2.0, D_RGBColor<value_type>::red));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.06, 0.06, 0.00, 0.0, 2.0, D_RGBColor<value_type>::black, true, true));
		SMLineSegment<value_type> _linex(-0.37, -0.7, -0.43, -0.78, 0.5, D_RGBColor<value_type>::black);
		for (int i = 13; i >= 0; i--) {
			ret.m_lines.push_back(_linex);
			_linex.MoveXY(0.06, 0);
		}
		for (auto bi = ret.m_lines.begin(); bi != ret.m_lines.end(); bi++)
			bi->ScaleBy(50);
		return ret;
	}
	static SMSupport CreateDirSupport(SMNode<value_type>& node) {
		SMSupport ret(node);
		ret.m_lines.push_back(SMLineSegment<value_type>(0.43, 0.0, -0.43, 0.0, 1.5, D_RGBColor<value_type>::red));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.43, -0.7, -0.43, -0.7, 1.5, D_RGBColor<value_type>::red));

		ret.m_lines.push_back(SMLineSegment<value_type>(0.06, 0.06, 0.22, -0.06, 1.0, D_RGBColor<value_type>::black, true, false));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.06, 0.06, -0.22, -0.06, 1.0, D_RGBColor<value_type>::black, true, false));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.06, 0.06, 0.22, -0.64, 1.0, D_RGBColor<value_type>::black, true, false));
		ret.m_lines.push_back(SMLineSegment<value_type>(0.06, 0.06, -0.22, -0.64, 1.0, D_RGBColor<value_type>::black, true, false));

		ret.m_lines.push_back(SMLineSegment<value_type>(0.22, -0.12, 0.22, -0.58, 1.5, D_RGBColor<value_type>::red));
		ret.m_lines.push_back(SMLineSegment<value_type>(-0.22, -0.12, -0.22, -0.58, 1.5, D_RGBColor<value_type>::red));
		SMLineSegment<value_type> _linex(-0.37, -0.7, -0.43, -0.78, 0.5, D_RGBColor<value_type>::black);
		for (int i = 13; i >= 0; i--) {
			ret.m_lines.push_back(_linex);
			_linex.MoveXY(0.06, 0);
		}
		for (auto bi = ret.m_lines.begin(); bi != ret.m_lines.end(); bi++)
			bi->ScaleBy(50);
		return ret;
	}

#ifdef __WINDOWS__
	bool draw_with(ID2D1Factory* factory, ID2D1RenderTarget* target) {
		if (target == nullptr || factory == nullptr) { return false; }
		ID2D1SolidColorBrush* brush = nullptr;
		ID2D1Layer* support_layer = nullptr;
		DX::ThrowIfFailed(target->CreateLayer(NULL, &support_layer));
		target->PushLayer(D2D1::LayerParameters(
			D2D1::InfiniteRect(),
			NULL,
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
			D2D1::IdentityMatrix(),
			0.8
		), support_layer);
		D2D1::Matrix3x2F origin_transform;
		target->GetTransform(&origin_transform);
		target->SetTransform(D2D1::Matrix3x2F::Rotation(this->m_angle, this->m_node) * origin_transform);
		for (auto bi = this->m_lines.begin(); bi != this->m_lines.end(); bi++) {
			if (!SUCCEEDED(target->CreateSolidColorBrush(bi->m_color, &brush))) return false;
			auto p1 = D2D1::Point2F(this->m_node.GetX() + bi->m_x_off, this->m_node.GetY() + bi->m_y_off);
			if (!bi->IsEllipse()) {
				auto p2 = D2D1::Point2F(this->m_node.GetX() + bi->m_dx, this->m_node.GetY() + bi->m_dy);
				target->DrawLine(p1, p2, brush, bi->m_bold);
			}
			else {
				D2D1_ELLIPSE e = D2D1::Ellipse(p1, bi->m_dx, bi->m_dy);
				if (bi->Fill()) {
					target->FillEllipse(e, brush);
				}
				else {
					target->DrawEllipse(e, brush);
				}
			}
			SafeReleaseX(&brush);
		}
		Microsoft::WRL::ComPtr<ID2D1PathGeometry> cp_pathgeo;
		target->PopLayer();
		target->SetTransform(origin_transform);
		return true;
	}
#endif // __WINDOWS__
}; //}

#endif // PRESENTATION_SUPPORT_HPP
