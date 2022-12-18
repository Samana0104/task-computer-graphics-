


#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "CarGameTextureApp.h"
#include "../BaseCodes/Camera.h"
#include "../BaseCodes/InitShader.h"
#include "../BaseCodes/BasicShapeObjs.h"
#include "RingModel.h"
#include "GroundTexture.h"
#include "MoonTexture.h"


// Window and User Interface
static bool g_left_button_pushed;
static bool g_right_button_pushed;
static int g_last_mouse_x;
static int g_last_mouse_y;

extern GLuint g_window_w;
extern GLuint g_window_h;

//////////////////////////////////////////////////////////////////////
// Camera 
//////////////////////////////////////////////////////////////////////
static Camera g_camera;


//////////////////////////////////////////////////////////////////////
//// Define Shader Programs
//////////////////////////////////////////////////////////////////////
GLuint s_program_id;



//////////////////////////////////////////////////////////////////////
//// Animation Parameters
//////////////////////////////////////////////////////////////////////
float g_elaped_time_s = 0.f;	// 


//////////////////////////////////////////////////////////////////////
//// Car Position, Rotation, Velocity
//// �ڵ��� ���� ������.
//////////////////////////////////////////////////////////////////////
float g_ring_rotation_angle = 0;		          // ���� ���� (y�� ȸ��)

bool isShining = false;




