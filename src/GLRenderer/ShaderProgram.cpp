#include "ShaderProgram.h"

#include <sstream>
#include <iostream>
#include <fstream>

namespace GLRenderer
{
	ShaderProgram::ShaderProgram()
	{
		_programHandle = glCreateProgram();
	}

	ShaderProgram::~ShaderProgram()
	{
		for(auto& handle : _shaderHandles)
			glDeleteShader(handle);
		glDeleteProgram(_programHandle);
	}

	bool ShaderProgram::AddShaderFromFile(const GLenum type,
			const std::string& path, std::string& log)
	{
		// create shader
		GLuint handle = glCreateShader(type);

		// read and store code from file
		std::ifstream fileStream(path);
		if( !fileStream.is_open() )
			std::cout << "Could not open : " << path << std::endl;

		std::string str((std::istreambuf_iterator<char>(fileStream)),
				std::istreambuf_iterator<char>());
		fileStream.close();

		const GLchar* shaderCode = str.c_str();
		const GLint codeSize[] = { (GLint) str.size() };
		const GLchar* codeArray[] = { shaderCode };
		glShaderSource(handle, 1, codeArray, codeSize);

		// compile
		glCompileShader(handle);

		// check status
		GLint res;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &res);
		if(res == GL_FALSE)
		{
			GLint logSize;
			glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);

			if(logSize > 0)
			{
				char* logChar = new char[logSize];

				GLsizei written;
				glGetShaderInfoLog(handle, logSize, &written, logChar);

				log = std::string(logChar);
				delete [] logChar;
			}
		}
		else
		{
			_shaderHandles.push_back(handle);
		}

		return res == GL_TRUE;
	}

	void ShaderProgram::BindAttribLocation(const int location, const std::string& name)
	{
		glBindAttribLocation(_programHandle, location, name.c_str());
	}

	GLuint ShaderProgram::GetAttribLocation(const std::string& name) const
	{
		return glGetAttribLocation(_programHandle, name.c_str());
	}

	GLuint ShaderProgram::GetUniformLocation(const std::string& name) const
	{
		return glGetUniformLocation(_programHandle, name.c_str()); 
	}

	void ShaderProgram::BindUniformBlock(const std::string& name, const GLuint bindingIndex)
	{
		glUniformBlockBinding(_programHandle, GetUniformBlockIndex(name), bindingIndex);
	}

	GLuint ShaderProgram::GetSubroutineIndex(const GLenum type, const std::string& name) const
	{
		return glGetSubroutineIndex(_programHandle, type, name.c_str());
	}

	GLuint ShaderProgram::GetUniformBlockIndex(const std::string& name) const
	{
		return glGetUniformBlockIndex(_programHandle, name.c_str());
	}

	GLint ShaderProgram::GetUniformBlockSize(const std::string& name) const
	{
		GLint blockSize;
		glGetActiveUniformBlockiv(_programHandle, GetUniformBlockIndex(name),
			GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

		return blockSize;
	}

	GLint ShaderProgram::GetUniformBlockOffset(const std::string& name, const std::string& variable) const
	{
		const GLchar* variableName[] = { variable.c_str() };
		GLuint indice[1];
		GLint offset[1];

		glGetUniformIndices(_programHandle, 1, variableName, indice);
		glGetActiveUniformsiv(_programHandle, 1, indice, GL_UNIFORM_OFFSET, offset);

		return offset[0];
	}

	bool ShaderProgram::Link(std::string& log)
	{
		for(auto& handle : _shaderHandles)
			glAttachShader(_programHandle, handle);

		glLinkProgram(_programHandle);

		GLint res;
		glGetProgramiv(_programHandle, GL_LINK_STATUS, &res);
		if(res == GL_FALSE)
		{
			GLint logSize;
			glGetProgramiv(_programHandle, GL_INFO_LOG_LENGTH, &logSize);

			if(logSize > 0)
			{
				char* logChar = new char[logSize];

				GLsizei written;
				glGetProgramInfoLog(_programHandle, logSize, &written, logChar);

				log = std::string(logChar);
				delete [] logChar;
			}
		}

		return res == GL_TRUE;
	}

	void ShaderProgram::Install() const
	{
		glUseProgram(_programHandle);
	}

	void ShaderProgram::Release() const
	{
		glUseProgram(0);
	}

	void ShaderProgram::PrintAttrib() const
	{
		GLint maxLength, nAttribs;
		glGetProgramiv(_programHandle, GL_ACTIVE_ATTRIBUTES, &nAttribs);
		glGetProgramiv(_programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

		GLchar* name = new char[maxLength];

		GLint written, size, location;
		GLenum type;

		std::cout << "Active attributes location : " << std::endl;
		std::cout << "Location\t|\tName" << std::endl;
		std::cout << "--------------------------------------------------------" << std::endl;

		for(unsigned int i = 0; i < nAttribs; ++i)
		{
			glGetActiveAttrib(_programHandle, i, maxLength, &written, &size, &type, name);
			location = glGetAttribLocation(_programHandle, name);
			std::cout << location << "\t|\t" << name << std::endl;
		}

		delete [] name;
	}

	void ShaderProgram::PrintUniform() const
	{
		GLint maxLength, nUniforms;
		glGetProgramiv(_programHandle, GL_ACTIVE_UNIFORMS, &nUniforms);
		glGetProgramiv(_programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

		GLchar* name = new char[maxLength];

		GLint written, size, location;
		GLenum type;

		std::cout << "Active uniforms location : " << std::endl;
		std::cout << "Location\t|\tName" << std::endl;
		std::cout << "--------------------------------------------------------" << std::endl;

		for(unsigned int i = 0; i < nUniforms; ++i)
		{
			glGetActiveUniform(_programHandle, i, maxLength, &written, &size, &type, name);
			location = glGetUniformLocation(_programHandle, name);
			std::cout << location << "\t|\t" << name << std::endl;
		}

		delete [] name;
	}
}
