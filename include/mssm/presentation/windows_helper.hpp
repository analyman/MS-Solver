#ifndef WINDOWS_HELPER_HEPP
#define WINDOWS_HELPER_HEPP

#if defined(_WIN32) || defined (_WIN64)
#ifndef __WINDOWS__
#define __WINDOWS__
#endif // __WINDOWS__
#endif // _WIN32 || _WIN64

#include<cstdlib>

template<typename T>
inline void SafeReleaseX(T** ptr) {
	if (*ptr) {
		(*ptr)->Release();
		*ptr = NULL;
	}
}

#endif // WINDOWS_HELPER_HPP
