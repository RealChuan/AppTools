#ifndef CONTROLS_GLOBAL_H
#define CONTROLS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CONTROLS_LIBRARY)
#  define CONTROLS_EXPORT Q_DECL_EXPORT
#else
#  define CONTROLS_EXPORT Q_DECL_IMPORT
#endif

#endif // CONTROLS_GLOBAL_H