/**
InitOpenGL: ���α׷� �ʱ� �� ������ ���� ���� �� �� ȣ��Ǵ� �Լ�. (main �Լ� ����)
OpenGL�� ������ �ʱ� ���� ���α׷��� �ʿ��� �ٸ� �ʱ� ���� �����Ѵ�.
�������, VAO�� VBO�� ���⼭ ������ �� �ִ�.
*/
void InitOpenGL()
{
	s_program_id = CreateFromFiles("../Shaders/v_shader_texture.glsl", "../Shaders/f_shader_texture.glsl");
	glUseProgram(s_program_id);

	glViewport(0, 0, (GLsizei)g_window_w, (GLsizei)g_window_h);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	// Initial State of Camera
	// ī�޶� �ʱ� ��ġ �����Ѵ�.
	g_camera.lookAt(glm::vec3(5.f, 8.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	
	// basic meshes
	InitBasicShapeObjs();


	// Ring
	InitRingModel();

	// �ٴ� VAO ����
	InitGroundTexture();

	// Moon VAO ����
	InitMoonTexture();
}







/**
ClearOpenGLResource: ���α׷��� ������ �޸� ������ ���� �� �� ȣ��Ǵ� �Լ�. (main �Լ� ����)
���α׷����� ����� �޸𸮸� ���⿡�� ������ �� �ִ�.
�������, VAO�� VBO�� ���⼭ ���� �� �ִ�.
*/
void ClearOpenGLResource()
{
	// Delete (VAO, VBO)
	DeleteBasicShapeObjs();
	DeleteRingModel();
	DeleteGroundTexture();
	DeleteMoonTexture();
}





/**
Display: ������ ȭ���� ������Ʈ �� �ʿ䰡 ���� �� ȣ��Ǵ� callback �Լ�.

������ �� ���� ����� ������ �ϴ� �ڵ�� �� �Լ� ���� �����ؾ��Ѵ�.
�����찡 ó�� ���� ��, ������ ũ�Ⱑ �ٲ� ��, �ٸ� �����쿡 ���� ȭ���� �Ϻ�
�Ǵ� ��ü�� �������ٰ� �ٽ� ��Ÿ�� �� �� �ý����� �ش� ������ ���� �׸��� ����
������Ʈ�� �ʿ��ϴٰ� �Ǵ��ϴ� ��� �ڵ����� ȣ��ȴ�.
���� ȣ���� �ʿ��� ��쿡�� glutPostRedisplay() �Լ��� ȣ���ϸ�ȴ�.

�� �Լ��� �ҽÿ� ����ϰ� ȣ��ȴٴ� ���� �����ϰ�, ������ ���� ��ȭ�� ������
1ȸ�� �ڵ�� �������� �� �Լ� �ۿ� �����ؾ��Ѵ�. Ư�� �޸� �Ҵ�, VAO, VBO ����
���� �ϵ���� ������ �õ��ϴ� �ڵ�� Ư���� ������ ���ٸ� ���� �� �Լ��� ���Խ�Ű��
�ȵȴ�. ���� ���, �޽� ���� �����ϰ� VAO, VBO�� �����ϴ� �κ��� ���� 1ȸ��
�����ϸ�ǹǷ� main() �Լ� �� �ܺο� �����ؾ��Ѵ�. ���ǵ� �޽� ���� ������ ���ۿ�
�׸����� �����ϴ� �ڵ常 �� �Լ��� �����ϸ� �ȴ�.

����, �� �Լ� ������ ���� �޸� �Ҵ��� �ؾ��ϴ� ��찡 �ִٸ� �ش� �޸𸮴� �ݵ��
�� �Լ��� ������ ���� ���� �ؾ��Ѵ�.

ref: https://www.opengl.org/resources/libraries/glut/spec3/node46.html#SECTION00081000000000000000
*/
#include <iostream>
void Display()
{
	// ��ü ȭ���� �����.
	// glClear�� Display �Լ� ���� �� �κп��� �� ���� ȣ��Ǿ���Ѵ�.

	if(isShining)
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	else
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	// Vertex shader �� matrix �������� location�� �޾ƿ´�.
	int m_proj_loc = glGetUniformLocation(s_program_id, "proj_matrix");
	int m_view_loc = glGetUniformLocation(s_program_id, "view_matrix");
	int m_model_loc = glGetUniformLocation(s_program_id, "model_matrix");

	
	// Projection Transform Matrix ����.
	glm::mat4 projection_matrix = glm::perspective(glm::radians(40.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
	glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	// Camera Transform Matrix ����.
	glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(g_camera.GetGLViewMatrix()));

	// texture filtering ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	// Moon
	{
		glUniform1i(glGetUniformLocation(s_program_id, "flag_texture"), true);
		glBindTexture(GL_TEXTURE_2D, s_program_id);

		glm::mat4 moon_T = glm::translate(glm::vec3(0.f, 3.2f, 0.f)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(moon_T));
		DrawMoonTexture();
	}

	glUniform1i(glGetUniformLocation(s_program_id, "flag_texture"), false);
	glBindTexture(GL_TEXTURE_2D, s_program_id);

	//���ĵ� �Ʒ�
	glVertexAttrib4f(2, 1.f, 1.f, 0.f, 1.f);
	glm::mat4 standCylinder = glm::translate(glm::vec3(0.f, 0.5f, 0.f)) * glm::scale(glm::vec3(0.7f, 1.0f, 0.7f));
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(standCylinder));
	DrawCylinder();

	// ���ĵ� �߾�
	glVertexAttrib4f(2, 0.9f, 0.9f, 0.f, 1.f);
	standCylinder = glm::rotate(glm::radians(g_ring_rotation_angle), glm::vec3(0.f, -1.f, 0.f)) *
		glm::translate(glm::vec3(0.f, 1.3f, 0.f)) * glm::scale(glm::vec3(0.25f, 0.8f, 0.25f));
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(standCylinder));
	DrawCylinder();

	// ���ĵ� ����
	glVertexAttrib4f(2, 1.f, 1.f, 1.f, 1.f);
	standCylinder = glm::translate(glm::vec3(0.f, 2.0f, 0.f)) * glm::scale(glm::vec3(0.05f, 0.7f, 0.05f));
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(standCylinder));
	DrawCylinder();

	// ���ĵ� ��ư1
	glVertexAttrib4f(2, 1.f, 1.f, 1.f, 1.f);
	if(isShining)
		standCylinder = glm::translate(glm::vec3(0.f, 0.3f, 0.6f)) * glm::rotate(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(0.1f, 0.3f, 0.1f));
	else
		standCylinder = glm::translate(glm::vec3(0.f, 0.3f, 0.7f)) * glm::rotate(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(0.1f, 0.3f, 0.1f));
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(standCylinder));
	DrawCylinder();

	// ���ĵ� ��ư2
	if(isShining)
		standCylinder = glm::translate(glm::vec3(0.f, 0.7f, 0.6f)) * glm::rotate(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(0.1f, 0.3f, 0.1f));
	else
		standCylinder = glm::translate(glm::vec3(0.f, 0.7f, 0.7f)) * glm::rotate(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(0.1f, 0.3f, 0.1f));	
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(standCylinder));
	DrawCylinder();

	// ���� ��ħ��1
	standCylinder = glm::rotate(glm::radians(g_ring_rotation_angle), glm::vec3(0.f, -1.f, 0.f)) * 
		glm::translate(glm::vec3(0.f, 1.3f, 2.f)) * glm::scale(glm::vec3(0.08f, 0.08f, 4.f));
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(standCylinder));
	DrawBox();

	// ���� ��ħ��2
	standCylinder = glm::rotate(glm::radians(g_ring_rotation_angle), glm::vec3(0.f, -1.f, 0.f)) * 
		glm::translate(glm::vec3(0.f, 1.785f, 4.f)) * glm::scale(glm::vec3(0.08f, 1.05f, 0.08f));
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(standCylinder));
	DrawBox();

	// ���� ��
	glm::mat4 standRing = glm::rotate(glm::radians(g_ring_rotation_angle), glm::vec3(0.f, -1.f, 0.f)) * 
		glm::translate(glm::vec3(0.f, 3.1f, 4.f)) * glm::scale(glm::vec3(4.f, 4.f, 4.f));
	glUniformMatrix4fv(m_model_loc, 1, GL_FALSE,  glm::value_ptr(standRing));
	DrawRingModel();


	glm::mat4 standSphere;

	glVertexAttrib4f(2, 1.f, 1.f, 1.f, 1.f);

	for (int i = 0; i < 360; i+=20)
	{
		standSphere = glm::rotate(glm::radians((float)i), glm::vec3(0.f, 0.f, 1.f)) * glm::translate(glm::vec3(0.8f, 0.f, 0.f)) * glm::scale(glm::vec3(0.05f, 0.05f, 0.05f));
		standSphere = glm::rotate(glm::radians(g_ring_rotation_angle), glm::vec3(0.f, -1.f, 0.f)) *
			glm::translate(glm::vec3(0.f, 3.1f, 3.92f)) * standSphere;
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(standSphere));
		DrawSphere();
		
	}


	glm::mat4 view_matrix = g_camera.GetGLViewMatrix();
	glm::vec3 pos, dir;
	glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));


	// ���� ���� ���� (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl ����.

	// ���� ����ϴ� ��ġ(����) ����.
	pos = glm::rotate(glm::radians(g_ring_rotation_angle), glm::vec3(0.f, -1.f, 0.f)) *
		glm::translate(glm::vec3(0.f, 3.1f, 6.f)) * glm::vec4(0.f, 0.f, 0.f, 1.f);

	// Apply Camera Matrices
	////// *** ���� ī�޶� ������ �����ϱ� ���� view transform ����  ***
	//  �̶� pos�� ��ġ�� ��Ÿ���� ����Ʈ�̹Ƿ� �̵�(Translation)��ȯ�� ����ǵ��� �Ѵ�. (�� ��° ��� 1.f���� ����)
	pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

	int pos_loc = glGetUniformLocation(s_program_id, "spotLight.position");
	glUniform3f(pos_loc, pos[0], pos[1], pos[2]);

	//glUniform1f(glGetUniformLocation(s_program_id, "shininess_n"), 300.f);
	// ���� ���� ����.
	// �ð��� ���� ������ ���ϵ��� ��.
	dir = glm::rotate(glm::radians(g_ring_rotation_angle), glm::vec3(0.f, -1.f, 0.f)) *
		glm::vec4(glm::vec3(0.f, 0.f, -1.0f), 1.f);
	dir = glm::normalize(dir);

	////// *** ���� ī�޶� ������ �����ϱ� ���� view transform ����  ***
	//  dir�� ������ ��Ÿ���� �����̹Ƿ� �̵�(Translation)��ȯ�� ���õǵ��� �Ѵ�. (�� ��° ��� 0.f���� ����)
	dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

	int dir_loc = glGetUniformLocation(s_program_id, "spotLight.dir");
	glUniform3f(dir_loc, dir[0], dir[1], dir[2]);

	// ���� ���� ���� ����.
	int light_cos_cutoff_loc = glGetUniformLocation(s_program_id, "spotLight.cos_cutoff");
	glUniform1f(light_cos_cutoff_loc, cos(40.f / 180.f * glm::pi<float>()));

	if (isShining) 
	{
		// ���� ���� ����
		glUniform1i(glGetUniformLocation(s_program_id, "lightType"), 2);
		glUniform1f(glGetUniformLocation(s_program_id, "spotLight.intensity"), 1.4f);
	}
	else 
	{
		glUniform1i(glGetUniformLocation(s_program_id, "lightType"), 1);

		pos = glm::vec3(20.f, 20.f, 20.f);
		pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

		int pos_loc = glGetUniformLocation(s_program_id, "directionLight.position");
		glUniform3f(pos_loc, pos[0], pos[1], pos[2]);

		dir = glm::normalize(glm::vec3(-1.f, -1.f, -1.f));
		int dir_loc = glGetUniformLocation(s_program_id, "directionLight.dir");
		glUniform3f(dir_loc, dir[0], dir[1], dir[2]);

		// ���� ���� ����
		glUniform1f(glGetUniformLocation(s_program_id, "directionLight.intensity"), 1.f);
		glUniform1f(glGetUniformLocation(s_program_id, "spotLight.intensity"), 0.9f);
	}

	glutSwapBuffers();
}


