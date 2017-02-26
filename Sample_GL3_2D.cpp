#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unistd.h>
#include <cstdlib>



using namespace std;

struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer
;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

GLuint programID;


struct gridkablock
{
  VAO *gridkavao;
  float gridkai;
  float gridkaj;
  int gridkanumber;
  int active;

};
typedef struct gridkablock gridkablock;
gridkablock gridkapiece[101];

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
//    exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
                          0,                  // attribute 0. Vertices
                          3,                  // size (x,y,z)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
                          1,                  // attribute 1. Color
                          3,                  // size (r,g,b)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/

float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;
float lastwala=0.49;
float pointtwocolor; float pointfourninecolor; float pointsevencolor;
float x=0.5,y=1,z=0.5;
float flagx=0;
float flagy=1;
float flagz=0;
float xblock_x=0;
float xblock_y=1.5;
float xblock_z=1.5;
float yblock_x=-1;
float yblock_y=1.5;
float yblock_z=1;
float zblock_x=0.5;
float zblock_y=1.5;
float zblock_z=1;
float camx=-5;
float camy=5;
float camz=6;
int activevariable1 =0 ;
int activevariabl2=0;
float zoom=4;
int checkerarray[105]={0};
unsigned int microseconds;
int level=1;
int score=0;
int bridgecount=0;

