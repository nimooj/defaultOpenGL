#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace glm;

#include "Human.h"
#include "Camera.h"
#include "Skinning.h"

Human human;
Camera camera;
Skinning skinning;

GLFWwindow* window; 
mat4 projectionMatrix;
mat4 viewMatrix;
mat4 modelMatrix;
/*** OPENGL 3.3 ***/
GLuint loadShaders(const char*, const char*);
GLuint vao, joint_vao;
GLuint ebo, joint_ebo;
GLuint bodyShaderProgram;
GLuint jointShaderProgram;

vector<vec3> position;
vector<vec3> normal;
vector<int> indexing;
vector<vec3> color;

vector<int> jointIndex;

vec3 pos;
vec3 dir;
vec3 up;

int posing = 0;

vector<vec3> colorArray;

GLuint loadShaders(const char*, const char*);
void defineBodyVAO(GLuint&);
void defineJointVAO(GLuint&);
void keyboardInput();

// Define main function
int main()
{
  for (int i = 0; i < JointNum; i++) {
    float r = rand() / double(RAND_MAX);
    float g = rand() / double(RAND_MAX);
    float b = rand() / double(RAND_MAX);
    colorArray.push_back(vec3(r, g, b));
  }

  camera = Camera();

  human = Human();
  skinning = Skinning(human);
  skinning.segment();
  skinning.paintWeight();

  // Initialize GLFW
  glfwInit();

  // Define version and compatibility settings
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create OpenGL window and context
  window = glfwCreateWindow(1024, 1024, "HumanOBJ", NULL, NULL);
  glfwMakeContextCurrent(window);

  // Check for window creation failure
  if (!window)
  {
    // Terminate GLFW
    glfwTerminate();
    return 0;
  }

  // Initialize GLEW
  glewExperimental = GL_TRUE; 
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


  glEnable(GL_BLEND);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);

  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Event loop
  while(!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
  {
    // Clear the screen to black
    glClearColor(1.0f, 1.0f, 1.0f, 0.5f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboardInput();
    defineBodyVAO(vao);

    glBegin(GL_POINTS);
    glColor3f(0, 0, 0);
    glVertex3f(3, 0, 2);
    glEnd();

    pos = camera.cameraPosition;
    dir = camera.cameraFront;
    up = camera.cameraUp;

    vec3 color = vec3(1.0f, 1.0f, 0.0f);
    vec3 lightColor = vec3(1, 1, 1);
    vec3 lightPos = vec3(0.0f, 10.0f, -15.0f);
    vec3 cameraPos = pos;

    /*** Draw Body ***/
    glUseProgram(bodyShaderProgram);
    GLint color_attribute = glGetUniformLocation(bodyShaderProgram, "objColor");
    glUniform3fv(color_attribute, 1, &color[0]);
    GLint lightColor_attribute = glGetUniformLocation(bodyShaderProgram, "lightColor");
    glUniform3fv(lightColor_attribute, 1, &lightColor[0]);
    GLint lightPos_attribute = glGetUniformLocation(bodyShaderProgram, "lightPos");
    glUniform3fv(lightPos_attribute, 1, &lightPos[0]);
    GLint cameraPos_attribute = glGetUniformLocation(bodyShaderProgram, "cameraPos");
    glUniform3fv(cameraPos_attribute, 1, &cameraPos[0]);

    //mat4 projectionMatrix = perspective((float)radians(45.0f), width2 / (float)height2, 0.1f, 300.0f);
    projectionMatrix = ortho(-10.0f, 10.0f, -10.0f, 10.f, 0.0f, 100.0f);
    viewMatrix = lookAt(pos, dir, up);
    modelMatrix = mat4(1.0f);

    GLuint projectionID = glGetUniformLocation(bodyShaderProgram, "projection");
    glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix[0][0]);
    GLuint viewID = glGetUniformLocation(bodyShaderProgram, "view");
    glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
    GLuint modelID = glGetUniformLocation(bodyShaderProgram, "model");
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &modelMatrix[0][0]);

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, indexing.size(), GL_UNSIGNED_INT, 0);

    //defineJointVAO(joint_vao);

    //glUseProgram(jointShaderProgram);
    //projectionID = glGetUniformLocation(jointShaderProgram, "projection");
    //glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix[0][0]);
    //viewID = glGetUniformLocation(jointShaderProgram, "view");
    //glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
    //modelID = glGetUniformLocation(jointShaderProgram, "model");
    //glUniformMatrix4fv(modelID, 1, GL_FALSE, &modelMatrix[0][0]);

    //glPointSize(5);
    //glBindVertexArray(joint_vao);
    //glDrawArrays(GL_POINTS, 0, human.joints.size());
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, joint_ebo);
    //glDrawElements(GL_LINES, jointIndex.size(), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Terminate GLFW
  glfwTerminate(); 
  return 0;
}

