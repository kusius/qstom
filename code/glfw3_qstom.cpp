/*
 * Plattform independent window context creation code.
 * Depends on glfw
 * Platform-dependent parts that are used and
 * would need to be implemented:
 *            Type         |     Unix           |        Windows
 * Window context creation |     xcb            |        HWND
 * High resolution time    |     clock_gettime  |    QueryPerformanceCounter
 * Input                   |
 * Audio                   |
 */
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define internal static
#define global_variable static
#define local_persist static

//TODO:File IO in Windows
#ifdef QSTOM_UNIX
#include "unix_qstom.h"
#endif
#include "glfw3_qstom.h"
#include "math_qstom.h"

global_variable int WindowWidth = 800;
global_variable int WindowHeight = 600;

internal void
PrintOpenGLInfo(void)
{
    printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
           glGetString(GL_SHADING_LANGUAGE_VERSION));
}

internal int
GetShaderLogAndStatus(GLint Shader, GLint ShaderType)
{
    GLint Success = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
    if(Success == GL_FALSE)
    {

        //GLint InfoLogLength;
        const char *ShaderTypeString = (ShaderType==GL_VERTEX_SHADER)
            ? "Vertex"
            : "Fragment";
        GLchar InfoLog[256] = {};
        //glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
        glGetShaderInfoLog(Shader, 256,
                           NULL, &InfoLog[0]);
        //TODO: Proper logging in file
        fprintf(stderr, "[%s] %s\n", ShaderTypeString, InfoLog);
    }
    return(Success);
}

internal opengl_program
CompileAndLinkOpenGLProgram(platform_file_data* VertexFile,
                            platform_file_data* FragmentFile)
{
    GLint Success = 1;
    GLint StringSize = 0;
    // TODO: Handle linking errors
    opengl_program Program = {};
    Program.VertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Program.VertexID, 1, &(VertexFile->Memory),NULL);
    glCompileShader(Program.VertexID);
    int VertexStatus = 0, FragmentStatus = 0;

    Program.FragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(Program.FragmentID, 1, &(FragmentFile->Memory),NULL);
    glCompileShader(Program.FragmentID);

    VertexStatus = GetShaderLogAndStatus(Program.VertexID,
                                         GL_VERTEX_SHADER);
    FragmentStatus = GetShaderLogAndStatus(Program.FragmentID,
                                         GL_FRAGMENT_SHADER);
    if(!VertexStatus || !FragmentStatus)
    {
        glDeleteProgram(Program.VertexID);
        glDeleteProgram(Program.FragmentID);
        Program = {};
        return(Program);
    }

    Program.ID = glCreateProgram();
    glAttachShader(Program.ID, Program.VertexID);
    glAttachShader(Program.ID, Program.FragmentID);
    glLinkProgram(Program.ID);
    glDeleteShader(Program.VertexID);
    glDeleteShader(Program.FragmentID);
    return Program;
}

global_variable float VertexData[] =
{
-0.5, -0.5, 0.5,
0.5, -0.5, 0.5,
0.5, 0.5, 0.5,
-0.5, -0.5, 0.5,
-0.5, 0.5, 0.5,
0.5, 0.5, 0.5
};

global_variable float CubeData[] =
{
/*Front*/
-1.0, -1.0, 0.5,
1.0, -1.0, 0.5,
1.0, 1.0, 0.5,
-1.0, -1.0, 0.5,
-1.0, 1.0, 0.5,
1.0, 1.0, 0.5
/*Back*/
/*Top*/
/*Bottom*/
/*Left*/
/*Right*/
};

global_variable mat4 Model = MAT4_IDENTITY;
global_variable mat4 View = MAT4_IDENTITY;
global_variable mat4 Projection = MAT4_IDENTITY;
global_variable GLuint vao, vbo;
global_variable GLuint PositionAttrib, ColorAttrib;
global_variable GLuint ProjectionUniform, ModelUniform, ViewUniform;

