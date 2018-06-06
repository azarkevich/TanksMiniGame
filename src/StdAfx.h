// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
#include <deque>

using namespace std;

#include <cstdio>
#include <cstdlib>

#include "SDL/include/SDL.h"

#include <math.h>

#include <sys/stat.h>
#include <sys/types.h>

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }
