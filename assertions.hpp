#pragma once
#include <stdexcept>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define LINE_STR TOSTRING(__LINE__)

#ifndef NDEBUG
#define ASSERT_NOT_REACHED() throw std::logic_error("Unexpected control flow at " __FILE__ ":" LINE_STR)
#else
#define ASSERT_NOT_REACHED() 0;
#endif 

