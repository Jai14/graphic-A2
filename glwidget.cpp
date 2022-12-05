#include "glwidget.h"
#include <QDebug>
#include <iostream>

static float PI = 3.141592f;

static GLfloat light_position[] = {0.0, 100.0, 100.0, 0.0};

// basic colours
static GLfloat black[] = {0.0, 0.0, 0.0, 1.0};
static GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
static GLfloat grey[] = {0.5, 0.5, 0.5, 1.0};

// primary colours
static GLfloat red[] = {1.0, 0.0, 0.0, 1.0};
static GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
static GLfloat blue[] = {0.0, 0.0, 1.0, 1.0};

// secondary colours
static GLfloat yellow[] = {1.0, 1.0, 0.0, 1.0};
static GLfloat magenta[] = {1.0, 0.0, 1.0, 1.0};
static GLfloat cyan[] = {0.0, 1.0, 1.0, 1.0};

// other colours
static GLfloat orange[] = {1.0, 0.5, 0.0, 1.0};
static GLfloat brown[] = {0.5, 0.25, 0.0, 1.0};
static GLfloat dkgreen[] = {0.0, 0.5, 0.0, 1.0};
static GLfloat pink[] = {1.0, 0.6f, 0.6f, 1.0};


GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);

    m_lookAt.eye =    {+0.0f, +5.0f, 15.0f};
    m_lookAt.center =  {+0.0f, +0.0f, -1.0f};
    m_lookAt.up =     {+0.0f, +1.0f, +0.0f};

    m_showGrid = false;
    m_showWireFrame = false;
    m_runAnimation = false;

    viewWidth = this->width();
    viewHeight = this->height();

    m_time = 0;
    m_nframes = 600;

    m_objects = std::map<std::string, Object*>();

    // Timer for animation
    QTimer *timer = new QTimer(this);
    timer->start(0);
    connect(timer, SIGNAL(timeout()), this, SLOT(idle()));

    // Set frame 0 of the animation
    animate(0);
}

void GLWidget::initializeGL() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position); // specify the position of the light
    glEnable(GL_LIGHT0);       // switch light #0 on
    glEnable(GL_LIGHTING);     // switch lighting on
    glEnable(GL_DEPTH_TEST);   // make sure depth buffer is switched on
    glEnable(GL_NORMALIZE);    // normalize normal vectors for safety

    // Create storage for 10 textures - use those slots in turn
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glGenTextures(10, m_textures);
    glMatrixMode(GL_MODELVIEW);

    setupObjects();
}

void GLWidget::setProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (m_orthoProjection){
        double aspectRatio = static_cast<double>(viewWidth) / static_cast<float>(viewHeight);
        if (aspectRatio > 1.0) {
            glOrtho(static_cast<GLdouble>(-m_ortho.extent),
                    static_cast<GLdouble>(m_ortho.extent),
                    static_cast<GLdouble>(-m_ortho.extent) / aspectRatio,
                    static_cast<GLdouble>(m_ortho.extent) / aspectRatio, m_ortho.nearPlane, m_ortho.farPlane*5);
        } else {
            glOrtho(static_cast<GLdouble>(-m_ortho.extent) * aspectRatio,
                    static_cast<GLdouble>(m_ortho.extent) * aspectRatio,
                    static_cast<GLdouble>(-m_ortho.extent),
                    static_cast<GLdouble>(m_ortho.extent), m_ortho.nearPlane, m_ortho.farPlane*5);
        }
    } else {
        QMatrix4x4 projectionMat;
        projectionMat.setToIdentity();
        projectionMat.perspective(m_perspective.zoom, m_perspective.aspectRatio,
                                  m_perspective.nearPlane, m_perspective.farPlane*5);
        glLoadMatrixf(projectionMat.constData());
    }
}

void GLWidget::paintGL() {
    // clear all pixels
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setProjection();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    QMatrix4x4 viewMat;
    viewMat.setToIdentity();
    viewMat.lookAt(m_lookAt.eye,
                   m_lookAt.eye+m_lookAt.center,
                   m_lookAt.up);
    glLoadMatrixf(viewMat.constData());


    if (m_showGrid) {
        drawGrid(magenta);
    }

    if (m_showWireFrame) {  // switch for wireframe
        glDisable(GL_LIGHTING); // disable lighting
        glColor4fv(red);        // set a colour for the model wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // switch to line mode
        glPushMatrix();
        drawObjects();
        glPopMatrix();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // fill triangles for the rest of rendering
        glEnable(GL_LIGHTING);  // enable lighting for the rest of the rendering
    } else {
        glPushMatrix();
        drawObjects();
        glPopMatrix();
    }
}