/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    
  if (action == GLFW_PRESS)
  {
    if(flagx==0 && flagy==1 && flagz==0) //khada
      { if(key==GLFW_KEY_R)
        {
           yblock_x=-1;
 yblock_y=1.5;
 yblock_z=1;
 flagx=0;flagy=1;flagz=0;
        }
        if(key==GLFW_KEY_RIGHT) 
          {system("aplay -q sound.wav &");
          score++; printf("Score: %d\n",score );

            flagx=1; flagy=0;flagz=0;
            xblock_x=yblock_x+1;
            //xblock_y=yblock_y-0.25;
            xblock_z=yblock_z+0.5;

            for(int test=0; test<81; test++)
            {
              if(xblock_x+1==gridkapiece[test].gridkai && xblock_z-0.5==gridkapiece[test].gridkaj)
              {
                if(gridkapiece[test].active==0)
                {
                  printf("YOU LOSE\n");
                  usleep(10000);
                  flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

                }
                
                else if(gridkapiece[test-1].active==0)
                {
                  printf("YOU LOSE\n");
                  usleep(10000);
                  flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

                }
                else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                  level++;

                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
              }
            }

          }
        else if (key == GLFW_KEY_LEFT)
          {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
            flagx=1; flagy=0; flagz=0;
            xblock_x=yblock_x-1;
            //xblock_y=yblock_y-0.25;
            xblock_z=yblock_z+0.5;

            for(int test=0; test<81; test++)
            {
              if(xblock_x+1==gridkapiece[test].gridkai && xblock_z-0.5==gridkapiece[test].gridkaj)
              {
                if(gridkapiece[test].active==0)
                {
                  printf("YOU LOSE\n");
                  usleep(10000);
                  flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

                }
                
                else if(gridkapiece[test-1].active==0)
                {
                  printf("YOU LOSE\n");
                  usleep(10000);
                  flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

                }
                else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                  level++;
                  
                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
              }
            }

          }
        else if (key==GLFW_KEY_DOWN)
        {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
          flagx=0; flagy=0; flagz=1;
          zblock_x=yblock_x-0.5;
          //zblock_y=yblock_y-0.25;
          zblock_z=yblock_z+2;

          for(int test=0; test<81; test++)
          {
            if(zblock_x+0.5==gridkapiece[test].gridkai && zblock_z==gridkapiece[test].gridkaj)
            {
              if(gridkapiece[test].active==0)
              {
                printf("YOU LOSE\n");
                usleep(10000);
                flagx=0;
                flagy=1;
                flagz=0;
                yblock_x=-1;
                yblock_y=1.5;
                yblock_z=1;
                system("aplay -q out.wav &");
                exit(0);

              }
              else if(gridkapiece[test+1].active==0)
                {
                  printf("YOU LOSE\n");
                  usleep(10000);
                  flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

                }
                else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                  level++;
                  
                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
                
            }
          }
        }
        else if (key==GLFW_KEY_UP)
        {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
          flagx=0; flagy=0; flagz=1;
          zblock_x=yblock_x-0.5;
          //zblock_y=yblock_y-0.25;
          zblock_z=yblock_z-1;

          for(int test=0; test<81; test++)
          {
            if(zblock_x+0.5==gridkapiece[test].gridkai && zblock_z==gridkapiece[test].gridkaj)
            {
              if(gridkapiece[test].active==0)
              {
                printf("YOU LOSE\n");
                usleep(10000);
                flagx=0;
                flagy=1;
                flagz=0;
                yblock_x=-1;
                yblock_y=1.5;
                yblock_z=1;
                system("aplay -q out.wav &");
                exit(0);

              }
              else if(gridkapiece[test+1].active==0)
                {
                  printf("YOU LOSE\n");
                  usleep(10000);
                  flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

                }
                else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                  level++;
                  
                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
            }
          }

        }
        else if(key==GLFW_KEY_T)
        {
          camx=0;
          camy=2;
          camz=0.1;
        }
        else if(key==GLFW_KEY_N)
        {
          camx=-5;
          camy=5;
          camz=6;
        }
        else if (key==GLFW_KEY_B)
        {
          camx=yblock_x;
          camy=yblock_y;
          camz=yblock_z+0.5;
        }
        else if(key==GLFW_KEY_I)
        {
          zoom-=0.5;
        }
        else if(key==GLFW_KEY_O)
        {
          zoom+=0.5;
        }
        else{
          printf("Invalid Key\n");
        }







      }

      else if(flagx==1 && flagy==0 && flagz==0) //leta
      { if(key==GLFW_KEY_R)
        {
           yblock_x=-1;
 yblock_y=1.5;
 yblock_z=1;
 flagx=0;flagy=1;flagz=0;
        }
        if (key==GLFW_KEY_LEFT)
        {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
          flagx=0; flagy=1;flagz=0;
          yblock_x=xblock_x-1;
          //yblock_y=xblock_y+0.25;
          yblock_z=xblock_z-0.5;

          for(int test=0; test<81; test++)
          {
            if(yblock_x==gridkapiece[test].gridkai && yblock_z==gridkapiece[test].gridkaj)
            {
              if(gridkapiece[test].active==0)
              {
                printf("YOU LOSE\n");
                usleep(10000);
                flagx=0;;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

              }
              else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                  level++;
                  
                }
                else if(gridkapiece[test].active==3)
                {
                  printf("YOU LOSE\n");
                  system("aplay -q out.wav &");
                  exit(0);
                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
            }
          }
        }
        else if(key == GLFW_KEY_RIGHT)
        {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
          flagx=0; flagy=1; flagz=0;
          yblock_x=xblock_x+2;
         // yblock_y=xblock_y+0.25;
          yblock_z=xblock_z-0.5;

          for(int test=0; test<81; test++)
          {
            if(yblock_x==gridkapiece[test].gridkai && yblock_z==gridkapiece[test].gridkaj)
            {
              if(gridkapiece[test].active==0)
              {
                printf("YOU LOSE\n");
                usleep(10000);
                flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

              }
              else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                  level++;
                  
                }
                else if(gridkapiece[test].active==3)
                {
                  printf("YOU LOSE\n");
                  system("aplay -q out.wav &");
                  exit(0);
                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
            }
          }
        }
        else if (key==GLFW_KEY_UP)
        {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
          flagx=1; flagy=0;flagz=0;
          xblock_z=xblock_z-1;

          for(int test=0; test<81; test++)
          {
            if(xblock_x+1==gridkapiece[test].gridkai && xblock_z-0.5==gridkapiece[test].gridkaj)
            {
              if(gridkapiece[test].active==0)
              {
                printf("YOU LOSE\n");
                usleep(10000);
                flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

              }
              else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                  level++;
                  
                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
   
            }
          }

        }
         else if (key==GLFW_KEY_DOWN)
        {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
          flagx=1; flagy=0;flagz=0;
          xblock_z=xblock_z+1;

          for(int test=0; test<81; test++)
          {
            if(xblock_x+1==gridkapiece[test].gridkai && xblock_z-0.5==gridkapiece[test].gridkaj)
            {
              if(gridkapiece[test].active==0)
              {
                printf("YOU LOSE\n");
                usleep(10000);
                flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

              }
              else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                  level++;
                  
                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
         
            }
          }

        }
        else if(key==GLFW_KEY_T)
        {
          camx=0.1;
          camy=10;
          camz=0.1;
        }
        else if(key==GLFW_KEY_N)
        {
          camx=-5;
          camy=5;
          camz=6;
        }
         else if (key==GLFW_KEY_B)
        {
          camx=xblock_x+1;
          camy=xblock_y;
          camz=xblock_z-0.5;
        }
        else if(key==GLFW_KEY_I)
        {
          zoom-=0.5;
        }
        else if(key==GLFW_KEY_O)
        {
          zoom+=0.5;
        }
        
      }
      else if (flagx==0 && flagy==0 && flagz==1) //BEDA
      {if(key==GLFW_KEY_R)
        {
           yblock_x=-1;
 yblock_y=1.5;
 yblock_z=1;
 flagx=0;flagy=1;flagz=0;
        }
        if(key==GLFW_KEY_UP)
        {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
          flagx=0; flagy=1; flagz=0;
          yblock_x=zblock_x+0.50;
          //yblock_y=zblock_y+0.25;
          yblock_z=zblock_z-2;

          for(int test=0; test<81; test++)
          {
            if(yblock_x==gridkapiece[test].gridkai && yblock_z==gridkapiece[test].gridkaj)
            {
              if(gridkapiece[test].active==0)
              {
                printf("YOU LOSE\n");
                usleep(10000);
                flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

              }
              else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                  level++;
                  
                }
                else if(gridkapiece[test].active==3)
                {
                  printf("YOU LOSE\n");
                  system("aplay -q out.wav &");
                  exit(0);
                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
            }
          }
        }
        else if(key==GLFW_KEY_DOWN)
        {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
          flagx=0; flagy=1; flagz=0;
          yblock_x=zblock_x+0.5;
          //yblock_y=zblock_y+0.25;
          yblock_z=zblock_z+1;

          for(int test=0; test<81; test++)
          {
            if(yblock_x==gridkapiece[test].gridkai && yblock_z==gridkapiece[test].gridkaj)
            {
              if(gridkapiece[test].active==0)
              {
                printf("YOU LOSE\n");
                usleep(10000);
                flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

              }
              else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");
                  score+=10; printf("Score: %d\n",score );
                  level++;
                  
                }
                else if(gridkapiece[test].active==3)
                {
                  printf("YOU LOSE\n");
                  system("aplay -q out.wav &");
                  exit(0);
                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
            }
          }
        }
        else if (key==GLFW_KEY_RIGHT)
        {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
          flagx=0; flagy=0; flagz=1;
          zblock_x+=1;

          for(int test=0; test<81; test++)
          {
            if(zblock_x+0.5==gridkapiece[test].gridkai && zblock_z==gridkapiece[test].gridkaj)
            {
              if(gridkapiece[test].active==0)
              {
                printf("YOU LOSE\n");
                usleep(10000);
                flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");
                  exit(0);

              }
              else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                  level++;
                }
                 else if(gridkapiece[test].active==5)
                {
                  bridgecount++;
                }
           
               
            }
          }
        }
        else if (key==GLFW_KEY_LEFT)
        {system("aplay -q sound.wav &");score++; printf("Score: %d\n",score );
          flagx=0; flagy=0; flagz=1;
          zblock_x-=1;

          for(int test=0; test<81; test++)
          {
            if(zblock_x+0.5==gridkapiece[test].gridkai && zblock_z==gridkapiece[test].gridkaj)
            {
              if(gridkapiece[test].active==0)
              {
                printf("YOU LOSE\n");
                usleep(10000);
                flagx=0;
                  flagy=1;
                  flagz=0;
                  yblock_x=-1;
                  yblock_y=1.5;
                  yblock_z=1;
                  system("aplay -q out.wav &");

              }
              else if(gridkapiece[test].active==2)
                {
                  printf("YOU WIN\n");
                  system("aplay -q win.wav &");score+=10; printf("Score: %d\n",score );
                }
                else if(gridkapiece[test].active==5)
                {
                  bridgecount++;printf("bridgecount bada\n");
                }
         
                
            }
          }
        }
       else if(key==GLFW_KEY_T)
        {
          camx=0.1;
          camy=10;
          camz=0.1;
        }
        else if(key==GLFW_KEY_N)
        {
          camx=-5;
          camy=5;
          camz=6;
        }
         else if (key==GLFW_KEY_B)
        {
          camx=zblock_x+0.5;
          camy=zblock_y;
          camz=zblock_z+1;
        }
        else if(key==GLFW_KEY_I)
        {
          zoom-=0.5;
        }
        else if(key==GLFW_KEY_O)
        {
          zoom+=0.5;
        }
      }
  } 
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
            quit(window);
            break;
		default:
			break;
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_RELEASE)
                triangle_rot_dir *= -1;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_RELEASE) {
                rectangle_rot_dir *= -1;
            }
            break;
        default:
            break;
    }
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
    int fbwidth=width, fbheight=height;
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
     is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.49, 500.0); */
	// Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    //Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
    Matrices.projection = glm::ortho(-zoom, zoom, -zoom, zoom, 0.1f, 500.0f);
}

