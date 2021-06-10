#ifndef __INCLUSION_SQLITEMP_H__
#define __INCLUSION_SQLITEMP_H__

#ifdef _MSC_VER

#if _MSC_VER < 1600							// [..., VS2010)

#ifndef __SQLITEMP_RVALUE_REFERENCE__
#define __SQLITEMP_RVALUE_REFERENCE__       0
#endif

#ifndef __SQLITEMP_USE_TR1_NAMESPACE__
#define __SQLITEMP_USE_TR1_NAMESPACE__      1
#endif

#else										// [VS2010, ...)

#ifndef __SQLITEMP_RVALUE_REFERENCE__
#define __SQLITEMP_RVALUE_REFERENCE__       1
#endif

#ifndef __SQLITEMP_USE_TR1_NAMESPACE__
#define __SQLITEMP_USE_TR1_NAMESPACE__      0
#endif

#endif

#else										// !Visual Studio

#ifndef __SQLITEMP_RVALUE_REFERENCE__
#define __SQLITEMP_RVALUE_REFERENCE__       1
#endif

#ifndef __SQLITEMP_USE_TR1_NAMESPACE__
#define __SQLITEMP_USE_TR1_NAMESPACE__      0
#endif

#endif

#include "lib/sqlite3.h"

#include <memory>
#include <string>
#include <algorithm>

#endif
