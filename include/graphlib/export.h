#ifndef GRAPHLIB_EXPORT_H
#define GRAPHLIB_EXPORT_H

#ifdef _WIN32
#ifdef GRAPHLIB_EXPORTS
#define GRAPHLIB_API __declspec(dllexport)
#else
#define GRAPHLIB_API __declspec(dllimport)
#endif
#else
#define GRAPHLIB_API __attribute__((visibility("default")))
#endif

#endif

