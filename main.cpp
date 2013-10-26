#include "include/Angel.h"
#include "Circle.h"
using namespace std;
// Window Variables
const GLfloat WindowWidth = 800;
const GLfloat WindowHeight = 800;

//---------------

// game variables
const int rectangle_points = 4;
const int circle_points_num = 20;
const float rectangle_offest_x = 0.9;
const float rectangle_offest_y = 0.7;
const GLfloat radius = 0.07;
const vec4 white = vec4(1, 1, 1, 1);
const vec4 bgcolor = vec4(1, 1, 1, 1);
const vec3 player1_color = vec3(1, 1, 0);
const vec3 player2_color = vec3(1, 0.0, 1);
const vec4 rectangle_color = vec4(1.0, 0.0, 1.0, 1);

//----------------------
int curr_player = 1; // grean
int game[6][7];
int row_pos = -1, col_pos = -1;
Circle*** cells;
vec2 points[rectangle_points];
vec4 colors[rectangle_points];
GLuint program;
GLuint vao;
vec2 center;
int index;
bool end_game;
int number_of_plays;
int number_of_cells;
int g_rotate = 0;
float angle_rotate = ((float) g_rotate / 180.0f) * 3.14;
//float player_color[] = {1,1,0,0};
Circle* c; /*moving circle*/
bool animating;
GLfloat matrix[] = { 1.0f, 0.0f, 0.0f, 0.0f, // first column
		0.0f, 1.0f, 0.0f, 0.0f, // second column
		0.0f, 0.0f, 1.0f, 0.0f, // third column
		0.0f, 0.0f, 0.0f, 1.0f // fourth column
		};
GLfloat rot_matrix[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos(angle_rotate), sin(
		angle_rotate), 0.0f, 0.0f, -sin(angle_rotate), cos(angle_rotate), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
GLfloat translate_matrix[16] = { 1.0f, 0.0f, 0.0f, 0.0f
		, 0.0f, 1, 0, 0.0f,
		0.0f,0, 1, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
GLfloat translate_back_matrix[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1, 0, 0.0f,
		0.0f, 0, 1, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
float freePlaces[] = { 6, 6, 6, 6, 6, 6, 6 };

void check_winner(int player, int row, int col);

void animate(int m) {
	//move down
	matrix[13] -= .05;

	//rotate
	g_rotate += 20;
	//	glUseProgram(program2);
	angle_rotate = ((float) g_rotate / 180.0f) * 3.14;
	rot_matrix[0] = cos(angle_rotate);
	rot_matrix[1] = sin(angle_rotate);
	rot_matrix[4] = -1 * sin(angle_rotate);
	rot_matrix[5] = cos(angle_rotate);

	translate_matrix[13] = -center.y;
	translate_matrix[12] = -center.x;

	translate_back_matrix[13] = center.y;
	translate_back_matrix[12] = center.x;
	cout << "matrix " << matrix[13] << "index " << index << endl;

	if (matrix[13] <= -1 * (freePlaces[index] / 6)) {
		g_rotate = 0;
		angle_rotate = ((float) g_rotate / 180.0f) * 3.14;
		rot_matrix[0] = cos(angle_rotate);
		rot_matrix[1] = sin(angle_rotate);
		rot_matrix[4] = -1 * sin(angle_rotate);
		rot_matrix[5] = cos(angle_rotate);
		int i = 0;
		for (i = 0; i < 6; i++) {
			if (game[i][index] != 0)
				break;
		}
		//back the ball again to the starting of the board
		matrix[13] = 0;
		// get free place in this column
		int row = (int) freePlaces[index] - 1;
		//set play
		game[row][index] = curr_player;
		check_winner(curr_player, row, index);
		if (curr_player == 1) {
			cells[row][index]->change_color(player1_color);
			freePlaces[index]--;
			number_of_plays++;
			curr_player = 2;
		} else {
			cells[row][index]->change_color(player2_color);
			freePlaces[index]--;
			number_of_plays++;
			curr_player = 1;
		}
		animating = false;

	} else {

		glutTimerFunc(30, animate, 0);
		animating = true;
	}
	glutPostRedisplay();

}
/** the function to draw the board
 *
 */
void init_rectangle() {
	//init the points positions and color
	points[0] = vec2(-1 * rectangle_offest_x, -1 * rectangle_offest_y);
	colors[0] = vec4(1, 0, .2, 1);

	points[1] = vec2(-1 * rectangle_offest_x, rectangle_offest_y);
	colors[1] = vec4(0, 0, 1, 1);

	points[2] = vec2(rectangle_offest_x, -1 * rectangle_offest_y);
	colors[2] = vec4(1, 0, 1, 1);

	points[3] = vec2(rectangle_offest_x, rectangle_offest_y);

	colors[3] = vec4(0, 1, 1, 1);

	// Create a vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
			(sizeof(vec2) + sizeof(vec4)) * rectangle_points, NULL,
			GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * rectangle_points,
			points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec2) * rectangle_points,
			sizeof(vec4) * rectangle_points, colors);
	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Initialize the vertex color attribute from the vertex shader
	GLuint loc2 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc2);
	glVertexAttribPointer(loc2, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(vec2) * rectangle_points));

}
//----------------------------------------------------------------------------

