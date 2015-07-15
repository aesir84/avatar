#pragma once


//
// Qt
//

#pragma warning(push)
#pragma warning(disable : 4127) // because of QVector

#include <QAbstractVideoSurface>
#include <QBuffer>
#include <QByteArray>
#include <QCamera>
#include <QCameraInfo>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDataStream>
#include <QDebug>
#include <QEvent>
#include <QExposeEvent>
#include <QGuiApplication>
#include <QHostAddress>
#include <QImage>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QScreen>
#include <QtGui\5.4.1\QtGui\qpa\qplatformnativeinterface.h>
#include <QThread>
#include <QTimer>
#include <QTransform>
#include <QUdpSocket>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVideoSurfaceFormat>
#include <QWindow>

#pragma warning(pop)


//
// C++
//

#include <array>
#include <atomic>
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

#include <OVR.h>
#include <OVR_CAPI_GL.h>
