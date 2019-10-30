#ifndef WINDOWS_HELPER_HEPP
#define WINDOWS_HELPER_HEPP

#if defined(_WIN32) || defined (_WIN64)
#ifndef __WINDOWS__
#define __WINDOWS__
#endif // __WINDOWS__
#endif // _WIN32 || _WIN64

#ifdef __WINDOWS__
#include "d2d1utils.hpp"
#include <Windows.h>
#endif // __WINDOWS__

#include<cstdlib>

template<typename T>
inline void SafeReleaseX(T** ptr) {
	if (*ptr) {
		(*ptr)->Release();
		*ptr = NULL;
	}
}

namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		virtual const char* what() const override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X",
				static_cast<unsigned int>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}
}

#endif // WINDOWS_HELPER_HPP
