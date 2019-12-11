#pragma once

#pragma once

#if defined(_MSC_VER)

#pragma warning( disable : 4251 ) // exporting STL classes

#if defined(NeiVu_STATIC)
#define NEIVU_EXPORT
#elif defined(NeiVu_EXPORTS)
#define NEIVU_EXPORT __declspec(dllexport)
#else
#define NEIVU_EXPORT __declspec(dllimport)
#endif

#endif



