#include <iostream>

#define GLEW_STATIC
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
// #define STBI_ONLY_TGA
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "include/Shader.h"
#include "include/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include "include/Mbox.h"
#include "include/texture.hpp"
#include "include/Humanoid.h"
#include "include/House.h"
#include <ctime>
#include <fstream>
#include <vector>

using namespace std;

bool keys[1024];

GLint activeHuman = 0;
House *house=NULL;

GLint PointShader;
// constants
GLint height = 720;
GLint width = 1200;
GLboolean isBeizer = GL_FALSE;

// cube
GLuint cubeVAO, cubeVBO;

// line
GLuint lineVAO, lineVBO;

// time between two frames
GLfloat lastFrame = 0.0f;	// last frame time

GLfloat lastX = 400, lastY = 300;
GLfloat deltaTime = 0.0f;
GLboolean firstMouse = true;
glm::vec3 isLight = glm::vec3(3.0f);
glm::vec3 isLight1 = glm::vec3(3.0f);

vector<glm::vec3> pathlist;

Camera camera(glm::vec3(0.0f, 33.0f, 70.0f));
Mbox *mbox1 = NULL, *mbox2 = NULL;
// human 1
Humanoid *human = NULL;
BodyParts *lArm = NULL;
BodyParts *lHand = NULL;
BodyParts *rArm = NULL;
BodyParts *rHand = NULL;
BodyParts *lThigh = NULL;
BodyParts *lLeg = NULL;
BodyParts *rThigh = NULL;
BodyParts *rLeg = NULL;

// human 2
Humanoid *human1 = NULL;
BodyParts *lArm1 = NULL;
BodyParts *lHand1 = NULL;
BodyParts *rArm1 = NULL;
BodyParts *rHand1 = NULL;
BodyParts *lThigh1 = NULL;
BodyParts *lLeg1 = NULL;
BodyParts *rThigh1 = NULL;
BodyParts *rLeg1 = NULL;

glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view;// = glm::mat4(1.0f)

ofstream outfile;
ifstream infile;

GLboolean recordMode = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	// music box
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		mbox1->isOpen = GL_TRUE;
		human->isshift = GL_TRUE;
		human1->isshift = GL_TRUE;
	}
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		mbox1->isOpen = GL_FALSE;
		human->isshift = GL_FALSE;
		human1->isshift = GL_FALSE;
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		isBeizer = !isBeizer;
	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		int lights = 0, lamp = 0;
		if(isLight.x>0.0f)
			lights = 1;
		if(isLight.y>0.0f)
			lamp = 1;
		cout <<lights<<" "<<lamp<<" "<<house->openWindow<<" "<<house->openDoor<<" "<<mbox1->openAngle<<" "<<human1->root->cen<<"\n";

		outfile <<lights<<" "<<lamp<<" "<<house->openWindow<<" "<<house->openDoor<<" "<<mbox1->openAngle<<" "<<human1->root->cen<<"\n";
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		outfile.open ("keyframes.txt", ios::app);
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		recordMode = true;
		infile.open("keyframes.txt");
	}

	// open and closing sliding door
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
		house->isOpen = GL_TRUE;
	if (key == GLFW_KEY_RIGHT_CONTROL && action == GLFW_PRESS)
		house->isOpen = GL_FALSE;
	// open and close door
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		house->isOpen1 = GL_TRUE;
	if (key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_PRESS)
		house->isOpen1 = GL_FALSE;

	// lights on/off
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (isLight.x > 0.0f) isLight.x=0.0f;
		else isLight.x = 3.0f;
		// cout << glm::to_string(isLight) << endl;
	}
	if (key == GLFW_KEY_RIGHT_ALT && action == GLFW_PRESS)
	{
		if (isLight.y > 0.0f) isLight.y=0.0f;
		else isLight.y = 3.0f;
		// cout << glm::to_string(isLight) << endl;
	}

	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS)
	{	
		int n = 4*800*600;
		GLubyte pixels[n];// = new GLubyte[n];
		glReadPixels(0,0,800,600, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		int num_bytes_written = 0;
		unsigned char ustore[n];// = new unsigned char[n];
		for(unsigned int x=0; x<800; x++)
			for(unsigned int y=0;y<600; y++)
			{
				unsigned int index=0, uindex=0;
				index=(4*800*y) + (4*x);
				uindex=(4*800*(600-y)) + (4*x);
				ustore[uindex] = (unsigned char)(pixels[index]);
				ustore[uindex+1]=(unsigned char)(pixels[index+1]);
				ustore[uindex+2]=(unsigned char)(pixels[index+2]);
				ustore[uindex+3]=(unsigned char)(pixels[index+3]);
			}

		time_t tt;
	    // Applying time() 
	    time (&tt); 
	    // Using localtime() 
	    tm TM  = *localtime(&tt);
	    int year    = TM.tm_year + 1900;
		int month   = TM.tm_mon ;
		int day     = TM.tm_mday;
		int hour    = TM.tm_hour;
		int mins    = TM.tm_min ;
		int secs    = TM.tm_sec ;

		string temp = "screenshot/"+to_string(year)+to_string(month+1)+to_string(day)+to_string(hour)+to_string(mins)+to_string(secs)+".jpg";
		num_bytes_written = stbi_write_jpg( temp.c_str(), 800, 600, 4, (void*)ustore, 90);
	    
	    // cout << to_string(year)+to_string(month+1)+to_string(day)+to_string(hour)+to_string(mins)+to_string(secs)+".jpg" << endl;
	}
	if (action == GLFW_PRESS)
		keys[key]=true;
	else if (action == GLFW_RELEASE)
		keys[key]=false;
}