void init(void) {
	//initialize attributes
	end_game = false;
	index = 0;
	center = vec2(-0.7, rectangle_offest_y + radius);
	number_of_plays = 0;
	//initialize board

	// Load shaders and use the resulting shader program
	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	//initialize rectangle
	init_rectangle();
	// init cells (draw the white circles )
	cells = new Circle**[6];
	for (int i = 0; i < 6; ++i) {
		cells[i] = new Circle*[7];
	}
	number_of_cells = 6 * 7;

	float x = -0.7;
	float y = 0.5;
	glBindVertexArray(vao);
	for (int row = 0; row < 6; row++) {
		for (int col = 0; col < 7; col++) {
			vec2 center = vec2(x, y);
			cells[row][col] = new Circle(center, radius, circle_points_num,
					program, bgcolor);
			x += radius * 2 + 0.1;
		}
		y -= radius * 2 + 0.05;
		x = -0.7;
	}
	GLuint program2 = InitShader("moving.glsl", "fshader.glsl");
	if (curr_player == 1)
		c = new Circle(center, radius, 30, program2, player1_color);
	else
		c = new Circle(center, radius, 30, program2, player2_color);
}

//----------------------------------------------------------------------------

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(bgcolor.x, bgcolor.y, bgcolor.z, 1); // white background

	glBindVertexArray(vao);
	glUseProgram(program);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // draw the square

	for (int row = 0; row < 6; row++) {
		for (int col = 0; col < 7; col++) {
			cells[row][col]->render();
		}
	}

	GLuint program2 = InitShader("moving.glsl", "fshader.glsl");
	glUseProgram(program2);
	int matrix_location = glGetUniformLocation(program2, "matrix");
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, matrix);

	int trans_matrix_location = glGetUniformLocation(program2, "translate_mat");
	glUniformMatrix4fv(trans_matrix_location, 1, GL_FALSE, translate_matrix);
	int trans_back_matrix_location = glGetUniformLocation(program2, "translate_back_mat");
	glUniformMatrix4fv(trans_back_matrix_location, 1, GL_FALSE, translate_back_matrix);
	int rot_matrix_location = glGetUniformLocation(program2, "rot_matrix");
	glUniformMatrix4fv(rot_matrix_location, 1, GL_FALSE, rot_matrix);
//	GLint myLoc = glGetUniformLocation(program2, "vColor");
////		glUseProgram(program2);
//		glProgramUniform4fv(program2, myLoc, 1, player_color);
	if (curr_player == 1) {
//		glUniform4fv(myLoc, 1, player1_color);
		c->change_color(player1_color);
	} else {
//		glUniform4fv(myLoc, 1, player2_color);
		c->change_color(player2_color);
	}
	if (!end_game)
		c->render();

	glFlush();
}

