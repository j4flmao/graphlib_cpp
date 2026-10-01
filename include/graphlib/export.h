#ifndef GRAPHLIB_EXPORT_H
#define GRAPHLIB_EXPORT_H

#if defined(_WIN32) && defined(GRAPHLIB_STATIC)
#define GRAPHLIB_API
#elif defined(_WIN32)
#ifdef GRAPHLIB_EXPORTS
#define GRAPHLIB_API __declspec(dllexport)
#else
#define GRAPHLIB_API __declspec(dllimport)
#endif
#else
#define GRAPHLIB_API __attribute__((visibility("default")))
#endif

#endif

