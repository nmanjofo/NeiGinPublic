#pragma once

#if defined(_MSC_VER)

#if defined(_MSC_VER)
  #pragma warning( disable : 4251 ) // exporting STL derived classes
#endif

#if defined(NeiGin_STATIC)
  #define NEIGIN_EXPORT
#elif defined(NeiGin_EXPORTS)
  #define NEIGIN_EXPORT __declspec(dllexport)
#else
  #define NEIGIN_EXPORT __declspec(dllimport)
#endif

#endif
