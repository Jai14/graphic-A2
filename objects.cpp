#include "objects.h"
#include <cmath>

const float PI = 3.1415926f;
static GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

Object::Object(QOpenGLWidget *parent) : QObject(parent) {
    qGLWidget = parent;
    m_color = white;
    m_specularColor = white;
    m_shininess = 64;
    m_hasTexture = false;
}

void Object::loadTexture(QString fileName, GLuint textureId){

    m_texture = textureId;

    QImage img;
    img.load(fileName);
    QImage glFormattedImage = QGLWidget::convertToGLFormat(img);
    if (!glFormattedImage.isNull()) {
        qDebug() << "Texture read correctly...";
        m_hasTexture = true;
        // Set the texture as m_texture for later access
        glBindTexture(GL_TEXTURE_2D, m_texture);
        // tell OpenGL which texture you want to use
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glFormattedImage.width(),
                     glFormattedImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     glFormattedImage.bits());
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    } else {
        qDebug() << "Image would not load";
    }
}

void Object::setDrawProperties(){
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, m_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_specularColor);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_shininess);

    if(m_hasTexture){
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glEnable(GL_TEXTURE_2D);
    }
}


// TETRAHEDRON
void Tetrahedron::draw() {

    setDrawProperties();

    Point vertices[4] = {
        Point(-1.0, -1.0, -1.0), // 0
        Point(-1.0, 1.0, 1.0),   // 1
        Point(1.0, -1.0, 1.0),   // 2
        Point(1.0, 1.0, -1.0)    // 3
    };

    // Go counter clockwise
    int faces[4][3] = {
        {0, 2, 1}, // face 0
        {1, 3, 0}, // face 1
        {2, 3, 1}, // face 2
        {2, 0, 3}  // face 3
    };

    glBegin(GL_TRIANGLES);

    // Draw 4 faces with 3 vertices each
    for (int i = 0; i < 4; i++) {
        // To find the normal vector for this face: make vectors for
        // two edges and compute cross product
        Vector v = vertices[faces[i][1]] - vertices[faces[i][0]];
        Vector u = vertices[faces[i][2]] - vertices[faces[i][0]];
        Vector normal = v.cross(u).normalised();
        glNormal3f(normal.dx, normal.dy, normal.dz);
        for (int j = 0; j < 3; j++){
            int vertexIndex = faces[i][j];
            glVertex3f(vertices[vertexIndex].x, vertices[vertexIndex].y, vertices[vertexIndex].z);
        }
    }
    glEnd();

    if(m_hasTexture){
        glDisable(GL_TEXTURE_2D);
    }
}


// CUBE
void Cube::draw() {

    Point vertices[8] = {
        Point(-1.0, -1.0, -1.0), Point(-1.0, -1.0, 1.0),
        Point(-1.0, 1.0, -1.0),  Point(-1.0, 1.0, 1.0),
        Point(1.0, -1.0, -1.0),  Point(1.0, -1.0, 1.0),
        Point(1.0, 1.0, -1.0),   Point(1.0, 1.0, 1.0)
    };

    int faces[6][4] = {
        {0, 4, 5, 1}, {0, 2, 6, 4}, {0, 1, 3, 2},
        {4, 6, 7, 5}, {1, 5, 7, 3}, {2, 3, 7, 6}
    };

    // Tell OpenGL we are going to pass in info for quads

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    // Draw 6 faces w/ 4 vertices each
    for(int i = 0; i < 6; ++i){
        // Compute face normal
        Vector v = vertices[faces[i][1]] - vertices[faces[i][0]];
        Vector u = vertices[faces[i][2]] - vertices[faces[i][0]];
        Vector normal = v.cross(u).normalised();
        glNormal3f(normal.dx, normal.dy, normal.dz);

        // Vertex positions and matching texture coordinates

        glVertex3f(vertices[faces[i][0]].x, vertices[faces[i][0]].y, vertices[faces[i][0]].z);
        glTexCoord2f(0, 0);

        glVertex3f(vertices[faces[i][1]].x, vertices[faces[i][1]].y, vertices[faces[i][1]].z);
        glTexCoord2f(0, 1);

        glVertex3f(vertices[faces[i][2]].x, vertices[faces[i][2]].y, vertices[faces[i][2]].z);
        glTexCoord2f(1, 0);

        glVertex3f(vertices[faces[i][3]].x, vertices[faces[i][3]].y, vertices[faces[i][3]].z);
        glTexCoord2f(1, 1);

    }
    glEnd();


    if(m_hasTexture){
        glDisable(GL_TEXTURE_2D);
    }

}