void GLWidget::resizeGL(int w, int h) {

    viewWidth = w;
    viewHeight = h;

    m_perspective.zoom = 45.0;
    m_perspective.aspectRatio = static_cast<double>(viewWidth) / static_cast<float>(viewHeight ? viewHeight : 1.0f);
    m_perspective.nearPlane =  0.1f;
    m_perspective.farPlane = 100.0f;

    m_ortho.extent = 15.0f;
    m_ortho.nearPlane = 0.1f;
    m_ortho.farPlane = 100.0f;
    m_ortho.zoomFactor = 15.0f;

    glViewport(0, 0, viewWidth, viewHeight);
    this->update();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {

    case Qt::Key_1:
        m_showGrid = !m_showGrid;
        this->update();
        break;
    case Qt::Key_2:
        m_showWireFrame = !m_showWireFrame;
        this->update();
        break;
    case Qt::Key_3:
        m_runAnimation = !m_runAnimation;
        this->update();
        break;
    case Qt::Key_W:
        m_lookAt.eye += m_lookAt.center * 0.5f;
        this->update();
        break;
    case Qt::Key_S:
        m_lookAt.eye -= m_lookAt.center * 0.5f;
        this->update();
        break;
    case Qt::Key_A:
        m_lookAt.eye -= QVector3D::crossProduct(m_lookAt.center, QVector3D(0,1,0)) * 0.5f;
        this->update();
        break;
    case Qt::Key_D:
        m_lookAt.eye += QVector3D::crossProduct(m_lookAt.center, QVector3D(0,1,0)) * 0.5f;
        this->update();
        break;
    case Qt::Key_P:
        m_orthoProjection = !m_orthoProjection;
        this->update();
        break;
    case Qt::Key_Q:
        qApp->exit();
        break;
    default:
        event->ignore();
        break;
    }
}

void GLWidget::wheelEvent(QWheelEvent *event) {

    float delta = event->angleDelta().y() > 0 ? -5.0f : +5.0f;
    m_perspective.zoom += delta;
    if(m_perspective.zoom < 10.0f) {
        m_perspective.zoom = 10.0f;
    } else if(m_perspective.zoom > 120.0f) {
        m_perspective.zoom = 120.0f;
    }

    float numDegrees = (static_cast<float>(event->angleDelta().y()) / 8.0f);
    float numSteps = numDegrees / (180.0f * (1.0f / m_ortho.extent));
    m_ortho.extent -= numSteps;
    if (m_ortho.extent <= 0.0f) {
        m_ortho.extent = 0.0001f;
    }
    viewWidth = this->width();
    viewHeight = this->height();

    this->update();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_mousePressPosition = QVector2D(event->localPos());
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        // Update rotation
        auto diff = QVector2D(event->localPos()) - m_mousePressPosition;
        auto n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
        float mSpeed = diff.length() * 0.1f;
        mRotationAxis = (mRotationAxis + n * mSpeed).normalized();
        mRotation = QQuaternion::fromAxisAndAngle(mRotationAxis, mSpeed) * mRotation;

        // Update center vector based on rotation
        QMatrix4x4 mat;
        mat.setToIdentity();
        mat.rotate(mRotation);
        m_lookAt.center = {+0.0f, +0.0f, -1.0f};
        m_lookAt.center = m_lookAt.center * mat;

        m_mousePressPosition = QVector2D(event->localPos());
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
}

void GLWidget::resetCamera() {

    m_lookAt.eye = {+0.0f, +5.0f, 15.0f};
    m_lookAt.center = {+0.0f, +0.0f, -1.0f};
    m_lookAt.up = {+0.0f, +1.0f, +0.0f};

    mRotationAxis = QVector3D(0,0,0);
    mRotation = QQuaternion();

    m_ortho.extent = m_ortho.zoomFactor;
    m_perspective.zoom = 45.0;

    this->update();
}

// function to draw grid on z = 0 plane
void GLWidget::drawGrid(GLfloat *colour) {
    int x, z;
    int nGridlines = 3;
    // edges don't reflect
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, black);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, colour); // but they do emit
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    for (x = -nGridlines; x <= nGridlines; x++) { // for each x
        if (x % 12)
            glLineWidth(1.0);
        else
            glLineWidth(2.0);
        glBegin(GL_LINES);
        glVertex3i(x, 0, -nGridlines);
        glVertex3i(x, 0, +nGridlines);
        glEnd();
    }                                             // for each x
    for (z = -nGridlines; z <= nGridlines; z++) { // for each y
        if (z % 12)
            glLineWidth(1.0);
        else
            glLineWidth(2.0);
        glBegin(GL_LINES);
        glVertex3i(-nGridlines, 0, z);
        glVertex3i(+nGridlines, 0, z);
        glEnd();
    } // for each y
    glLineWidth(1.0);
    glPopMatrix();
    // stop emitting, otherwise other objects will emit the same colour
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
}


//  TO DO: draw an articultated character, you can use these
//  member functions or model the character using a separate class.

//  Criteria:

//  The character you create must have :

//  1. two arms with elbow joints
//  2. two legs with knee joints
//  3. a body
//  4. a head