/**
Timer: ������ �ð� �Ŀ� �ڵ����� ȣ��Ǵ� callback �Լ�.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node64.html#SECTION000819000000000000000
*/
void Timer(int value)
{
	// Timer ȣ�� �ð� ������ �����Ͽ�, ���� Timer�� ȣ��� �ĺ��� ������� �귯�� ����Ѵ�.
	g_elaped_time_s += value/1000.f;

	g_ring_rotation_angle = (int)g_ring_rotation_angle%360;
	g_ring_rotation_angle += 1.5f;


	// glutPostRedisplay�� ������ ���� �ð� �ȿ� ��ü �׸��� �ٽ� �׸� ���� �ý��ۿ� ��û�Ѵ�.
	// ��������� Display() �Լ��� ȣ�� �ȴ�.
	glutPostRedisplay();

	// 1/60 �� �Ŀ� Timer �Լ��� �ٽ� ȣ��Ƿη� �Ѵ�.
	// Timer �Լ� �� ������ �ð� �������� �ݺ� ȣ��ǰ��Ͽ�,
	// �ִϸ��̼� ȿ���� ǥ���� �� �ִ�
	glutTimerFunc((unsigned int)(1000 / 60), Timer, (1000 / 60));
}




/**
Reshape: �������� ũ�Ⱑ ������ ������ �ڵ����� ȣ��Ǵ� callback �Լ�.

@param w, h�� ���� ������ �������� ���� ũ��� ���� ũ�� (�ȼ� ����).
ref: https://www.opengl.org/resources/libraries/glut/spec3/node48.html#SECTION00083000000000000000
*/
void Reshape(int w, int h)
{
	//  w : window width   h : window height
	g_window_w = w;
	g_window_h = h;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glutPostRedisplay();
}