VAO *triangle, *rectangleX, *rectangleY, *rectangleZ;


// Creates the triangle object used in this sample code

// Creates the rectangle object used in this sample code
void createRectangleX ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {


     -1, -0.5, -0.5, 
     -1, -0.5, +0.5, 
     -1, +0.5, +0.5, 

     +1, +0.5, -0.5, 
     -1, -0.5, -0.5, 
     -1, +0.5, -0.5,

     +1, -0.5, +0.5, 
     -1, -0.5, -0.5, 
     +1, -0.5, -0.5,

     +1, +0.5, -0.5,
     +1, -0.5, -0.5, 
     -1, -0.5, -0.5, 

     -1, -0.5, -0.5, 
     -1, +0.5, +0.5, 
     -1, +0.5, -0.5, 

     +1, -0.5, +0.5, 
     -1, -0.5, +0.5, 
     -1, -0.5, -0.5, 

     -1, +0.5, +0.5, 
     -1, -0.5, +0.5, 
     +1, -0.5, +0.5,

     +1, +0.5, +0.5,
     +1, -0.5, -0.5,
     +1, +0.5, -0.5, 

     +1, -0.5, -0.5, 
     +1, +0.5, +0.5, 
     +1, -0.5, +0.5, 

     +1, +0.5, +0.5, 
     +1, +0.5, -0.5,
     -1, +0.5, -0.5, 

     +1, +0.5, +0.5, 
     -1, +0.5, -0.5, 
     -1, +0.5, +0.5, 

     +1, +0.5, +0.5, 
     -1, +0.5, +0.5, 
     +1, -0.5, +0.5, 
  

  };

  static const GLfloat color_buffer_data [] = {
    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,
    
    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,
    
    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0
  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  rectangleX = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}