// OCTAHEDRON
void Octahedron::draw() {

    setDrawProperties();

    Point vertices[6] = {
        Point(-1.0, 0.0, 0.0), Point(0.0, -1.0, 0.0),
        Point(0.0, 0.0, -1.0), Point(1.0, 0.0, 0.0),
        Point(0.0, 1.0, 0.0),  Point(0.0, 0.0, 1.0)
    };

    int faces[8][3] = {
        {1, 5, 0}, {1, 3, 5}, {3, 4, 5}, {4, 0, 5},
        {2, 0, 4}, {3, 2, 4}, {1, 2, 3}, {1, 0, 2}
    };

    // Tell OpenGL we are going to pass in info for triangles
    glBegin(GL_TRIANGLES);

    // Draw 8 faces w/ 3 vertices each
    for (int i = 0; i < 8; i++) {
        // Compute face normal
        Vector v = vertices[faces[i][1]] - vertices[faces[i][0]];
        Vector u = vertices[faces[i][2]] - vertices[faces[i][0]];
        Vector normal = v.cross(u).normalised();
        glNormal3f(normal.dx, normal.dy, normal.dz);
        // Vertex position
        for (int j = 0; j < 3; j++){
            int vertexIndex = faces[i][j];
            glVertex3f(vertices[vertexIndex].x, vertices[vertexIndex].y, vertices[vertexIndex].z);
        }
    }
    glEnd();
}


// CONE
Cone::Cone(float radius, float height, int nSlices){
    m_radius = radius;
    m_height = height;
    m_nSlices = nSlices;
}

void Cone::draw() {

    setDrawProperties();

    glBegin(GL_TRIANGLES);

    // Loop around the circumference
    for (float i = 0.0; i <= static_cast<float>(m_nSlices); i += 1.0f) {
        float angle = (PI * i * 2.0f) / static_cast<float>(m_nSlices);
        float nextAngle = (PI * (i+1) * 2.0f) / static_cast<float>(m_nSlices);
        float x1 = sin(angle) * m_radius, y1 = cos(angle) * m_radius;
        float x2 = sin(nextAngle) * m_radius, y2 = cos(nextAngle) * m_radius;

        // Triangle for 1 slice
        glNormal3f(x1, y1, 0); glVertex3f(x1, y1, 0);
        glNormal3f(x2, y2, 0); glVertex3f(x2, y2, 0);
        glNormal3f(x2, y2, 0); glVertex3f(0, 0, m_height);


        // Triangle for the bottom circle
        glNormal3f(0.0f, 0.0f, -1.0f); // point down
        glVertex3f(x1, y1, 0);
        glVertex3f(x2, y2, 0);
        glVertex3f(0, 0, 0);
    }
    glEnd();
}


// CYLINDER
Cylinder::Cylinder(float radius, float height, int nSegments) {
    m_radius = radius;
    m_height = height;
    m_nSegments = nSegments;
}

void Cylinder::draw() {

    setDrawProperties();

    glBegin(GL_TRIANGLES);

    // Loop around the circumference
    for (float i = 0.0; i <= m_nSegments; i += 1.0f) {
        float angle = (PI * i * 2.0f) / static_cast<float>(m_nSegments);
        float nextAngle = (PI * (i+1) * 2.0f) / static_cast<float>(m_nSegments);
        float x1 = sin(angle) * m_radius, y1 = cos(angle) * m_radius;
        float x2 = sin(nextAngle) * m_radius, y2 = cos(nextAngle) * m_radius;

        // Draw the top triangle (orange) - normal and position for each vertex
        glNormal3f(x1, y1, 0); glVertex3f(x1, y1, 0);
        glNormal3f(x2, y2, 0); glVertex3f(x2, y2, m_height);
        glNormal3f(x1, y1, 0); glVertex3f(x1, y1, m_height);

        // Draw bottom triangle (cyan) - normal and position for each vertex
        glNormal3f(x1, y1, 0); glVertex3f(x1, y1, 0);
        glNormal3f(x2, y2, 0); glVertex3f(x2, y2, 0);
        glNormal3f(x2, y2, 0); glVertex3f(x2, y2, m_height);

        // For a closed cylinder: a bottom triangle
        glNormal3f(0.0f, 0.0f, -1.0f); // point down
        glVertex3f(x1, y1, 0);
        glVertex3f(x2, y2, 0);
        glVertex3f(0, 0, 0);

        // For a closed cylinder: a top triangle
        glNormal3f(0.0f, 0.0f, 1.0f); // point up
        glVertex3f(x1, y1, m_height);
        glVertex3f(x2, y2, m_height);
        glVertex3f(0.0f, 0.0f, m_height);
    }
    glEnd();
}


