#include <iostream>
#include "GL/glew.h"
#include "GL/freeglut.h"


GLuint g_window_w = 500;
GLuint g_window_h = 500;


//////////////////////////////////////////////////////////////////////
//// Define Mesh Model
//////////////////////////////////////////////////////////////////////
const int g_num_vertices = 3;
const int g_num_triangles = 1;

GLfloat m_positions[g_num_vertices * 3] =
{ -100.f,  250.f,  -150.f
, 100.f, 250.f,  -150.f
, 0.f, 250.f, -300.f
};

GLfloat m_normals[g_num_vertices * 3] =
{ 0.f, -1.f, 0.f
, 0.f, -1.f, 0.f
, 0.f, -1.f, 0.f
};

GLfloat m_colors[g_num_vertices * 4] =
{ 1.f, 0.f, 0.f, 1.f
, 0.f, 1.f, 0.f, 1.f
, 0.f, 0.f, 1.f, 1.f
};

GLuint m_indices[g_num_triangles * 3] =
{
	0, 1, 2
};


//////////////////////////////////////////////////////////////////////
//// Define Shader Programs
//////////////////////////////////////////////////////////////////////

GLuint f_shader_id;
GLuint v_shader_id;
GLuint s_program_id;

// vertex shader source code
const GLchar * v_shader_source =
R"(
	#version 330

	layout (location=0) in vec3 vs_position;
	layout (location=1) in vec3 vs_normal;
	layout (location=2) in vec4 vs_color;

	out vec4 fs_color;

	void main()
	{
		mat4 T; // ��� �̵�
	    T[0][0] = 1.f; T[1][0] = 0.f; T[2][0] = 0.f; T[3][0] = 0.f;
	    T[0][1] = 0.f; T[1][1] = 1.f; T[2][1] = 0.f; T[3][1] = -250.f;
	    T[0][2] = 0.f; T[1][2] = 0.f; T[2][2] = 1.f; T[3][2] = 225.f;
	    T[0][3] = 0.f; T[1][3] = 0.f; T[2][3] = 0.f; T[3][3] = 1.f;
		
		mat4 R1; // X�� ���� ȸ��
		R1[0][0] = 1.f; R1[1][0] = 0.f; R1[2][0] = 0.f; R1[3][0] = 0.f;
		R1[0][1] = 0.f; R1[1][1] = cos(radians(90.f)); R1[2][1] = -sin(radians(90.f)); R1[3][1] = 0.f;
		R1[0][2] = 0.f; R1[1][2] = sin(radians(90.f)); R1[2][2] = cos(radians(90.f));  R1[3][2] = 0.f;
		R1[0][3] = 0.f; R1[1][3] = 0.f; R1[2][3] = 0.f; R1[3][3] = 1.f;

		mat4 R2;  // Z�� ���� ȸ��
		R2[0][0] = cos(radians(90.f)); R2[1][0] = -sin(radians(90.f)); R2[2][0] = 0.f; R2[3][0] = 0.f;
		R2[0][1] = sin(radians(90.f)); R2[1][1] = cos(radians(90.f)); R2[2][1] = 0.f; R2[3][1] = 0.f;
		R2[0][2] = 0.f; R2[1][2] = 0.f; R2[2][2] = 1.f; R2[3][2] = 0.f;
		R2[0][3] = 0.f; R2[1][3] = 0.f; R2[2][3] = 0.f; R2[3][3] = 1.f;

		mat4 S1; // 10% ��������� Ȯ��
		S1[0][0] = 0.0106f; S1[1][0] = 0.f; S1[2][0] = 0.f; S1[3][0] = 0.f;
		S1[0][1] = 0.f; S1[1][1] = 0.008f; S1[2][1] = 0.f; S1[3][1] = 0.f;
		S1[0][2] = 0.f; S1[1][2] = 0.f; S1[2][2] = 1.f; S1[3][2] = 0.f;
		S1[0][3] = 0.f; S1[1][3] = 0.f; S1[2][3] = 0.f; S1[3][3] = 1.f;
		gl_Position = S1 * R2 * R1 * T * vec4(vs_position, 1.f);
		fs_color = vs_color;
	}
)";

// fragament shader source code
const GLchar * f_shader_code =
R"(
	#version 330

	in vec4 fs_color;
	
	layout (location = 0) out vec4 color;

	void main()
	{
		color = fs_color;
	}
		
)";

//////////////////////////////////////////////////////////////////////
//// Declare VAO, VBO variables
//////////////////////////////////////////////////////////////////////
GLuint g_vao_id;
GLuint g_vbo_position_id;
GLuint g_vbo_normal_id;
GLuint g_vbo_color_id;
GLuint g_index_buffer_id;

// Index Buffer Object
GLuint index_buffer_id;


//////////////////////////////////////////////////////////////////////
//// Declare callback functions
//////////////////////////////////////////////////////////////////////
void Reshape(int w, int h);
void Display();
void Timer(int value);




