/*
 * A library of string utility functions.
 */

#pragma once

#include <string>

#ifdef _WIN32
#  ifdef UTILITIES_MODULE_EXPORT
#    define UTILITIES_API __declspec(dllexport)
#  else
#    define UTILITIES_API __declspec(dllimport)
#  endif
#else
#  define UTILITIES_API
#endif

/*
 * Splits and joins the list of tokens into a string, separated by space characters.
 */
UTILITIES_API std::string split_and_join(const std::string & source);