void createRectangleY ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {


     -0.5, -1, -0.5, 
     -0.5, -1, +0.5, 
     -0.5, +1, +0.5, 

     +0.5, +1, -0.5, 
     -0.5, -1, -0.5, 
     -0.5, +1, -0.5,

     +0.5, -1, +0.5, 
     -0.5, -1, -0.5, 
     +0.5, -1, -0.5,

     +0.5, +1, -0.5,
     +0.5, -1, -0.5, 
     -0.5, -1, -0.5, 

     -0.5, -1, -0.5, 
     -0.5, +1, +0.5, 
     -0.5, +1, -0.5, 

     +0.5, -1, +0.5, 
     -0.5, -1, +0.5, 
     -0.5, -1, -0.5, 

     -0.5, +1, +0.5, 
     -0.5, -1, +0.5, 
     +0.5, -1, +0.5,

     +0.5, +1, +0.5,
     +0.5, -1, -0.5,
     +0.5, +1, -0.5, 

     +0.5, -1, -0.5, 
     +0.5, +1, +0.5, 
     +0.5, -1, +0.5, 

     +0.5, +1, +0.5, 
     +0.5, +1, -0.5,
     -0.5, +1, -0.5, 

     +0.5, +1, +0.5, 
     -0.5, +1, -0.5, 
     -0.5, +1, +0.5, 

     +0.5, +1, +0.5, 
     -0.5, +1, +0.5, 
     +0.5, -1, +0.5, 
  

  };

  static const GLfloat color_buffer_data [] = {
    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,
    
    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,
    
    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0
  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  rectangleY = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}




