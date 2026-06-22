
// Standard C-Library includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

// Windows libraries
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif // _WIN32

// POSIX libraries
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h>
#endif

// Start C++ includes

#ifdef __cplusplus

// STL containers
#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// STL Utilities
#include <algorithm>
#include <functional>
#include <memory>
#include <utility>

// QT Core
#include <QList>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>

// QT GUI
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#endif // __cplusplus