void check_winner(int player, int row, int col) {
	int counter = 0;
	//check row
	for (int i = col; i < 7; i++) {
		if (game[row][i] == player) {
			counter++;
		} else
			break;
	}
	for (int i = col - 1; i >= 0; i--) {
		if (game[row][i] == player) {
			counter++;
		} else
			break;
	}

	if (counter < 4) {
		counter = 0;

		//check column
		for (int i = row; i < 6; i++) {
			if (game[i][col] == player) {
				counter++;
			} else
				break;
		}
		for (int i = row - 1; i >= 0; i--) {
			if (game[i][col] == player) {
				counter++;
			} else
				break;
		}
	}
	//check diagonal

	if (counter < 4) {

		counter = 0;
		int i = row;
		int j = col;
		while (i >= 0 && j >= 0) {
			if (game[i][j] == player) {
				counter++;
				i--;
				j--;
			} else
				break;
		}
		i = row + 1;
		j = col + 1;
		while (i < 6 && j < 7) {
			if (game[i][j] == player) {
				counter++;
				i++;
				j++;
			} else
				break;
		}

	}

	if (counter < 4) {
		counter = 0;
		int i = row;
		int j = col;
		while (i >= 0 && j < 7) {
			if (game[i][j] == player) {
				counter++;
				i--;
				j++;

			} else
				break;
		}
		i = row + 1;
		j = col - 1;
		while (i < 6 && j >= 0) {
			if (game[i][j] == player) {
				counter++;
				i++;
				j--;

			} else
				break;
		}
	}

	if (counter >= 4) {
		std::cout << "Winner Player " << player;
		end_game = true;
	}

}
//----------------------------------------------------------------------------

void update_board() {
	if (!end_game) {
		if (freePlaces[index] > 0) {
			glutTimerFunc(30, animate, 0);

			// initially game array is zero's

			// add current game on this place i
//		i = i - 1;
//		if (i >= 0) {
//			// we can add here
//			game[i][index] = curr_player;
//			check_winner(curr_player, i, index);

//			if (curr_player == 1) {
//				cells[i][index]->change_color(player1_color);
//				number_of_plays++;
//				curr_player = 2;
//			} else {
//				cells[i][index]->change_color(player2_color);
//				number_of_plays++;
//				curr_player = 1;
//			}
			if (number_of_plays == number_of_cells) {
				end_game = true;
			}
		}

	}

}
//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y) {

	int k = key;
	switch (k) {
	case 27:
		exit(EXIT_SUCCESS);
		break;
	case 13:
		update_board();
		glutPostRedisplay();
		break;
	}
}

//----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		update_board();
		glutPostRedisplay();
	}
}

void mouse_motion(int x, int y) {
	if (!animating) {
		float cx, cy;
		float dx = 0;
		//mouse in rectangle range
		if (x >= 100 && x < 728) {
			//get index of the cell
			index = ((x - 68) / 96);
			// 0.24 width of each cell
			cx = index * .24;
			cx = cx - rectangle_offest_x + 0.2;
			// 0.07 redius of each circle
			cy = rectangle_offest_y + radius;
			//center of the new circle
			dx = center.x - cx;
//		cout << "prev center " << center.x << "new center : " << cx << endl;

			matrix[12] = -center.x + cx;
//		std::cout << "matrix " << matrix[3] << "  dx : " << dx << endl;
			glutPostRedisplay();
		}
	}
}

//----------------------------------------------------------------------------

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);

	//----------------------------------------
	// If you are using freeglut, the next two lines will check if
	// the code is truly 3.2. Otherwise, comment them out

	//glutInitContextVersion( 3, 2 );
	//glutInitContextProfile( GLUT_CORE_PROFILE );
	//----------------------------------------

	glutCreateWindow("Connect4 Game");

	glewInit();

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glutPassiveMotionFunc(mouse_motion);
	glutMainLoop();

	return 0;
}
