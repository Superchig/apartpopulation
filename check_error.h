#ifndef CHECK_ERROR_H
#define CHECK_ERROR_H

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#endif