int main(int argc, char** argv)
{


	//////////////////////////////////////////////////////////////////////////////////////
	//// 1. freeglut �ʱ�ȭ, ������ �����ϱ�. 
	////    Ref: https://en.wikibooks.org/wiki/OpenGL_Programming/Installation/GLUT
	//////////////////////////////////////////////////////////////////////////////////////

	//// 1.1. �ֱ�ȭ
	glutInit(&argc, argv);
	
	//// 1.2. ������ ��� ����. 
	// ������۸�, RGB �����ӹ���, Depth ���۸� ����Ѵ�.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	//// 1.3. ������ ������ ���� (����, ���� �ȼ� ��).
	glutInitWindowSize(g_window_w, g_window_h);

	//// 1.4. OpenGL Context�� ����� ������ ����.
	glutCreateWindow("Computer Graphics");
	
	//// 1.5. Callback functions 
	// Ref: http://freeglut.sourceforge.net/docs/api.php#GlobalCallback
	//      http://freeglut.sourceforge.net/docs/api.php#WindowCallback
	//      https://www.opengl.org/resources/libraries/glut/spec3/node45.html
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutTimerFunc((unsigned int)(1000 /60), Timer, 0);





	//////////////////////////////////////////////////////////////////////////////////////
	//// 2. GLEW �ʱ�ȭ. 
	////    Ref: http://glew.sourceforge.net/basic.html
	//////////////////////////////////////////////////////////////////////////////////////
	glewExperimental = GL_TRUE;
	if ( glewInit() != GLEW_OK )
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW OK\n";
	}


	


	//////////////////////////////////////////////////////////////////////////////////////
	//// 3. Shader Programs ���
	////    Ref: https://www.khronos.org/opengl/wiki/Shader_Compilation
	//////////////////////////////////////////////////////////////////////////////////////

	//// Vertex shader program
	//// 3.1. Vertex shader ��ü ����.
	v_shader_id = glCreateShader(GL_VERTEX_SHADER);

	//// 3.2. Vertex shader �ҽ��ڵ� �Է�.
	glShaderSource(v_shader_id, 1, &v_shader_source, NULL);

	//// 3.3. Vertex shader �ҽ��ڵ� ������.
	glCompileShader(v_shader_id);

	//// 3.4. ������ ���� �߻��� ���� �޼��� ���.
	{
		GLint compiled;
		glGetShaderiv(v_shader_id, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			GLsizei len;
			glGetShaderiv(v_shader_id, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = new GLchar[len + 1];
			glGetShaderInfoLog(v_shader_id, len, &len, log);
			std::cerr << "Shader compilation failed: " << log << std::endl;
			delete[] log;
			return 0;
		}
	}


	//// Fragment shader program 
	//// 3.5. Fragment shader ��ü ����.
	f_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	//// 3.6. Fragment shader �ҽ��ڵ� �Է�.
	glShaderSource(f_shader_id, 1, &f_shader_code, NULL);

	//// 3.7. Fragment shader �ҽ��ڵ� ������.
	glCompileShader(f_shader_id);

	//// 3.8. ������ ���� �߻��� ���� �޼��� ���.
	{
		GLint compiled;
		glGetShaderiv(f_shader_id, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			GLsizei len;
			glGetShaderiv(f_shader_id, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = new GLchar[len + 1];
			glGetShaderInfoLog(f_shader_id, len, &len, log);
			std::cerr << "Shader compilation failed: " << log << std::endl;
			delete[] log;
			return 0;
		}
	}

	//// Shader Program (vertex shader + fragment shader)
	//// 3.9. Shader Program ��ä ����.
	s_program_id = glCreateProgram();

	//// 3.10. Vertex shader�� Fragment shader�� Shader Program ��ü�� ���.
	glAttachShader(s_program_id, v_shader_id);
	glAttachShader(s_program_id, f_shader_id);

	//// 3.11. Shader Program ��ũ
	glLinkProgram(s_program_id);

	//// 3.12. Shader Program ������
	glUseProgram(s_program_id);






	////////////////////////////////////////////////////////////////////////////////////
	//// 4. OpenGL ����
	//////////////////////////////////////////////////////////////////////////////////////

	glViewport(0, 0, (GLsizei)g_window_w, (GLsizei)g_window_h);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	

	

	
	////////////////////////////////////////////////////////////////////////////////////
	//// 5. VAO, VBO ����
	////    Ref: https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
	///////////////////////////////////////////////////////////////////////////////////

	//// 5.1. VAO ��ü ���� �� ���ε�
	glGenVertexArrays(1, &g_vao_id);
	glBindVertexArray(g_vao_id);

	
	//// 5.2. vertex positions ������ ���� VBO ���� �� ���ε�.
	glGenBuffers(1, &g_vbo_position_id);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo_position_id);

	//// 5.3. vertex positions ������ �Է�.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * g_num_vertices, m_positions, GL_STATIC_DRAW);

	//// 5.4. ���� ���ε��Ǿ��ִ� VBO�� shader program�� ����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);


	//// 5.5. vertex normal vectors ������ ���� VBO ���� �� ���ε�.
	glGenBuffers(1, &g_vbo_normal_id);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo_normal_id);

	//// 5.6. vertex positions ������ �Է�.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * g_num_vertices, m_normals, GL_STATIC_DRAW);

	//// 5.7. ���� ���ε��Ǿ��ִ� VBO�� shader program�� ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	//// 5.8. vertex colors ������ ���� VBO ���� �� ���ε�.
	glGenBuffers(1, &g_vbo_color_id);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo_color_id);

	//// 5.9. vertex positions ������ �Է�.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * g_num_vertices, m_colors, GL_STATIC_DRAW);

	//// 5.10. ���� ���ε��Ǿ��ִ� VBO�� shader program�� ����
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);


	//// 5.11. Index Buffer ��ü ���� �� ������ �Է�
	glGenBuffers(1, &g_index_buffer_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 3 * g_num_triangles, m_indices, GL_STATIC_DRAW);

	


	//// 1.6. freeglut ������ �̺�Ʈ ó�� ����. �����찡 ���������� ���ѷ��� ����.
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutMainLoop();

	//// 5.12. VAO, VBO �޸� ����. 
	glDeleteBuffers(1, &g_vbo_position_id);
	glDeleteBuffers(1, &g_vbo_color_id);
	glDeleteBuffers(1, &g_vbo_normal_id);
	glDeleteVertexArrays(1, &g_vao_id);


	return 0;
}