//  You may also create as many other characters/objects for your scene as you wish

void GLWidget::setupObjects() {

    /* Example code to get you started */


    m_objects["universe"] = new Sphere(100.f,100,100);
    m_objects["universe"]->loadTexture(QDir::homePath() + "/Desktop/space.jpg", m_textures[1]);

    m_objects["body"] = new Cylinder(0.7f, 2.f, 32);
    m_objects["body"]->m_color = orange;

    m_objects["head"] = new Sphere(0.8f, 32, 32);
    m_objects["head"]->m_color = orange;

    m_objects["shoulder"] = new Sphere(0.3f, 32, 32);
    m_objects["shoulder"]->m_color = orange;

    m_objects["arm"] = new Cylinder(0.25f, 1.8f, 32);
    m_objects["arm"]->m_color = orange;

    m_objects["leg"] = new Cylinder(0.3f, 2.f, 32);
    m_objects["leg"]->m_color = orange;

    m_objects["earth"] = new Sphere(4.0f, 64, 64);
    m_objects["earth"]->loadTexture(QDir::homePath() + "/Desktop/earth.jpg", m_textures[0]);

    m_objects["sun"] = new Sphere(8.0f, 64, 64);
    m_objects["sun"]->loadTexture(QDir::homePath() + "/Desktop/sun.png", m_textures[2]);


}

void GLWidget::drawObjects() {

    /* Example code to get you started
     * Doesn't include any rotation/translation/scaling based on animation */



    //Universe
    glPushMatrix();
    m_objects["universe"]->draw();
    glPopMatrix();

    // Earth
    glPushMatrix();
    glTranslatef(18.f, 0.f, -30.f);
    glRotatef(-90.f, 1.f, 0.f, 0.f);
    m_objects["earth"]->draw();
    glPopMatrix();


    // Sun
    glPushMatrix();
    glTranslatef(0.f, 0.f, -30.f);
    glRotatef(-90.f, 1.f, 0.f, 0.f);
    m_objects["sun"]->draw();
    glPopMatrix();


    // Astronaut - transformations to apply to the full character
    glPushMatrix();
    glTranslatef(0.f, 3.f, -4.f);
    glScalef(0.6f, 0.6f, 0.6f);
    glTranslatef(15.f, 0.f, 0.f);

    // Now add body parts / bones in a hierarchy

        // body
        glPushMatrix();
        glTranslatef(0.0f, 1.5f, 0.0f);

            glPushMatrix();
            glRotatef(90.f, 1, 0, 0);
            m_objects["body"]->draw();
            glPopMatrix();

            // head
            glPushMatrix();
            glTranslatef(0.0f, 0.6f, 0.0f);
            m_objects["head"]->draw();
            glPopMatrix();

            // right shoulder
            glPushMatrix();
            glTranslatef(0.8f, -0.1f, 0.0f);
            m_objects["shoulder"]->draw();

                // right arm
                glPushMatrix();
                glTranslatef(0.0f, -0.1f, 0.0f);

                    glPushMatrix();
                    glRotatef(90.f, 1, 0, 0);
                    m_objects["arm"]->draw();
                    glPopMatrix();

                glPopMatrix();

            glPopMatrix();

            // left shoulder
            glPushMatrix();
            glTranslatef(-0.8f, -0.1f, 0.0f);
            m_objects["shoulder"]->draw();

                // left arm
                glPushMatrix();
                glTranslatef(0.0f, -0.1f, 0.0f);

                    glPushMatrix();
                    glRotatef(90.f, 1, 0, 0);
                    m_objects["arm"]->draw();
                    glPopMatrix();

                glPopMatrix();

            glPopMatrix();

            // right leg
            glPushMatrix();
            glTranslatef(0.3f, -1.9f, 0.0f);

                glPushMatrix();
                glRotatef(90.f, 1, 0, 0);
                m_objects["leg"]->draw();
                glPopMatrix();

            glPopMatrix();

            // left leg
            glPushMatrix();
            glTranslatef(-0.3f, -1.9f, 0.0f);

                glPushMatrix();
                glRotatef(90.f, 1, 0, 0);
                m_objects["leg"]->draw();
                glPopMatrix();

            glPopMatrix();

        glPopMatrix();

    glPopMatrix();
}

// Automatically called by the timer
void GLWidget::idle() {
    if (m_runAnimation == true) {
        if (m_time < m_nframes)
            m_time++;
        else
            m_time = 0;
        animate(m_time);
    }
    this->update();
}

void GLWidget::animate(int time) {
    /* TODO
     * This is where you should update the rotation/position/scale values for
     * different objects, such as those in the m_animation struct (see header)
     * The provided fields (e.g. left_arm, right_arm) are just examples, you can
     * add/subtract any fields that are useful for the animations you want to create. */


}

GLWidget::~GLWidget() {
}