// SPHERE
Sphere::Sphere(float radius, int nSegments, int nSlices){
    m_radius = radius;
    m_nSegments = nSegments;
    m_nSlices = nSlices;
}

void Sphere::draw() {

    setDrawProperties();

    float x, y, z; // quad vertices and normals
    float s, t; // texture coordinates

    // circumference steps
    float theta_step = (2.0f * PI) /  static_cast<float>(m_nSlices);

    // top to bottom steps (half of circumference)
    float phi_step = PI /  static_cast<float>(m_nSegments);

    glBegin(GL_QUADS);

    // Loop around the circumference with theta_step,
    // and around half the circumference with phi_step
    for (float theta = -PI; theta < PI; theta += theta_step) {
        for (float phi = -(PI / 2.0f); phi < (PI / 2.0f); phi += phi_step) {
            // Calculate the four vertex corners of the quad:
            //1
            x = cos(phi) * cos(theta) * m_radius;
            y = cos(phi) * sin(theta) * m_radius;
            z = sin(phi) * m_radius;
            t = (phi / PI) + 0.5f;
            s = (theta / (PI * 2.0f)) + 0.5f;
            glTexCoord2f(s, t);
            glNormal3f(x, y, z);
            glVertex3f(x, y, z);

            // 2
            x = cos(phi) * cos(theta + theta_step) * m_radius;
            y = cos(phi) * sin(theta + theta_step) * m_radius;
            z = sin(phi) * m_radius;
            t = (phi / PI) + 0.5f;
            s = ((theta + theta_step) / (PI * 2.0f)) + 0.5f;
            glTexCoord2f(s, t);
            glNormal3f(x, y, z);
            glVertex3f(x, y, z);

            // 3
            x = cos(phi + phi_step) * cos(theta + theta_step) * m_radius;
            y = cos(phi + phi_step) * sin(theta + theta_step) * m_radius;
            z = sin(phi + phi_step) * m_radius;
            t = ((phi + phi_step) / PI) + 0.5f;
            s = ((theta + theta_step) / (PI * 2.0f)) + 0.5f;
            glTexCoord2f(s, t);
            glNormal3f(x, y, z);
            glVertex3f(x, y, z);

            // 4
            x = cos(phi + phi_step) * cos(theta) * m_radius;
            y = cos(phi + phi_step) * sin(theta) * m_radius;
            z = sin(phi + phi_step) * m_radius;
            t = ((phi + phi_step) / PI) + 0.5f;
            s = (theta / (PI * 2.0f)) + 0.5f;
            glTexCoord2f(s, t);
            glNormal3f(x, y, z);
            glVertex3f(x, y, z);
        }
    }

    glEnd();

    if(m_hasTexture){
        glDisable(GL_TEXTURE_2D);
    }
}

//void Quad::draw(){

//    int offset = 1.0f;
//    offset++;
//    glDisable(GL_DEPTH_TEST);
//    setDrawProperties();

//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glLoadIdentity();
//    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//    glLoadIdentity();

//    glBegin( GL_QUADS );

//       glVertex2f(-1.0f, -1.0f);
//       glTexCoord3f(0.0f + offset,0.0f,1.0f);
//       glVertex2f(1.0f, -1.0f);
//       glTexCoord3f(1.0f + offset,0.0f,1.0f);
//       glVertex2f(1.0f, 1.0f);
//       glTexCoord3f(1.0f + offset,1.0f,1.0f);
//       glVertex2f(-1.0f, 1.0f);
//       glTexCoord3f(0.0f + offset,1.0f,1.0f);

//    glEnd();


//    glPopMatrix();
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glMatrixMode(GL_MODELVIEW);


//    glEnable(GL_DEPTH_TEST);



//}
