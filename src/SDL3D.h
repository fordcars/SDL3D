#ifndef SDL3D_H_
#define SDL3D_H_

/* Include windows.h properly on Windows */
#if defined(WIN32) || defined(_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif

#endif /* SDL3D_H_ */