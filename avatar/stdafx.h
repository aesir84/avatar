#pragma once


//
// Disable silly warnings
//

#pragma warning(disable : 4127) // conditional expression is constant


//
// Qt
//

#include <QAbstractVideoSurface>
#include <QBuffer>
#include <QByteArray>
#include <QCamera>
#include <QCameraInfo>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDataStream>
#include <QDebug>
#include <qglobal.h> // there is no QGlobal file in Qt 5.4.1 installation directory
#include <QEvent>
#include <QExposeEvent>
#include <QGuiApplication>
#include <QHostAddress>
#include <QImage>
#include <qmath.h> // there is no QMath file in Qt 5.4.1 installation directory
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QScreen>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QTimer>
#include <QTransform>
#include <QUdpSocket>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVideoSurfaceFormat>
#include <QWindow>


//
// C++
//

#include <array>
#include <atomic>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>


//
// OVR
//

#include <OVR_CAPI_GL.h>
#include "Extras\OVR_Math.h"