/**
Display: ������ ȭ���� ������Ʈ �� �ʿ䰡 ���� �� ȣ��Ǵ� callback �Լ�.

������ �� ���� ����� ������ �ϴ� �ڵ�� �� �Լ� ���� �����ؾ��Ѵ�.
�����찡 ó�� ���� ��, ������ ũ�Ⱑ �ٲ� ��, �ٸ� �����쿡 ���� ȭ���� �Ϻ�
�Ǵ� ��ü�� �������ٰ� �ٽ� ��Ÿ�� �� �� �ý����� �ش� ������ ���� �׸��� ����
������Ʈ�� �ʿ��ϴٰ� �Ǵ��ϴ� ��� �ڵ����� ȣ��ȴ�.
���� ȣ���� �ʿ��� ��쿡�� glutPostRedisplay() �Լ��� ȣ���ϸ�ȴ�.

�� �Լ��� �ҽÿ� ����ϰ� ȣ��ȴٴ� ���� ����ϰ�, ������ ���� ��ȭ�� ������
1ȸ�� �ڵ�� �������� �� �Լ� �ۿ� �����ؾ��Ѵ�. Ư�� �޸� �Ҵ�, VAO, VBO ����
���� �ϵ���� ������ �õ��ϴ� �ڵ�� Ư���� ������ ���ٸ� ���� �� �Լ��� ���Խ�Ű��
�ȵȴ�. ���� ���, �޽� ���� �����ϰ� VAO, VBO�� �����ϴ� �κ��� ���� 1ȸ��
�����ϸ�ǹǷ� main() �Լ� �� �ܺο� �����ؾ��Ѵ�. ���ǵ� �޽� ���� ������ ���ۿ�
�׸����� �����ϴ� �ڵ常 �� �Լ��� �����ϸ� �ȴ�.

����, �� �Լ� ������ ���� �޸� �Ҵ��� �ؾ��ϴ� ��찡 �ִٸ� �ش� �޸𸮴� �ݵ��
�� �Լ��� ������ ���� ���� �ؾ��Ѵ�.

ref: https://www.opengl.org/resources/libraries/glut/spec3/node46.html#SECTION00081000000000000000
*/
void Display()
{
	// ��ü ȭ���� �����.
	// glClear�� Display �Լ� ���� �� �κп��� �� ���� ȣ��Ǿ���Ѵ�.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glBindVertexArray(g_vao_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_index_buffer_id);
	glDrawElements(GL_TRIANGLES, g_num_triangles * 3, GL_UNSIGNED_INT, 0);

	
	// flipping the double buffers
	// glutSwapBuffers�� �׻� Display �Լ� ���� �Ʒ� �κп��� �� ���� ȣ��Ǿ���Ѵ�.
	glutSwapBuffers();
}



/**
Timer: ������ �ð� �Ŀ� �ڵ����� ȣ��Ǵ� callback �Լ�.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node64.html#SECTION000819000000000000000
*/
void Timer(int value)
{
	if(glutGetWindow() != 0)
		glutPostRedisplay();

	// glutPostRedisplay�� ������ ���� �ð� �ȿ� ��ü �׸��� �ٽ� �׸� ���� �ý��ۿ� ��û�Ѵ�.
	// ��������� Display() �Լ��� ȣ�� �ȴ�.
	

	// 1/60 �� �Ŀ� Timer �Լ��� �ٽ� ȣ��Ƿη� �Ѵ�.
	// Timer �Լ� �� ������ �ð� �������� �ݺ� ȣ��ǰ��Ͽ�,
	// �ִϸ��̼� ȿ���� ǥ���� �� �ִ�
	glutTimerFunc((unsigned int)(1000 / 60), Timer, 0);
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

	glutPostRedisplay();
}