void defineBodyVAO(GLuint& vao) {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  bodyShaderProgram = loadShaders("shaders/BodyVertexShader.vertexshader", "shaders/BodyFragmentShader.fragmentshader");

  position.clear();
  normal.clear();
  indexing.clear();
  color.clear();

  for (int i = 0; i < human.vertices.size(); i++) {
    Vertex bv = &human.vertices[i];
    position.push_back(vec3(bv.x, bv.y, bv.z));

    color.push_back( vec3(0.8, 0.8, 0.8) );
  }

  //for (int i = 0; i < JointNum; i++) {
    //for (int j = 0; j < skinning.weightSegment[i].size(); j++){
      //color[skinning.weightSegment[i][j]] = colorArray[i];
    //}
  //}

  for (int i = 0; i < skinning.armRSegment.size(); i++) {
    color[skinning.armRSegment[i]] = vec3(1, 0, 0);

    for (int j = 0; j < human.vertices[skinning.armRSegment[i]].jointsRelated.size(); j++) {
      if (human.vertices[skinning.armRSegment[i]].jointsRelated[j] == Joint_shoulderR) {
        float weightVal = human.vertices[skinning.armRSegment[i]].jointWeights[j];
        if (weightVal > 0.5 && weightVal < 1)
          color[skinning.armRSegment[i]] = vec3(1, 1, 0);
        else if (weightVal <= 0.5)
          color[skinning.armRSegment[i]] = vec3(0, 0, 1);
      }
    }
  }

  for (int i = 0; i < human.normals.size(); i++) {
    Vertex nv = human.normals[i];
    normal.push_back(vec3(nv.x, nv.y, nv.z));
  }

  for (int i = 0; i < human.meshes.size(); i++) {
    indexing.push_back(human.meshes[i].index1 - 1);
    indexing.push_back(human.meshes[i].index2 - 1);
    indexing.push_back(human.meshes[i].index3 - 1);
  }

  // Position
  GLuint position_vbo;
  glGenBuffers(1, &position_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
  glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(vec3), &position[0], GL_STATIC_DRAW);

  GLint position_attribute = glGetAttribLocation(bodyShaderProgram, "position");
  glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_attribute);

  // Normal
  GLuint normal_vbo;
  glGenBuffers(1, &normal_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
  glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(vec3), &normal[0], GL_STATIC_DRAW);

  GLint normal_attribute = glGetAttribLocation(bodyShaderProgram, "normal");
  glVertexAttribPointer(normal_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normal_attribute);

  // Indexing
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexing.size() * sizeof(int), &indexing[0], GL_STATIC_DRAW);

  GLuint color_vbo;
	glGenBuffers(1, &color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(vec3), &color[0], GL_STATIC_DRAW);
	GLint color_attribute = glGetAttribLocation(bodyShaderProgram, "color");
	glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(color_attribute);


  glBindVertexArray(0);
}