void createRectangleZ ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {


     -0.5, -0.5, -1, 
     -0.5, -0.5, +1, 
     -0.5, +0.5, +1, 

     +0.5, +0.5, -1, 
     -0.5, -0.5, -1, 
     -0.5, +0.5, -1,

     +0.5, -0.5, +1, 
     -0.5, -0.5, -1, 
     +0.5, -0.5, -1,

     +0.5, +0.5, -1,
     +0.5, -0.5, -1, 
     -0.5, -0.5, -1, 

     -0.5, -0.5, -1, 
     -0.5, +0.5, +1, 
     -0.5, +0.5, -1, 

     +0.5, -0.5, +1, 
     -0.5, -0.5, +1, 
     -0.5, -0.5, -1, 

     -0.5, +0.5, +1, 
     -0.5, -0.5, +1, 
     +0.5, -0.5, +1,

     +0.5, +0.5, +1,
     +0.5, -0.5, -1,
     +0.5, +0.5, -1, 

     +0.5, -0.5, -1, 
     +0.5, +0.5, +1, 
     +0.5, -0.5, +1, 

     +0.5, +0.5, +1, 
     +0.5, +0.5, -1,
     -0.5, +0.5, -1, 

     +0.5, +0.5, +1, 
     -0.5, +0.5, -1, 
     -0.5, +0.5, +1, 

     +0.5, +0.5, +1, 
     -0.5, +0.5, +1, 
     +0.5, -0.5, +1, 
  

  };

  static const GLfloat color_buffer_data [] = {
    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,
    
    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,
    
    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0,

    1,1,0,
    1,1,0,
    1,1,0
  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  rectangleZ = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}


VAO* createGrid(float pointtwocolor, float pointfourninecolor, float pointsevencolor, float lastwala)
{
  GLfloat vertex_buffer_data[] = 
  {
    -0.5,-0.5,-0.5,
    0.5,-0.5,-0.5,
    0.5,0.5,0.5,

    0.5,0.5,-0.5,
    -0.5,0.5,-0.5,
    -0.5,-0.5,-0.5,

    0.5,0.5,-0.5,
    -0.5,0.5,-0.5,
    -0.5,0.5,0.5,

    -0.5,0.5,0.5,
    0.5,0.5,0.5,
    0.5,0.5,-0.5,

    0.5,0.5,0.5,
    0.5,0.5,-0.5,
    0.5,-0.5,-0.5,

    0.5,-0.5,-0.5,
    0.5,-0.5,0.5,
    0.5,0.5,0.5,

    -0.5,-0.5,-0.5,
    0.5,-0.5,-0.5,
    0.5,-0.5,0.5,

    0.5,-0.5,0.5,
    -0.5,-0.5,0.5,
    -0.5,-0.5,-0.5,

    0.5,0.5,0.5,
    0.5,-0.5,0.5,
    -0.5,-0.5,0.5,

    -0.5,-0.5,0.5,
    -0.5,0.5,0.5,
    0.5,0.5,0.5,

    -0.5,-0.5,-0.5,
    -0.5,0.5,-0.5,
    -0.5,0.5,0.5,

    -0.5,0.5,0.5,
    -0.5,-0.5,0.5,
    -0.5,-0.5,-0.5
  };

  GLfloat color_buffer_data[]=
  {
    pointtwocolor, pointfourninecolor,lastwala,
    pointtwocolor, pointfourninecolor,lastwala,
    pointtwocolor, pointfourninecolor,lastwala,

    pointtwocolor, pointfourninecolor,lastwala,
    pointtwocolor, pointfourninecolor,lastwala,
    pointtwocolor, pointfourninecolor,lastwala,

    pointfourninecolor,pointfourninecolor,lastwala,
    pointfourninecolor,pointfourninecolor,lastwala,
    pointfourninecolor,pointfourninecolor,lastwala,

    pointfourninecolor,pointfourninecolor,lastwala,
    pointfourninecolor,pointfourninecolor,lastwala,
    pointfourninecolor,pointfourninecolor,lastwala,

    pointtwocolor,pointfourninecolor,lastwala,
    pointtwocolor,pointfourninecolor,lastwala,
    pointtwocolor,pointfourninecolor,lastwala,

    pointtwocolor,pointfourninecolor,lastwala,
    pointtwocolor,pointfourninecolor,lastwala,
    pointtwocolor,pointfourninecolor,lastwala,

    pointfourninecolor,pointtwocolor,lastwala,
    pointfourninecolor,pointtwocolor,lastwala,
    pointfourninecolor,pointtwocolor,lastwala,

    pointfourninecolor,pointtwocolor,lastwala,
    pointfourninecolor,pointtwocolor,lastwala,
    pointfourninecolor,pointtwocolor,lastwala,

    pointfourninecolor,pointsevencolor,lastwala,
    pointfourninecolor,pointsevencolor,lastwala,
    pointfourninecolor,pointsevencolor,lastwala,

    pointfourninecolor,pointsevencolor,lastwala,
    pointfourninecolor,pointsevencolor,lastwala,
    pointfourninecolor,pointsevencolor,lastwala,

    pointfourninecolor,pointfourninecolor,lastwala,
    pointfourninecolor,pointfourninecolor,lastwala,
    pointfourninecolor,pointfourninecolor,lastwala,

    pointfourninecolor,pointfourninecolor,lastwala,
    pointfourninecolor,pointfourninecolor,lastwala,
    pointfourninecolor,pointfourninecolor,lastwala,
  };

  return create3DObject(GL_TRIANGLES,36, vertex_buffer_data,color_buffer_data,GL_FILL);
}







