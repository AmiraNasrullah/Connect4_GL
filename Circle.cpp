/*
 * Circle.cpp
 *
 *  Created on: Oct 11, 2013
 *      Author: amira
 */

#include "Circle.h"
#include "include/Angel.h"

Circle::Circle(vec2 center, GLfloat radius, int points_num, GLuint program,
		vec4 color) {
	gl_program = program;
	//number of points to draw circle
	pointsNum = points_num; //np
	//points positions
	points = new vec2[pointsNum];
	//color of the circle
	colors = new vec4[pointsNum];
	for (int i = 0; i < points_num; i++) {
		double angle = 360.0 / points_num * i;
		double rad = angle / 180.0 * 3.1415;
		points[i] = center + vec2(radius * cos(rad), radius * sin(rad));
		colors[i] = vec4(color.x, color.y, color.z, 1);
	}
	// Create a vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// BUFFERS
	// Create and initialize a buffer object
	// NOTE: since both vertices positions and colors are static data
	// (ie. have the same use) we can put them on the same buffer
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec2) + sizeof(vec4)) * pointsNum,
			NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * pointsNum, points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec2) * pointsNum,
			sizeof(vec4) * pointsNum, colors);

	// ATTRIBUTES
	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Initialize the vertex color attribute from the vertex shader
	GLuint loc2 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc2);
	glVertexAttribPointer(loc2, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(vec2) * pointsNum));

}
/**
 * this function change the color of the circle (to fill the board)
 */
void Circle::change_color(vec4 color) {
    //change color of the circle points
	colors = new vec4[pointsNum];
	for (int i = 0; i < pointsNum; i++) {
		colors[i] = vec4(color.x, color.y, color.z, 1);
	}
    //bind vao
	glBindVertexArray(vao);
	//bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//put color array in the buffer
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec2) * pointsNum,
			sizeof(vec4) * pointsNum, colors);

	// ATTRIBUTES
	// Initialize the vertex position attribute from the vertex shader

	// Initialize the vertex color attribute from the vertex shader
	GLuint loc2 = glGetAttribLocation(gl_program, "vColor");
	glEnableVertexAttribArray(loc2);
	glVertexAttribPointer(loc2, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(vec2) * pointsNum));

}
void Circle::render() {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, pointsNum);

}
