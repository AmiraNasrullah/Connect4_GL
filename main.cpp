#include "include/Angel.h"
#include "Circle.h"

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
const vec4 player1_color = vec4(1, 1, 0, 0);
const vec4 player2_color = vec4(1, 0.0, 1, 1);
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
int number_of_plays ;
int number_of_cells ;
/** the function to draw the board
 *
 */
void init_rectangle() {
	//init the points positions and color
	points[0] = vec2(-1*rectangle_offest_x, -1 * rectangle_offest_y);
	colors[0] = vec4(1,0,.2,1);

	points[1] = vec2(-1*rectangle_offest_x, rectangle_offest_y);
	colors[1] = vec4(0,0,1,1);

	points[2] = vec2(rectangle_offest_x, -1* rectangle_offest_y);
	colors[2] = vec4(1,0,1,1);

	points[3] = vec2(rectangle_offest_x, rectangle_offest_y);

	colors[3] = vec4(0,1,1,1);

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
	center = vec2(0, 0);
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
			cells[row][col] = new Circle(center, radius, circle_points_num, program, bgcolor);
			x += radius * 2 + 0.1;
		}
		y -= radius * 2 + 0.05;
		x = -0.7;
	}

}

//----------------------------------------------------------------------------

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(bgcolor.x,bgcolor.y,bgcolor.z,1); // white background

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // draw the square

	for (int row = 0; row < 6; row++) {
		for (int col = 0; col < 7; col++) {
			cells[row][col]->render();
		}
	}
	Circle* c;

if(!end_game){
	//change between players
	if (curr_player == 1)
		c = new Circle(center, radius, circle_points_num, program, player1_color);
	else
		c = new Circle(center, radius, circle_points_num, program, player2_color);

	c->render();
}
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
		std::cout << "Winner Player "<<player;
		end_game = true;
	}

}
//----------------------------------------------------------------------------

void update_board(int player, int col) {
	if (!end_game) {
		// initially game array is zero's
		int i = 0;
		for (i = 0; i < 6; i++) {
			if (game[i][col] != 0)
				break;
		}
		// add current game on this place i
		i = i - 1;
		if (i >= 0) {
			// we can add here
			game[i][col] = player;
			check_winner(player, i, col);

			if (player == 1) {
				cells[i][col]->change_color(player1_color);
				number_of_plays++;
				curr_player = 2;
			} else {
				cells[i][col]->change_color(player2_color);
				number_of_plays++;
				curr_player = 1;
			}
			if(number_of_plays==number_of_cells){
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
		update_board(curr_player, index);
		glutPostRedisplay();
		break;
	}
}

//----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		update_board(curr_player, index);
		glutPostRedisplay();
	}
}

void mouse_motion(int x, int y) {

	float cx, cy;
	int dx_l = 0;
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
		center = vec2(cx, cy);
		glutPostRedisplay();
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
