//
// Created by wx on 18-9-8.
//

#ifndef CXXPERFCOUNTER_UTIL_H
#define CXXPERFCOUNTER_UTIL_H

#include <string>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#if defined(WIN32) || defined(WIN64)

#ifndef ACCESS
#define ACCESS(a) _access((a), 0)
#define MKDIR(a) _mkdir((a))
#endif
#else
#include <sys/stat.h>
#ifndef ACCESS
#define ACCESS(a) access((a),F_OK)
#define MKDIR(a) mkdir((a),0755)
#endif
#endif
#ifndef ASSERT
#include <assert.h>
#define ASSERT(x) assert(x)
#endif

#ifndef DISALLOW_COPY_AND_ASSIGN
#define    DISALLOW_COPY_AND_ASSIGN(T) \
    T(const T&);                \
    T& operator=(const T&)
#endif




#endif //CXXPERFCOUNTER_UTIL_H
