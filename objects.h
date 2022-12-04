#pragma once

#include <QGLWidget>
#include <QObject>
#include <QtOpenGL>
#include <QOpenGLWidget>
#include <point.h>

class Object : public QObject {
public:
    Object(QOpenGLWidget *parent = nullptr);
    virtual void draw() = 0;

    void loadTexture(QString fileName, GLuint textureId);

    GLfloat* m_color;
    GLfloat* m_specularColor;
    float m_shininess;

    // Note that only Cube and Sphere have texture coordinates defined
    GLuint m_texture;
    bool m_hasTexture;



protected:
    // To execute some common drawing code
    void setDrawProperties();

private:
    QOpenGLWidget *qGLWidget;
};


class Tetrahedron : public Object {
public:
    void draw() override;
};

class Cube : public Object {
public:
    void draw() override;
};

class Octahedron : public Object {
public:
    void draw() override;
};

class Cone : public Object {
public:
    Cone(float radius, float height, int nSlices);
    void draw() override;
private:
    float m_radius;
    float m_height;
    float m_nSlices;
};

class Cylinder : public Object {
public:
    Cylinder(float radius, float height, int nSegments);
    void draw() override;
private:
    float m_radius;
    float m_height;
    float m_nSegments;
};

class Sphere : public Object {
public:
    Sphere(float radius, int nSegments, int nSlices);
    void draw() override;
private:
    float m_radius;
    float m_nSegments;
    float m_nSlices;
};


class Quad : public Object {
public:
   void draw() override;

};

/*Point vertices[8] = {
    Point(-1.0, -1.0, -1.0), Point(-1.0, -1.0, 1.0),
    Point(-1.0, 1.0, -1.0),  Point(-1.0, 1.0, 1.0),
    Point(1.0, -1.0, -1.0),  Point(1.0, -1.0, 1.0),
    Point(1.0, 1.0, -1.0),   Point(1.0, 1.0, 1.0)
};

int faces[6][4] = {
    {0, 4, 5, 1}, {0, 2, 6, 4}, {0, 1, 3, 2},
    {4, 6, 7, 5}, {1, 5, 7, 3}, {2, 3, 7, 6}
};*/