float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw ()
{
  // clear the color and depth in the frame buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use the loaded shader program
  // Don't change unless you know what you are doing
  glUseProgram (programID);

  // Eye - Location of camera. Don't change unless you are sure!!
  glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  // Target - Where is the camera looking at.  Don't change unless you are sure!!
  glm::vec3 target (0,0,0);
  // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
  glm::vec3 up (1, 1, 0);

  // Compute Camera matrix (view)
  // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
  //  Don't change unless you are sure!!
  Matrices.view = glm::lookAt(glm::vec3(camx,camy, camz), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

  // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
  //  Don't change unless you are sure!!
  glm::mat4 VP = Matrices.projection * Matrices.view;

  // Send our transformation to the currently bound shader, in the "MVP" uniform
  // For each model you render, since the MVP will be different (at least the M part)
  //  Don't change unless you are sure!!
  glm::mat4 MVP;	// MVP = Projection * View * Model

  // Load identity to model matrix
  //YE HATAYA THAAAMatrices.model = glm::mat4(1.0f);

  /* Render your scene */

 

  //  Don't change unless you are sure!!
  //YE HATAYA THAglUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  //draw3DObject(triangle);

  // Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
  // glPopMatrix ();


  int index=0;
  int oindex=0;
  for (int i = 4.5; i >= -4.5 ; )
  {
    
    for (int j = 4.5; j >= -4.5; )
    {
      
      Matrices.model = glm::mat4(1.0f);

      glm::mat4 translateBlock = glm::translate (glm::vec3(i, 0, j));        // glTranslatef
      //glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
      Matrices.model =translateBlock;
      MVP = VP * Matrices.model;
      glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

      gridkapiece[index].gridkanumber = index;
      gridkapiece[index].gridkai = i;
      gridkapiece[index].gridkaj = j;
     
        draw3DObject(gridkapiece[index].gridkavao);
        //printf("block %d at x= %d and z= %d \n",gridkapiece[index].gridkanumber,gridkapiece[index].gridkai,gridkapiece[index].gridkaj );
      
      index++;
      j-=1;
    }
    i-=1;
  }




 // rectangle = create3DObject(GL_TRIANGLES,36, vertex_buffer_data,color_buffer_data,GL_FILL);
  //createRectangle (x,y,z);

if(flagx==1 && flagy==0 && flagz==0){
                          Matrices.model = glm::mat4(1.0f);


                          glm::mat4 translateRectangleX = glm::translate (glm::vec3(xblock_x, xblock_y, xblock_z));        // glTranslatef
                          //glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
                          Matrices.model *= (translateRectangleX );
                          MVP = VP * Matrices.model;
                          glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

                          // draw3DObject draws the VAO given to it using current MVP matrix
                          draw3DObject(rectangleX);
                        }
else if (flagx==0 && flagy==1 && flagz==0){

                                       Matrices.model = glm::mat4(1.0f);


                                      glm::mat4 translateRectangleY = glm::translate (glm::vec3(yblock_x, yblock_y, yblock_z));        // glTranslatef
                                      //glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
                                      Matrices.model *= (translateRectangleY );
                                      MVP = VP * Matrices.model;
                                      glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

                                      // draw3DObject draws the VAO given to it using current MVP matrix
                                      draw3DObject(rectangleY);}

else if (flagx==0 && flagy==0 && flagz==1){

                                                    Matrices.model = glm::mat4(1.0f);


                                                    glm::mat4 translateRectangleZ = glm::translate (glm::vec3(zblock_x, zblock_y, zblock_z));        // glTranslatef
                                                    //glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
                                                    Matrices.model *= (translateRectangleZ );
                                                    MVP = VP * Matrices.model;
                                                    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

                                                    // draw3DObject draws the VAO given to it using current MVP matrix
                                                    draw3DObject(rectangleZ);}

  // Increment angles
  float increments = 1;

  //camera_rotation_angle++; // Simulating camera rotation
  triangle_rotation = triangle_rotation + increments*triangle_rot_dir*triangle_rot_status;
  rectangle_rotation = rectangle_rotation + increments*rectangle_rot_dir*rectangle_rot_status;



  
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
//        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window) {
        glfwTerminate();
//        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
     is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

    /* Register function to handle mouse click */
    glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks

    return window;
}


/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
    /* Objects should be created before any other gl function and shaders */
	// Create the models
  for(int i=0;i<105;i++)
  {
    //grid[i]=createGrid(lastwala);
    //lastwala+=0.01;
    //grid[i]=createGrid(0.2,0.49,0.7,0.0);

   // printf("create with %d %d %d %d \n",g1,g2,g3,g4 );
    if(level==1){
    
    if (i==11 || i==12 || i==13 || i==14 || i==15 || i==21 || i==22  || i==24 || i==28 || i==29 || i==30 || i==31 || i==32 || i==33 || i==38 || i==39 || i==40 || i==41 ||  i== 48 || i==49 || i==50 || i==51|| i== 58 || i==59)
    {
      if(i%2==0)
      {
        gridkapiece[i].gridkavao=createGrid(0.2,0.49, 0.7,0.0);
         // printf("%d\n",i );
      //gridkapiece[i].active=1;
      activevariable1=1;
      gridkapiece[i].active=1;
      }
      else
      {
        gridkapiece[i].gridkavao=createGrid(0.2,0.49, 0.7,0.7);
         // printf("%d\n",i );
      //gridkapiece[i].active=1;
      activevariable1=1;
      gridkapiece[i].active=1;
      }
      //printf("block %d is active and x is %d and z is %d \n",i,gridkapiece[i].gridkai, gridkapiece[i].gridkaj );

    }
    else if(i==23)
    {
     gridkapiece[i].gridkavao=createGrid(0,0,0,1); 
         // printf("%d\n",i );
      checkerarray[i]=1;
     // gridkapiece[i].active=0;
      activevariabl2=1;
      gridkapiece[i].active=2; 
    }

    else if(i==42)
    {
      gridkapiece[i].gridkavao=createGrid(0,1,0,0); 
         // printf("%d\n",i );
      checkerarray[i]=1;
     // gridkapiece[i].active=0;
      activevariabl2=1;
      gridkapiece[i].active=3; 
    }
    else  {
      gridkapiece[i].gridkavao=createGrid(0,0,0,0); 
         // printf("%d\n",i );
      checkerarray[i]=1;
     // gridkapiece[i].active=0;
      activevariabl2=1;
      gridkapiece[i].active=0; 
    }
    }
    if(level==2)
    {

    if (i==11 || i==12 || i==13 || i==22   || i==28 || i==29 || i==30 || i==31 || i==38 || i==39 ||  i== 48 || i==49 || i==50 || i== 58 || i==59)
    {
      if(i%2==0)
      {
        gridkapiece[i].gridkavao=createGrid(0.2,0.49, 0.7,0.0);
         // printf("%d\n",i );
      //gridkapiece[i].active=1;
      activevariable1=1;
      gridkapiece[i].active=1;
      }
      else
      {
        gridkapiece[i].gridkavao=createGrid(0.2,0.49, 0.7,0.7);
         // printf("%d\n",i );
      //gridkapiece[i].active=1;
      activevariable1=1;
      gridkapiece[i].active=1;
      }
      //printf("block %d is active and x is %d and z is %d \n",i,gridkapiece[i].gridkai, gridkapiece[i].gridkaj );

    }
    else if(i==21)
    {
     gridkapiece[i].gridkavao=createGrid(0,0,0,1); 
         // printf("%d\n",i );
      checkerarray[i]=1;
     // gridkapiece[i].active=0;
      activevariabl2=1;
      gridkapiece[i].active=2; 
    }

    else if(i==42)
    {
      gridkapiece[i].gridkavao=createGrid(0,1,0,0); 
         // printf("%d\n",i );
      checkerarray[i]=1;
     // gridkapiece[i].active=0;
      activevariabl2=1;
      gridkapiece[i].active=3; 
    }
    else  {
      gridkapiece[i].gridkavao=createGrid(0,0,0,0); 
         // printf("%d\n",i );
      checkerarray[i]=1;
     // gridkapiece[i].active=0;
      activevariabl2=1;
      gridkapiece[i].active=0; 
    }
    }
     if(level==3)
    {

    if (i == 65 || i==66 || i==64|| i==56 ||i==55 || i==57 || i==47 || i==46 || i==48 || i==38 || i==39  || i==31 || i==22 || i==40  )
    {
      if(i%2==0)
      {
        gridkapiece[i].gridkavao=createGrid(0.2,0.49, 0.7,0.0);
         // printf("%d\n",i );
      //gridkapiece[i].active=1;
      activevariable1=1;
      gridkapiece[i].active=1;
      }
      else
      {
        gridkapiece[i].gridkavao=createGrid(0.2,0.49, 0.7,0.7);
         // printf("%d\n",i );
      //gridkapiece[i].active=1;
      activevariable1=1;
      gridkapiece[i].active=1;
      }
      //printf("block %d is active and x is %d and z is %d \n",i,gridkapiece[i].gridkai, gridkapiece[i].gridkaj );

    }
    else if(i==14)
    {
     gridkapiece[i].gridkavao=createGrid(0,0,0,1); 
         // printf("%d\n",i );
      checkerarray[i]=1;
     // gridkapiece[i].active=0;
      activevariabl2=1;
      gridkapiece[i].active=2; 
    }
    else if(i==41)
    {
      gridkapiece[i].gridkavao=createGrid(0,1,0,0); 
         // printf("%d\n",i );
      checkerarray[i]=1;
     // gridkapiece[i].active=0;
      activevariabl2=1;
      gridkapiece[i].active=5; 
    }

    
    else  {
      gridkapiece[i].gridkavao=createGrid(0,0,0,0); 
         // printf("%d\n",i );
      checkerarray[i]=1;
     // gridkapiece[i].active=0;
      activevariabl2=1;
      gridkapiece[i].active=0; 
    }

    if(bridgecount==1)
    {
      if (i==32 || i==23)
      {
        if(i%2==0)
      {
        gridkapiece[i].gridkavao=createGrid(0.2,0.49, 0.7,0.0);
         // printf("%d\n",i );
      //gridkapiece[i].active=1;
      activevariable1=1;
      gridkapiece[i].active=1;
      }
      else
      {
        gridkapiece[i].gridkavao=createGrid(0.2,0.49, 0.7,0.7);
         // printf("%d\n",i );
      //gridkapiece[i].active=1;
      activevariable1=1;
      gridkapiece[i].active=1;
      }
      }
    }
    }


    /*else
    {
      gridkapiece[i].gridkavao=createGrid(0.2, 0.49, 0.7, 0.7);
          //printf("%d\n",i );
      //gridkapiece[i].active=1;
      activevariable1=1;
      gridkapiece[i].active=1;
     // printf("block %d is active and x is %d and z is %d \n",i,gridkapiece[i].gridkai, gridkapiece[i].gridkaj );

    }*/


  }


	createRectangleX ();
	 createRectangleY ();
   createRectangleZ();

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");

	
	reshapeWindow (window, width, height);

    // Background color of the scene
	glClearColor (0,0,0,0); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

}

