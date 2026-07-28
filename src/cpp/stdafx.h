#include <QtSystemDetection>

// Standard C-Library includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

// Windows libraries

#ifdef Q_OS_WIN
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif // Q_OS_WIN

// POSIX libraries

#if Q_OS_UNIX
#include <unistd.h>
#endif // Q_OS_UNIX

// Android libraries

#ifdef Q_OS_ANDROID
#include <jni.h>
#endif // Q_OS_ANDROID

// Start Objective-C imports

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#endif // __OBJC__


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
#include <type_traits>
#include <utility>

// QT Core
#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariant>
#include <QVector>

// QT GUI
#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPainter>
#include <QStyle>
#include <QWidget>

// Platform specific QT modules


#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif // Q_OS_ANDROID

#endif // __cplusplus