#include "Utils.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

namespace GLRenderer
{
	void Utils::CheckErrorAndPrint(const std::string& message)
	{
		GLenum errCode;
		const GLubyte *errString;

		if ((errCode = glGetError()) != GL_NO_ERROR)
		{
			errString = gluErrorString(errCode);
			std::cerr << message << std::endl;
			std::cerr << "OpenGL Error : " << errString << std::endl;
		}
	}
}
