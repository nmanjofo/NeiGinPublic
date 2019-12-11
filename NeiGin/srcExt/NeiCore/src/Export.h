#pragma once

#pragma once

#if defined(_MSC_VER)

#pragma warning( disable : 4251 ) // exporting STL classes
#pragma warning( disable : 4275 ) // non-exported base class - Object

#if defined(NeiCore_STATIC)
#define NEICORE_EXPORT
#elif defined(NeiCore_EXPORTS)
#define NEICORE_EXPORT __declspec(dllexport)
#else
#define NEICORE_EXPORT __declspec(dllimport)
#endif

#endif