/**
Keyboard: Ű���� �Է��� ���� ������ �ڵ����� ȣ��Ǵ� �Լ�.
@param key�� ������ Ű������ ���ڰ�.
@param x,y�� ���� ���콺 �������� ��ǥ��.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)						
	{
	//case 's':
	//	g_car_speed = -0.01f;		// ���� �ӵ� ����
	//	glutPostRedisplay();
	//	break;

	//case 'w':
	//	g_car_speed = 0.01f;		// ���� �ӵ� ����
	//	glutPostRedisplay();
	//	break;

	//case 'a':
	//	g_car_angular_speed = glm::radians( 1.f );		// ��ȸ�� ���ӵ� ����
	//	glutPostRedisplay();
	//	break;

	//case 'd':
	//	g_car_angular_speed = -1 * glm::radians( 1.f );		//  ��ȸ�� ���ӵ� ����
	//	glutPostRedisplay();
	//	break;
	case 'n':
	case 'N':
		if (isShining)
			isShining = false;
		else
			isShining = true;
	}

}

/**
KeyboardUp: �������� Ű�� ������ ������ �ڵ����� ȣ��Ǵ� �Լ�.
@param key�� �ش� Ű������ ���ڰ�.
@param x,y�� ���� ���콺 �������� ��ǥ��.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/
void KeyboardUp(unsigned char key, int x, int y)
{
	//switch (key)						
	//{
	//case 's':
	//	g_car_speed = 0.f;		// ���� �ӵ� ����
	//	glutPostRedisplay();
	//	break;

	//case 'w':
	//	g_car_speed = 0.f;		// ���� �ӵ� ����
	//	glutPostRedisplay();
	//	break;

	//case 'a':
	//	g_car_angular_speed = 0.f;		// ��ȸ�� ���ӵ� ����
	//	glutPostRedisplay();
	//	break;

	//case 'd':
	//	g_car_angular_speed = 0.f;		//  ��ȸ�� ���ӵ� ����
	//	glutPostRedisplay();
	//	break;

	//}

}



/**
Mouse: ���콺 ��ư�� �Էµ� ������ �ڵ����� ȣ��Ǵ� �Լ�.
�Ķ������ �ǹ̴� ������ ����.
@param button: ���� ��ư�� ����
  GLUT_LEFT_BUTTON - ���� ��ư
  GLUT_RIGHT_BUTTON - ������ ��ư
  GLUT_MIDDLE_BUTTON - ��� ��ư (���� �������� ��)
  3 - ���콺 �� (���� ���� ���� ����).
  4 - ���콺 �� (���� �Ʒ��� ���� ����).
@param state: ���� ����
  GLUT_DOWN - ���� ����
  GLUT_UP - ��������
@param x,y: ������ �Ͼ�� ��, ���콺 �������� ��ǥ��.
*/
void Mouse(int button, int state, int x, int y)
{
	float mouse_xd = (float)x / g_window_w;
	float mouse_yd = 1 - (float)y / g_window_h;
	float last_mouse_xd = (float)g_last_mouse_x / g_window_w;
	float last_mouse_yd = 1 - (float)g_last_mouse_y / g_window_h;


	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		g_left_button_pushed = true;

	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		g_left_button_pushed = false;

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		g_right_button_pushed = true;

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		g_right_button_pushed = false;
	else if (button == 3)
	{
		g_camera.inputMouse(Camera::IN_TRANS_Z, 0, -1, 0.01f);
		glutPostRedisplay();
	}
	else if (button == 4)
	{
		g_camera.inputMouse(Camera::IN_TRANS_Z, 0, 1, 0.01f);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}





/**
MouseMotion: ���콺 �����Ͱ� ������ ������ �ڵ����� ȣ��Ǵ� �Լ�.
@prarm x,y�� ���� ���콺 �������� ��ǥ���� ��Ÿ����.
*/
void MouseMotion(int x, int y)
{
	float mouse_xd = (float)x / g_window_w;
	float mouse_yd = 1 - (float)y / g_window_h;
	float last_mouse_xd = (float)g_last_mouse_x / g_window_w;
	float last_mouse_yd = 1 - (float)g_last_mouse_y / g_window_h;

	if (g_left_button_pushed)
	{
		g_camera.inputMouse(Camera::IN_ROTATION_Y_UP, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd);
		glutPostRedisplay();
	}
	else if (g_right_button_pushed)
	{
		g_camera.inputMouse(Camera::IN_TRANS, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd, 0.01f);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}