void defineJointVAO(GLuint& joint_vao) {
	glGenVertexArrays(1, &joint_vao);
	glBindVertexArray(joint_vao);

	jointShaderProgram = loadShaders("shaders/JointVertexShader.vertexshader", "shaders/JointFragmentShader.fragmentshader");

	vector<vec3> joints;

  jointIndex.clear();

	for ( int i = 0; i < human.joints.size(); i++ ) {
		Vertex v = human.joints[i];
		joints.push_back(vec3(v.x, v.y, v.z));
	}

  jointIndex.push_back(0);
  jointIndex.push_back(1);

  jointIndex.push_back(1);
  jointIndex.push_back(2);

  jointIndex.push_back(2);
  jointIndex.push_back(3);

  jointIndex.push_back(3);
  jointIndex.push_back(4);

  jointIndex.push_back(1);
  jointIndex.push_back(5);

  jointIndex.push_back(5);
  jointIndex.push_back(6);

  jointIndex.push_back(6);
  jointIndex.push_back(7);

  jointIndex.push_back(7);
  jointIndex.push_back(8);

  jointIndex.push_back(8);
  jointIndex.push_back(9);

  jointIndex.push_back(9);
  jointIndex.push_back(10);

  jointIndex.push_back(6);
  jointIndex.push_back(11);

  jointIndex.push_back(11);
  jointIndex.push_back(12);

  jointIndex.push_back(12);
  jointIndex.push_back(13);

  jointIndex.push_back(13);
  jointIndex.push_back(14);

  jointIndex.push_back(1);
  jointIndex.push_back(15);

  jointIndex.push_back(15);
  jointIndex.push_back(16);

  jointIndex.push_back(16);
  jointIndex.push_back(17);

	if (!joints.empty()) {
		GLuint position_vbo;
		glGenBuffers(1, &position_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
		glBufferData(GL_ARRAY_BUFFER, joints.size() * sizeof(vec3), &joints[0], GL_STATIC_DRAW);

		GLint position_attribute = glGetAttribLocation(jointShaderProgram, "position");
		glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(position_attribute);

    glGenBuffers(1, &joint_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, joint_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, jointIndex.size() * sizeof(int), &jointIndex[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
}

GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath) {
	// Create shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read vertex shader from file
	string vertexShaderSource;
	ifstream vertexShaderStream(vertexFilePath, ios::in);
	if (vertexShaderStream.is_open() ) {
		stringstream sstr;
		sstr << vertexShaderStream.rdbuf();
		vertexShaderSource = sstr.str();
		vertexShaderStream.close();
	}
	else {
		return -1;
	}

	string fragmentShaderSource;
	ifstream fragmentShaderStream(fragmentFilePath, ios::in);
	if (fragmentShaderStream.is_open() ) {
		stringstream sstr;
		sstr << fragmentShaderStream.rdbuf();
		fragmentShaderSource = sstr.str();
		fragmentShaderStream.close();
	}
	else {
		return -1;
	}

	GLint success;
	GLchar infoLog[512];

	// Compile Vertex shader
	char const * vertexShaderPointer = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &vertexShaderPointer, NULL);
	glCompileShader(vertexShader);

	// Check vertex Shader
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	}

	// Compile Fragment shader
	const char * fragmentShaderPointer = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderPointer, NULL);
	glCompileShader(fragmentShader);

	// Check fragment Shader
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	}

	// Linking to program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check program
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);

	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

float degree = 0;

void keyboardInput() {
  float cameraSpeed = 0.05f;
  if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
    camera.ViewFront();
  }
  if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
    camera.ViewBack();
  }
  if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
    //camera.ViewLeft();
    degree += 10;
    camera.cameraPosition.x = 10 *cos(degree*M_PI/180);
    camera.cameraPosition.z = 10 * sin(degree*M_PI/180);
  }
  if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
    degree -= 10;
    //camera.ViewRight();
    camera.cameraPosition.x = 10 *cos(degree*M_PI/180);
    camera.cameraPosition.z = 10 * sin(degree*M_PI/180);
  }

  if ( glfwGetKey( window, GLFW_KEY_R ) == GLFW_PRESS ) {
    // Rightt Arm lifting 
    if (posing == 0) {
      skinning.rotateRA(Joint_shoulderR, -40);
      posing = 1;
    }
  }
  if ( glfwGetKey( window, GLFW_KEY_L ) == GLFW_PRESS ) {
    // Redo Right Arm
    if (posing == 1) {
      skinning.rotateRA(Joint_shoulderR, 40);
      posing = 0;
    }
  }

  if ( glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS ) {
    human.exportToOBJ();
  }
}

