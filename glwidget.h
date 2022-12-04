#pragma once

#include <QGLWidget>
#include <QKeyEvent>
#include <QMainWindow>
#include <QtOpenGL>
#include <QOpenGLWidget>
#include <cmath>
#include <objects.h>
using namespace std;

class GLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    virtual ~GLWidget();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void setProjection();
    void drawGrid(GLfloat *colour);
    void setupObjects();
    void drawObjects();
    void animate(int time);

public slots:
    void resetCamera();

protected:

    struct
    {
        QVector3D eye;
        QVector3D center;
        QVector3D up;
    } m_lookAt;

    struct
    {
        float zoom;
        float aspectRatio;
        float nearPlane;
        float farPlane;
    } m_perspective;

    struct
    {
        float zoomFactor;
        float extent;
        float nearPlane;
        float farPlane;
    } m_ortho;

    // Switch between orthographic and perspective projection
    bool m_orthoProjection = false;

    QVector2D m_mousePressPosition;
    QVector3D mRotationAxis;
    QQuaternion mRotation = QQuaternion();

    // Objects that will be drawn in your scene
    std::map<std::string, Object*> m_objects;

    // Storage for max 10 textures
    GLuint m_textures[20];

    // Flags (check key press function)
    bool m_showGrid;
    bool m_showWireFrame;
    bool m_runAnimation;

    // Animation parameters
    int m_time;
    int m_nframes; // the length of 1 animation loop

    // position / rotation values that change for each animation frame
    struct {
        float posn_x, posn_z, bodyTheta;
        float left_arm, right_arm;
        float left_leg, right_leg;
        float earthTheta;
    } m_animation;

    int viewWidth;
    int viewHeight;

public slots:
    void idle();
};
