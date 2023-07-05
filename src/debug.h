#pragma once


#include <iostream>

#ifndef NDEBUG
#define DEBUG_MSG(msg) do{ std::cerr << __FILE__ << "(@" << __LINE__ << "): " << msg << '\n'; exit(1); } while( false )
#else
#define DEBUG_MSG(msg) do{ } while ( false )
#endif