int main (int argc, char** argv)
{
	int width = 600;
	int height = 600;
  int initcounter=1;
  int bridgecountcheck=0;
  system("aplay intro.wav &");


    GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);

    double last_update_time = glfwGetTime(), current_time;

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {

        // OpenGL Draw commands
        draw();
        if(level==2)
        {
              if (initcounter==1){ initcounter++;
              	
            initGL (window, width, height);
            flagx=0;
            flagy=1;
            flagz=0;
            yblock_x=-1;
 yblock_y=1.5;
 yblock_z=1;

          }

        }
         if(level==3)
        {
              if (initcounter==2){ initcounter++;

            initGL (window, width, height);
            flagx=0;
            flagy=1;
            flagz=0;
            yblock_x=-3;
 yblock_y=1.5;
 yblock_z=2;

          }

        }
        if(level==4)
        {
        	exit(0);
        }
       
        if(bridgecount==1)

        {
          bridgecountcheck++;
          if(bridgecountcheck==1){
                      initGL (window, width, height);}

        }


        // Swap Frame Buffer in double buffering
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();
        for (int i = 0; i < 105; ++i)
        {
          /* code */
         // printf("block %d is %d with x= %d and y=%d \n",i,gridkapiece[i].active,gridkapiece[i].gridkai, gridkapiece[i].gridkaj );
        }

        // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
        current_time = glfwGetTime(); // Time in seconds
        if ((current_time - last_update_time) >= 0.49) { // atleast 0.5s elapsed since last frame
            // do something every 0.49 seconds ..
            last_update_time = current_time;
        }
    }



    glfwTerminate();
//    exit(EXIT_SUCCESS);
}
