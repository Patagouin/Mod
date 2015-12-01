#ifndef _PGHP_OPENGL_H_
#define _PGHP_OPENGL_H_

#include <glad/glad.h>

#include <iostream>

/** A macro to track OpenGL errors 
*/
inline void _check_gl_error(const char *file, int line) {
    GLenum err (glGetError());

    while(err!=GL_NO_ERROR) {
        std::string error;

        switch(err) {
        case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
        case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
        case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
        case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }

        std::cerr << "GL_" << error.c_str() << " - " << file<<":" << line << std::endl;
        err = glGetError();
    }
}

#define GL_TEST_ERR _check_gl_error(__FILE__,__LINE__)


#endif // _PGHP_OPENGL_H_
