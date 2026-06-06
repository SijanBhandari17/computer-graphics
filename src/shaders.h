#ifndef SHADERS_H
#define SHADERS_H

extern const char *vertexShaderSource;
extern const char *fragmentShaderSource;
unsigned int createShaderProgram(const char *vertSrc, const char *fragSrc);
unsigned int compileShader(unsigned int type, const char *source);

#endif
