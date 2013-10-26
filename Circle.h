/*
 * Circle.h
 *
 *  Created on: Oct 11, 2013
 *      Author: amira
 */

#ifndef CIRCLE_H_
#define CIRCLE_H_

#include "include/Angel.h"

class Circle {
public :
    GLuint gl_program;
    int pointsNum;
    vec2* points;
    vec4* colors;
    GLuint vao,vbo;
    //constructors
    // np = number of points to use in drawing the cirlce
    // the more the better
    Circle(vec2 center,GLfloat radius,int np,GLuint program,vec4 color);

    //destructor
    ~Circle();
    void change_color(vec4 color);
    void render();

};

#endif /* CIRCLE_H_ */