internal void
PassStaticRenderData(void)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(PositionAttrib);
    glVertexAttribPointer(PositionAttrib, 2, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(ColorAttrib);
    glVertexAttribPointer(ColorAttrib, 1, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat),
                          (void*)(2 * sizeof(GLfloat)));

    return;
}

internal void
UpdateRenderData(void)
{
    glUniformMatrix4fv(ProjectionUniform, 1, GL_FALSE,
                       &Projection[0][0]);
    glUniformMatrix4fv(ModelUniform, 1, GL_FALSE,
                       &Model[0][0]);
    glUniformMatrix4fv(ViewUniform, 1, GL_FALSE,
                       &View[0][0]);
    return;
}

internal void
InitRenderObjects(opengl_program Program)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    PositionAttrib = glGetAttribLocation(Program.ID,
                                         "position");
    ColorAttrib = glGetAttribLocation(Program.ID,
                                      "alpha");
    ProjectionUniform = glGetUniformLocation(Program.ID,
                                            "projection");
    ModelUniform = glGetUniformLocation(Program.ID,
                                            "model");
    ViewUniform = glGetUniformLocation(Program.ID,
                                            "view");
    return;
}

int main(void)
{
    GLFWwindow *WindowHandle;

    if(!glfwInit())
        return(-1);
   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    WindowHandle = glfwCreateWindow(WindowWidth, WindowHeight,
                                    "QStom", NULL, NULL);
    if(!WindowHandle)
    {
        glfwTerminate();
        return(-1);
    }
    glfwMakeContextCurrent(WindowHandle);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    //if(!gladLoadGL())
    {
        glfwDestroyWindow(WindowHandle);
        glfwTerminate();
        return(-1);
    }

    //TODO: If defined debug
    PrintOpenGLInfo();

    // NOTE: Read compile and link a vertex and fragment shader
    platform_file_data VertexFile = {}, FragmentFile = {};
    PlatformReadFile("shaders/simple.vert", &VertexFile);
    PlatformReadFile("shaders/simple.frag", &FragmentFile);
    opengl_program OpenGLProgram =
        CompileAndLinkOpenGLProgram(&VertexFile,
                                    &FragmentFile);
    PlatformFreeFileMemory(&VertexFile);
    PlatformFreeFileMemory(&FragmentFile);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(OpenGLProgram.ID);

    vec3 Axis = {1.0, 0.0, 0.0};

    InitRenderObjects(OpenGLProgram);


    RotateTransform(Model, -45.0f, vec3 {1.0f, 0.0f, 0.0f}, Model);
    TranslateTransform(View, 0.0f, 0.0f, -3.0f, View);
    PerspectiveMatrix(45.0f,
                      (float)WindowWidth/(float)WindowHeight,
                      0.1f, 100.0f, Projection);
    PassStaticRenderData();

    double LastFrameTime = glfwGetTime();
    double ThisFrameTime = LastFrameTime;
    double FrameDeltaTime = 0;
    vec3 Axis_t = {0.0f, 0.0f, 1.0f};
    while(!glfwWindowShouldClose(WindowHandle))
    {
        glViewport(0, 0, WindowWidth, WindowHeight);
        glClearColor(1.0f, 0.1f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //RotateTransform(Transform, 10 * FrameDeltaTime,
          //              Axis_t, Transform);
        UpdateRenderData();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(WindowHandle);
        glfwPollEvents();

        ThisFrameTime = glfwGetTime();
        FrameDeltaTime = ThisFrameTime - LastFrameTime;
        LastFrameTime = ThisFrameTime;
        //TODO: If defined debug
        #if 0
        fprintf(stdout, "Frame Delay: %.2f ms\n", FrameDeltaTime * 1000);
        #endif
    }

    glDeleteProgram(OpenGLProgram.ID);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(WindowHandle);
    glfwTerminate();
    return(0);
}