void drawCube()
{
	GLfloat vertices[108] = {
		-0.5f, -0.5f, -0.5f, 
		0.5f,  0.5f, -0.5f, 
		0.5f, -0.5f, -0.5f, 
		0.5f,  0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 
		-0.5f,  0.5f, -0.5f, 

		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f, 
		0.5f, -0.5f, -0.5f, 
		0.5f,  0.5f, -0.5f, 
		0.5f, -0.5f, -0.5f, 
		0.5f,  0.5f,  0.5f, 
		0.5f, -0.5f,  0.5f, 

		-0.5f, -0.5f, -0.5f, 
		0.5f, -0.5f, -0.5f, 
		0.5f, -0.5f,  0.5f, 
		0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f, -0.5f, 

		-0.5f,  0.5f, -0.5f,  
		0.5f,  0.5f,  0.5f,  
		0.5f,  0.5f, -0.5f,  
		0.5f,  0.5f,  0.5f,  
		-0.5f,  0.5f, -0.5f,  
		-0.5f,  0.5f,  0.5f
	};
	glGenBuffers(1, &cubeVBO);
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void renderCube(glm::vec3 center)
{
	glUniformMatrix4fv(glGetUniformLocation(PointShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(PointShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, center);
	model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
	glUniformMatrix4fv(glGetUniformLocation(PointShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

GLfloat data[300];

void renderLine()
{
	glUniformMatrix4fv(glGetUniformLocation(PointShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(PointShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glm::mat4 model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(PointShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINE_STRIP, 0, 100);
	glBindVertexArray(0);
}

void drawPoints()
{
	for (int i = 0; i < pathlist.size(); ++i)
	{
		renderCube(glm::vec3(pathlist[i].x,pathlist[i].y,pathlist[i].z));
	}
	if (pathlist.size()==4)
	{
		int pointCount = 0;
		GLfloat vertices1[12];
		while (pointCount < pathlist.size())
		{
			int count = 0;
			for (int j = 0; j < 12; j++)
			{
				vertices1[j++] = (pathlist[pointCount].x);
				vertices1[j++] = (pathlist[pointCount].y);
				vertices1[j] = (pathlist[pointCount].z);
				pointCount++;
			}
			for (float a = 0; a < 1.0f; a += (0.01f)) {
				data[count++] = (1 - a)*(1 - a)*(1 - a)*vertices1[0] + 3 * a*(1 - a)*(1 - a)*vertices1[3] + 3 * a*a*(1 - a)*vertices1[6] + a * a*a*vertices1[9];
				data[count++] = (1 - a)*(1 - a)*(1 - a)*vertices1[1] + 3 * a*(1 - a)*(1 - a)*vertices1[4] + 3 * a*a*(1 - a)*vertices1[7] + a * a*a*vertices1[10];
				data[count++] = (1 - a)*(1 - a)*(1 - a)*vertices1[2] + 3 * a*(1 - a)*(1 - a)*vertices1[5] + 3 * a*a*(1 - a)*vertices1[8] + a * a*a*vertices1[11];	
			}
			glGenBuffers(1, &lineVBO);
			glGenVertexArrays(1, &lineVAO);
			glBindVertexArray(lineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		renderLine();
	}
}



void mousebutton_callback(GLFWwindow* window, int button, int action, int mods) {
	    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS ) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        float x,y,z;
        x = float((xpos / width) * 2 - 1);
        y = float(1 - (ypos / height) * 2);
        glReadPixels(xpos, ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
        glm::mat4 inv_projection = glm::inverse(projection*view);
        //glm::mat4 inv_view = glm::inverse(camera.GetViewMatrix());

        glm::vec4 pr = inv_projection*glm::vec4(x,y,-z,1.0f);
        pr = pr/pr.w;
        glm::vec4 pr1 = inv_projection*glm::vec4(x,y,z,1.0f);
        pr1 = pr1/pr1.w;
        // cout << glm::to_string(pr) << endl;
        // cout << glm::to_string(pr1) << endl;
        pr = pr+30*glm::normalize(pr1-pr);
       // glm::vec4 pos = inv_view*pr;
        
        // cout << x << " " << y << " " << z << "\n";
        // cout << glm::to_string(pr) << endl;
        pathlist.push_back(glm::vec3(pr.x,pr.y,pr.z));
    }
	// camera.ProcessMouseMovement(xoffset, yoffset);	
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void doMovement() {
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_Z]) 
		camera.ProcessKeyboard(UP, deltaTime);
	if (keys[GLFW_KEY_X]) 
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (keys[GLFW_KEY_Q])  // rotate anti
		camera.ProcessKeyboard(CLOCK, deltaTime);
	if (keys[GLFW_KEY_E])  // rotate anti
		camera.ProcessKeyboard(ANTI, deltaTime);

}

void printVersion()
{
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	GLint nrVertexAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrVertexAttributes);
	cout << "Maximum number of vertex attributes are supported -> " << nrVertexAttributes << "\n";

	cout << "renderer -> " << renderer << "\n";
	cout << "version -> " << version << "\n";
}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
}

void initCallBacks(GLFWwindow* window)
{
	if (window == nullptr) {
		cout << "failed to create glfw window\n";
		glfwTerminate();
		// return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	// glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window,mousebutton_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
}

int main() 
{
	Init();

	// creating a context
	GLFWwindow* window = glfwCreateWindow(width, height, "shader", nullptr, nullptr);
	initCallBacks(window);

	if (glewInit() != GLEW_OK) {
		cout << "failed to initialise glew\n";
		return -1;
	}

	printVersion();
	glViewport(0, 0, width, height);

	// setup opengl options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);

	mbox1 = new Mbox();

	// land texture
	GLuint landTex = LoadTexture("textures/grass1.jpg");
	GLuint wallTex = LoadTexture("textures/wal.jpg");
	GLuint windowTex = LoadTexture("textures/blending_transparent_window.png", GL_TRUE);
	GLuint roofTex = LoadTexture("textures/roof1.jpg");

	Shader shaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");
	Shader pointShader("shaders/pvertex.glsl", "shaders/pfragment.glsl");
	PointShader = pointShader.Program;
	Shader lampProgram("shaders/lvertex.glsl", "shaders/lfragment.glsl");
	Shader lampProgram1("shaders/lvertex.glsl", "shaders/lfragment1.glsl");

	GLuint texture = LoadTexture("textures/te.jpg");
	GLuint texture1 = LoadTexture("textures/test.bmp");
	GLuint velvet = LoadTexture("textures/velvet.jpg");
	GLuint woodTex = LoadTexture("textures/wood.jpeg");
	


	// house
	house = new House();
	house->floorTex = landTex;
	house->wallTex = wallTex;
	house->windowTex = windowTex;
	house->roofTex = roofTex;
	house->wood = woodTex;
	house->sofa = velvet;

	mbox1->texture = texture1;
	mbox1->texture1 = texture;
	mbox1->texture2 = velvet;
	mbox1->innerTex = woodTex;

	// human textures
	GLuint texture3 = LoadTexture("textures/boyf1.jpg"), texture4 = LoadTexture("textures/boyfb.jpg"), neckTex = LoadTexture("textures/neck.jpg");
	GLuint texture5 = LoadTexture("textures/girlf1.jpg"), texture6 = LoadTexture("textures/girlfb.jpg"), pantTex = LoadTexture("textures/pant.jpg");
	GLuint shirtTex = LoadTexture("textures/shirt.jpg");
	GLuint weddingTex = LoadTexture("textures/wedding4.jpg");

	// human 1
	// cout << texture << texture1 << texture2 << "\n";
	human = new Humanoid(glm::vec3(-(2.25*house->length+6*house->thickness) / 2.0, -house->height/8.0f+house->thickness*5.5f, -6*house->breadth/8.0f), shaderProgram.Program);
	human->root->texture = shirtTex;
	BodyParts *neck = new BodyParts(0.25f, 0.3f, 0.25f, human->root->center1 + glm::vec3(0.0f, (human->root->height / 2.0) + 0.1f, 0.0f), shaderProgram.Program, Neck, neckTex);
	BodyParts *head = new BodyParts(0.5f, 0.55f, 0.5f, glm::vec3(0.0f), shaderProgram.Program, Head, -1);
	BodyParts *headf = new BodyParts(0.5f, 0.005f, 0.5f, glm::vec3(0.0f), shaderProgram.Program, Plate, texture3);
	BodyParts *headb = new BodyParts(0.5f, 0.005f, 0.5f, glm::vec3(0.0f), shaderProgram.Program, Plate, texture4);
	BodyParts *headl = new BodyParts(0.01f, 0.55f, 0.5f, glm::vec3(0.0f), shaderProgram.Program, Plate, texture4);
	BodyParts *headr = new BodyParts(0.01f, 0.55f, 0.5f, glm::vec3(0.0f), shaderProgram.Program, Plate, texture4);
	BodyParts *headt = new BodyParts(0.5f, 0.55f, 0.005f, glm::vec3(0.0f), shaderProgram.Program, Plate, texture4);
	head->addBodyPart(headf, glm::vec3(0.0f, 0.0f, head->breadth / 2.0f - 0.001f));
	head->addBodyPart(headb, glm::vec3(0.0f, 0.0f, -(head->breadth / 2.0f + 0.001f)));
	head->addBodyPart(headl, glm::vec3(-(head->length / 2.0f + 0.001f), 0.0f, 0.0f));
	head->addBodyPart(headr, glm::vec3((head->length / 2.0f + 0.001f), 0.0f, 0.0f));
	head->addBodyPart(headt, glm::vec3(0.0f, (head->height / 2.0f + 0.001f), 0.0f));
	neck->addBodyPart(head, glm::vec3(0.0f, 0.4f, 0.0f));
	human->addBodyPart(neck, glm::vec3(0.0f));

	lArm = new BodyParts(0.75f, 0.25f, 0.25f, human->root->center1 + glm::vec3(-human->root->length, human->root->breadth-0.2f, 0.0f), shaderProgram.Program, LeftArm, shirtTex);
	lHand = new BodyParts(0.8f, 0.2f, 0.2f, glm::vec3(0.0f), shaderProgram.Program, LeftHand, neckTex);
	lArm->addBodyPart(lHand, glm::vec3(-0.8f, 0.0f, 0.0f));
	human->addBodyPart(lArm, glm::vec3(0.0f));

	rArm = new BodyParts(0.75f, 0.25f, 0.25f, human->root->center1 + glm::vec3(human->root->length, human->root->breadth-0.2f, 0.0f), shaderProgram.Program, RightArm, shirtTex);
	rHand = new BodyParts(0.8f, 0.2f, 0.2f, glm::vec3(0.0f), shaderProgram.Program, RightHand, neckTex);
	rArm->addBodyPart(rHand, glm::vec3(0.8f, 0.0f, 0.0f));
	human->addBodyPart(rArm, glm::vec3(0.0f));

	lThigh = new BodyParts(0.35f, 0.45f, 0.85f, human->root->center1 + glm::vec3(-0.25f, -human->root->height+0.3f, 0.0f), shaderProgram.Program, LeftThigh, pantTex);
	lLeg = new BodyParts(0.25f, 0.25f, 0.9f, glm::vec3(0.0f), shaderProgram.Program, LeftLeg, pantTex);
	lThigh->addBodyPart(lLeg, glm::vec3(0.0f, -0.8f, 0.0f));
	human->addBodyPart(lThigh, glm::vec3(0.0f));

	rThigh = new BodyParts(0.35f, 0.45f, 0.85f, human->root->center1 + glm::vec3(0.25f, -human->root->height+0.3f, 0.0f), shaderProgram.Program, RightThigh, pantTex);
	rLeg = new BodyParts(0.25f, 0.25f, 0.9f, glm::vec3(0.0f), shaderProgram.Program, RightLeg, pantTex);
	rThigh->addBodyPart(rLeg, glm::vec3(0.0f, -0.8f, 0.0f));
	human->addBodyPart(rThigh, glm::vec3(0.0f));

	// human 2
	human1 = new Humanoid(glm::vec3(-(2.25*house->length - 8*house->thickness) / 2.0, -house->height/8.0f+house->thickness*5.5f, -6*house->breadth/8.0f), shaderProgram.Program);
	human1->root->texture = weddingTex;
	BodyParts *neck1 = new BodyParts(0.25f, 0.3f, 0.25f, human1->root->center1 + glm::vec3(0.0f, (human1->root->height / 2.0) + 0.1f, 0.0f), shaderProgram.Program, Neck, neckTex);
	BodyParts *head1 = new BodyParts(0.5f, 0.55f, 0.5f, glm::vec3(0.0f), shaderProgram.Program, Head, -1);
	BodyParts *headf1 = new BodyParts(0.5f, 0.005f, 0.5f, glm::vec3(0.0f), shaderProgram.Program, Plate, texture5);
	BodyParts *headb1 = new BodyParts(0.5f, 0.005f, 0.5f, glm::vec3(0.0f), shaderProgram.Program, Plate, texture6);
	BodyParts *headl1 = new BodyParts(0.005f, 0.55f, 0.5f, glm::vec3(0.0f), shaderProgram.Program, Plate, texture6);
	BodyParts *headr1 = new BodyParts(0.005f, 0.55f, 0.5f, glm::vec3(0.0f), shaderProgram.Program, Plate, texture6);
	BodyParts *headt1 = new BodyParts(0.5f, 0.55f, 0.005f, glm::vec3(0.0f), shaderProgram.Program, Plate, texture6);
	head1->addBodyPart(headf1, glm::vec3(0.0f, 0.0f, head1->breadth / 2.0f - 0.001f));
	head1->addBodyPart(headb1, glm::vec3(0.0f, 0.0f, -(head1->breadth / 2.0f + 0.001f)));
	head1->addBodyPart(headl1, glm::vec3(-(head1->length / 2.0f + 0.001f), 0.0f, 0.0f));
	head1->addBodyPart(headr1, glm::vec3((head1->length / 2.0f + 0.001f), 0.0f, 0.0f));
	head1->addBodyPart(headt1, glm::vec3(0.0f, (head1->height / 2.0f + 0.001f), 0.0f));
	neck1->addBodyPart(head1, glm::vec3(0.0f, 0.4f, 0.0f));
	human1->addBodyPart(neck1, glm::vec3(0.0f));

	lArm1 = new BodyParts(0.75f, 0.25f, 0.25f, human1->root->center1 + glm::vec3(-human1->root->length, human1->root->breadth-0.2f, 0.0f), shaderProgram.Program, LeftArm, neckTex);
	lHand1 = new BodyParts(0.8f, 0.2f, 0.2f, glm::vec3(0.0f), shaderProgram.Program, LeftHand, neckTex);
	lArm1->addBodyPart(lHand1, glm::vec3(-0.8f, 0.0f, 0.0f));
	human1->addBodyPart(lArm1, glm::vec3(0.0f));

	rArm1 = new BodyParts(0.75f, 0.25f, 0.25f, human1->root->center1 + glm::vec3(human1->root->length, human1->root->breadth-0.2f, 0.0f), shaderProgram.Program, RightArm, neckTex);
	rHand1 = new BodyParts(0.8f, 0.2f, 0.2f, glm::vec3(0.0f), shaderProgram.Program, RightHand, neckTex);
	rArm1->addBodyPart(rHand1, glm::vec3(0.8f, 0.0f, 0.0f));
	human1->addBodyPart(rArm1, glm::vec3(0.0f));

	lThigh1 = new BodyParts(0.35f, 0.45f, 0.85f, human1->root->center1 + glm::vec3(-0.25f, -human1->root->height+0.3f, 0.0f), shaderProgram.Program, LeftThigh, weddingTex);
	lLeg1 = new BodyParts(0.25f, 0.25f, 0.9f, glm::vec3(0.0f), shaderProgram.Program, LeftLeg, neckTex);
	lThigh1->addBodyPart(lLeg1, glm::vec3(0.0f, -0.8f, 0.0f));
	human1->addBodyPart(lThigh1, glm::vec3(0.0f));

	rThigh1 = new BodyParts(0.35f, 0.45f, 0.85f, human1->root->center1 + glm::vec3(0.25f, -human1->root->height+0.3f, 0.0f), shaderProgram.Program, RightThigh, weddingTex);
	rLeg1 = new BodyParts(0.25f, 0.25f, 0.9f, glm::vec3(0.0f), shaderProgram.Program, RightLeg, neckTex);
	rThigh1->addBodyPart(rLeg1, glm::vec3(0.0f, -0.8f, 0.0f));
	human1->addBodyPart(rThigh1, glm::vec3(0.0f));

	// animation
	for (int i = 0; i < 5; ++i)
	{
		lHand->swingHand(1);
		lArm->swingHand(1);
		rArm->swingHand(1);
		rArm->swingHand(4);
	}
	for (int i = 0; i < 3; ++i)
	{
		lArm->swingHand(4);
		lArm->swingHand(1);
		rHand->swingHand(1);
	}
	rHand->swingHand(1);
	for (int i = 0; i < 5; ++i)
	{
		lHand1->swingHand(1);
		lArm1->swingHand(1);
		rArm1->swingHand(1);
		rArm1->swingHand(4);
	}
	for (int i = 0; i < 3; ++i)
	{
		lArm1->swingHand(4);
		lArm1->swingHand(1);
		rHand1->swingHand(1);
	}
	rHand1->swingHand(1);

	

	GLint count = 0, count1=0, count2=0;

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.0f,  15.2f,  0.0f),
		glm::vec3(-house->length, 15.2f, -house->breadth / 2.0f),
		glm::vec3(-house->length/2.0f, (house->height-house->thickness)/2.0f-house->length/10.0f, -house->breadth + house->thickness*10)
	};

	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	pointShader.Use();
	glUniform3fv(glGetUniformLocation(pointShader.Program, "vertexColor"), 1, &color[0]);
	drawCube();

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		glfwPollEvents();
		doMovement();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		 view = camera.GetViewMatrix();

		//;
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 300.0f);

		lampProgram.Use();
		glUniformMatrix4fv(glGetUniformLocation(lampProgram.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lampProgram.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(lampProgram.Program, "isLight"), isLight.x, isLight.y, isLight.z);
		house->createLight(lampProgram.Program);
		lampProgram1.Use();
		glUniformMatrix4fv(glGetUniformLocation(lampProgram1.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lampProgram1.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(lampProgram1.Program, "isLight"), isLight.x, isLight.y, isLight.z);
		house->createLight1(lampProgram1.Program);

		shaderProgram.Use();
		glUniform3f(glGetUniformLocation(shaderProgram.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform3f(glGetUniformLocation(shaderProgram.Program, "isLight"), isLight.x, isLight.y, isLight.z);
		// Set lights properties
		// Directional light
		glUniform3f(glGetUniformLocation(shaderProgram.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(shaderProgram.Program, "dirLight.ambient"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(shaderProgram.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(shaderProgram.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
		if (isLight.x > 0.0f)
		{
			// Point light 1
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].diffuse"), 50.8f, 50.8f, 50.8f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].quadratic"), 0.032);
			// Point light 2
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].diffuse"), 50.8f, 50.8f, 50.8f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].quadratic"), 0.032);
		}
		else 
		{
			// Point light 1
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].specular"), 0.0f, 0.0f, 0.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[0].quadratic"), 0.032);
			// Point light 2
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].specular"), 0.0f, 0.0f, 0.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[1].quadratic"), 0.032);
		}
		// lamp light
		if (isLight.y > 0.0f)
		{
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].ambient"), 0.05f, 0.09f, 0.1f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].diffuse"), 5.1f, 9.0f, 10.0f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].quadratic"), 0.032);
		}
		else
		{
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].specular"), 0.0f, 0.0f, 0.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shaderProgram.Program, "pointLights[2].quadratic"), 0.032);
		}

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(glGetUniformLocation(shaderProgram.Program, "material.diffuse"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram.Program, "material.specular"), 1);
		glUniform1f(glGetUniformLocation(shaderProgram.Program, "material.shininess"), 32.0f);
		mbox1->createBox(glm::vec3(-(2.25*house->length - house->thickness) / 2.0, -house->height/8.0f+house->thickness*7.0f, -6*house->breadth/8.0f), shaderProgram.Program);
		human->displayHumanoid();
		human1->displayHumanoid();
		house->createHouse(glm::vec3(0.0f), shaderProgram.Program);
		count++;
		count1++;
		if (count1%2==0 && pathlist.size()==4 && isBeizer) 
		{
			count1=0;
			camera.Front = -glm::vec3(camera.Position.x+(2.25*house->length - house->thickness) / 2.0, camera.Position.y+house->height/8.0f-house->thickness*7.0f, camera.Position.z+6*house->breadth/8.0f);
			camera.Position.x = data[3*count2];
			camera.Position.y = data[3*count2+1];
			camera.Position.z = data[3*count2+2];
			count2++;
			if(count2%100==0)
			{
				count2=0;
				isBeizer=GL_FALSE;
			}
		}
		if (count == 5) 
		{
			count = 0;
			if (human1->isDone) human1->rotCen();
			if (human->isDone) human->rotCen();
		}

		pointShader.Use();
		if (pathlist.size()) 
		{
			drawPoints();
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}