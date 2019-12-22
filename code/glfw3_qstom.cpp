/*
 * Plattform independent window context creation code.
 * Depends on glfw
 */
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define internal static
#define global_variable static
#define local_persist static

#ifdef QSTOM_UNIX
#include "unix_qstom.h"
#endif
#include "glfw3_qstom.h"

internal void
PrintOpenGLInfo(void)
{
    printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
           glGetString(GL_SHADING_LANGUAGE_VERSION));
}

internal int
IsShaderCompiled(GLint Shader, GLint ShaderType)
{
    GLint Success = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
    if(Success == GL_FALSE)
    {

        GLint InfoLogLength;
        const char *ShaderTypeString = (ShaderType==GL_VERTEX_SHADER)
            ? "Vertex"
            : "Fragment";
        GLchar InfoLog[256] = {};
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
        glGetShaderInfoLog(Shader, InfoLogLength,
                           NULL, &InfoLog[0]);
        //TODO: Proper logging in file
        fprintf(stderr, "[%s] %s\n", ShaderTypeString, InfoLog);
    }
    return Success;
}

internal opengl_program
CompileAndLinkOpenGLProgram(file_data* VertexFile,
                            file_data* FragmentFile)
{
    GLint Success = 1;
    GLint StringSize = 0;
    // TODO: Handle linking errors
    opengl_program Program = {};
    Program.VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Program.VertexShader, 1, &(VertexFile->Memory),NULL);
    glCompileShader(Program.VertexShader);
    if(!IsShaderCompiled(Program.VertexShader, GL_VERTEX_SHADER))
    {
        glDeleteProgram(Program.VertexShader);
        Program = {};
        return Program;
    }

    Program.FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(Program.FragmentShader, 1, &(FragmentFile->Memory),NULL);
    glCompileShader(Program.FragmentShader);
    if(!IsShaderCompiled(Program.FragmentShader, GL_FRAGMENT_SHADER))
    {
        glDeleteProgram(Program.FragmentShader);
        Program = {};
        return Program;
    }

    Program.OpenGLProgram = glCreateProgram();
    glAttachShader(Program.OpenGLProgram, Program.VertexShader);
    glAttachShader(Program.OpenGLProgram, Program.FragmentShader);
    glLinkProgram(Program.OpenGLProgram);
    return Program;
}

int main(void)
{
    GLFWwindow *WindowHandle;

    if(!glfwInit())
        return(-1);
   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    WindowHandle = glfwCreateWindow(800, 600, "QStom", NULL, NULL);
    if(!WindowHandle)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(WindowHandle);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    //if(!gladLoadGL())
    {
        glfwDestroyWindow(WindowHandle);
        glfwTerminate();
        return -1;
    }
    PrintOpenGLInfo();

    // NOTE: Read compile and link a vertex and fragment shader
    file_data VertexFile = {}, FragmentFile = {};
    ReadEntireFile("shaders/simple.vert", &VertexFile);
    ReadEntireFile("shaders/simple.frag", &FragmentFile);
    opengl_program OpenGLProgram =
        CompileAndLinkOpenGLProgram(&VertexFile,
                                    &FragmentFile);
    FreeFileMemory(&VertexFile);
    FreeFileMemory(&FragmentFile);

    while(!glfwWindowShouldClose(WindowHandle))
    {
        glClearColor(1.0f, 0.1f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(WindowHandle);
        glfwPollEvents();
    }
    glfwDestroyWindow(WindowHandle);
    glfwTerminate();
    return(0);
}
