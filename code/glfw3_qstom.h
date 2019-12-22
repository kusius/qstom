#ifndef __GLFW3_QSTOM_H_
#define __GLFW3_QSTOM_H_

struct opengl_program
{
    GLuint VertexShader, FragmentShader, OpenGLProgram;
};

// TODO: Ifdefs on platform defines to expose all the functions
// we need to call from glfw3_qstom.cpp without specificity
// for example right now, to replace UnixReadFile with a generic
// ReadFile

#endif // __GLFW3_QSTOM_H_
