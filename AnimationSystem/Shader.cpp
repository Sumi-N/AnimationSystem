#include "Shader.h"
#include <fstream>
#include <iostream>

void Shader::SetShader(const char* i_vert, const char* i_frag)
{
	vertpath = i_vert;
	fragpath = i_frag;
}

void Shader::SetShader(const char* i_vert, const char* i_geo, const char* i_frag)
{
	vertpath = i_vert;
	geopath = i_geo;
	fragpath = i_frag;
}

void Shader::SetShader(const char* i_vert, const char* i_control, const char* i_eval, const char* i_frag)
{
	vertpath = i_vert;
	controlpath = i_control;
	evalpath = i_eval;
	fragpath = i_frag;
}

void Shader::SetShader(const char* i_vert, const char* i_control, const char* i_eval, const char* i_geo, const char* i_frag)
{
	vertpath = i_vert;
	controlpath = i_control;
	evalpath = i_eval;
	geopath = i_geo;
	fragpath = i_frag;
}

void Shader::LoadShader()
{
	if (controlpath && evalpath && geopath)
	{
		Shader::LoadShader(*this, vertpath, controlpath, evalpath, geopath, fragpath);
	}
	else if ((!controlpath && !evalpath) && geopath)
	{
		Shader::LoadShader(*this, vertpath, geopath, fragpath);
	}
	else if ((controlpath && evalpath) && !geopath)
	{
		Shader::LoadShader(*this, vertpath, controlpath, evalpath, fragpath);
	}
	else if (vertpath && fragpath)
	{
		Shader::LoadShader(*this, vertpath, fragpath);
	}
	else
	{
		DEBUG_ASSERT(false);
	}
}

void Shader::BindShader()
{
	glUseProgram(programid);
}

bool Shader::HasTessellationShader()
{
	// Check if the data required to draw in patches or not
	return (controlpath && evalpath) ? true : false;
}

void Shader::LoadShader(Shader& io_shader, const char* i_vert, const char* i_frag)
{
	std::vector<GLchar> vertsrc;
	std::vector<GLchar> fragsrc;

	if (!strstr(i_vert, "vert.glsl"))
	{
		printf("There is an erroer");
	}



	ReadShaderSource(i_vert, vertsrc);
	ReadShaderSource(i_frag, fragsrc);

	// Create program;
	if (io_shader.programid != 0)
	{
		glDeleteProgram(io_shader.programid);
	}

	io_shader.programid = glCreateProgram();

	// Attach the shaders to the program
	// Vertex Shader
	AttachShaderProgram(io_shader, vertsrc, GL_VERTEX_SHADER);

	// Fragment Shader
	AttachShaderProgram(io_shader, fragsrc, GL_FRAGMENT_SHADER);

	// Link the program
	glBindAttribLocation(io_shader.programid, 0, "model_position");
	glBindAttribLocation(io_shader.programid, 1, "model_normal");
	glBindAttribLocation(io_shader.programid, 2, "model_texcoord");
	glBindAttribLocation(io_shader.programid, 3, "model_tangent_vec");
	glBindAttribLocation(io_shader.programid, 4, "model_bitangent_vec");
	glBindAttribLocation(io_shader.programid, 5, "index");
	glBindAttribLocation(io_shader.programid, 6, "weight");
	glBindFragDataLocation(io_shader.programid, 0, "color");
	glLinkProgram(io_shader.programid);

	if (PrintProgramInfoLog(io_shader.programid))
	{
		DEBUG_PRINT("Succeed compiling the shader %s, and %s \n", i_vert, i_frag);
		printf("succeed loading shader\n");
	}
	else
	{
		DEBUG_PRINT("Failed compiling one of the shaders in %s, and %s \n", i_vert, i_frag);
		printf("failed loading shader\n");
	}
}

void Shader::LoadShader(Shader& io_shader, const char* i_vert, const char* i_geo, const char* i_frag)
{
	std::vector<GLchar> vertsrc;
	std::vector<GLchar> geosrc;
	std::vector<GLchar> fragsrc;

	ReadShaderSource(i_vert, vertsrc);
	ReadShaderSource(i_geo, geosrc);
	ReadShaderSource(i_frag, fragsrc);

	// Create program;
	if (io_shader.programid != 0)
	{
		glDeleteProgram(io_shader.programid);
	}

	io_shader.programid = glCreateProgram();

	// Attach the shaders to the program
	// Vertex Shader
	AttachShaderProgram(io_shader, vertsrc, GL_VERTEX_SHADER);

	// Geometry Shader
	AttachShaderProgram(io_shader, geosrc, GL_GEOMETRY_SHADER);

	// Fragment Shader
	AttachShaderProgram(io_shader, fragsrc, GL_FRAGMENT_SHADER);

	// Link the program
	glBindAttribLocation(io_shader.programid, 0, "model_position");
	glBindAttribLocation(io_shader.programid, 1, "model_normal");
	glBindAttribLocation(io_shader.programid, 2, "model_texcoord");
	glBindAttribLocation(io_shader.programid, 3, "model_tangent_vec");
	glBindAttribLocation(io_shader.programid, 4, "model_bitangent_vec");
	glBindFragDataLocation(io_shader.programid, 0, "color");
	glLinkProgram(io_shader.programid);

	if (PrintProgramInfoLog(io_shader.programid))
	{
		DEBUG_PRINT("Succeed compiling the shader %s, %s, and %s \n", i_vert, i_geo, i_frag);
	}
	else
	{
		DEBUG_PRINT("Failed compiling one of the shaders in %s, %s, and %s \n", i_vert, i_geo, i_frag);
	}
}

void Shader::LoadShader(Shader& io_shader, const char* i_vert, const char* i_control, const char* i_eval, const char* i_frag)
{
	std::vector<GLchar> vertsrc;
	std::vector<GLchar> controlsrc;
	std::vector<GLchar> evalsrc;
	std::vector<GLchar> fragsrc;

	ReadShaderSource(i_vert, vertsrc);
	ReadShaderSource(i_control, controlsrc);
	ReadShaderSource(i_eval, evalsrc);
	ReadShaderSource(i_frag, fragsrc);

	// Create program;
	if (io_shader.programid != 0)
	{
		glDeleteProgram(io_shader.programid);
	}

	io_shader.programid = glCreateProgram();

	// Attach the shaders to the program
	// Vertex Shader
	AttachShaderProgram(io_shader, vertsrc, GL_VERTEX_SHADER);

	// Control Shader
	AttachShaderProgram(io_shader, controlsrc, GL_TESS_CONTROL_SHADER);

	// Evaluation Shader
	AttachShaderProgram(io_shader, evalsrc, GL_TESS_EVALUATION_SHADER);

	// Fragment Shader
	AttachShaderProgram(io_shader, fragsrc, GL_FRAGMENT_SHADER);

	// Link the program
	glBindAttribLocation(io_shader.programid, 0, "model_position");
	glBindAttribLocation(io_shader.programid, 1, "model_normal");
	glBindAttribLocation(io_shader.programid, 2, "model_texcoord");
	glBindAttribLocation(io_shader.programid, 3, "model_tangent_vec");
	glBindAttribLocation(io_shader.programid, 4, "model_bitangent_vec");
	glBindFragDataLocation(io_shader.programid, 0, "color");
	glLinkProgram(io_shader.programid);

	if (PrintProgramInfoLog(io_shader.programid))
	{
		DEBUG_PRINT("Succeed compiling the shader %s, %s, %s, and %s\n", i_vert, i_control, i_eval, i_frag);
	}
	else
	{
		DEBUG_PRINT("Failed compiling one of the shaders in %s, %s, %s, and %s \n", i_vert, i_control, i_eval, i_frag);
	}
}

void Shader::LoadShader(Shader& io_shader, const char* i_vert, const char* i_control, const char* i_eval, const char* i_geo, const char* i_frag)
{
	std::vector<GLchar> vertsrc;
	std::vector<GLchar> controlsrc;
	std::vector<GLchar> evalsrc;
	std::vector<GLchar> geosrc;
	std::vector<GLchar> fragsrc;

	ReadShaderSource(i_vert, vertsrc);
	ReadShaderSource(i_control, controlsrc);
	ReadShaderSource(i_eval, evalsrc);
	ReadShaderSource(i_geo, geosrc);
	ReadShaderSource(i_frag, fragsrc);

	// Create program;
	if (io_shader.programid != 0)
	{
		glDeleteProgram(io_shader.programid);
	}

	io_shader.programid = glCreateProgram();

	// Attach the shaders to the program
	// Vertex Shader
	AttachShaderProgram(io_shader, vertsrc, GL_VERTEX_SHADER);

	// Control Shader
	AttachShaderProgram(io_shader, controlsrc, GL_TESS_CONTROL_SHADER);

	// Evaluation Shader
	AttachShaderProgram(io_shader, evalsrc, GL_TESS_EVALUATION_SHADER);

	// Geometry Shader
	AttachShaderProgram(io_shader, geosrc, GL_GEOMETRY_SHADER);

	// Fragment Shader
	AttachShaderProgram(io_shader, fragsrc, GL_FRAGMENT_SHADER);

	// Link the program
	glBindAttribLocation(io_shader.programid, 0, "model_position");
	glBindAttribLocation(io_shader.programid, 1, "model_normal");
	glBindAttribLocation(io_shader.programid, 2, "model_texcoord");
	glBindAttribLocation(io_shader.programid, 3, "model_tangent_vec");
	glBindAttribLocation(io_shader.programid, 4, "model_bitangent_vec");
	glBindFragDataLocation(io_shader.programid, 0, "color");
	glLinkProgram(io_shader.programid);

	if (PrintProgramInfoLog(io_shader.programid))
	{
		DEBUG_PRINT("Succeed compiling the shader %s, %s, %s, %s, and %s\n", i_vert, i_control, i_eval, i_geo, i_frag);
	}
	else
	{
		DEBUG_PRINT("Failed compiling one of the shaders %s, %s, %s, %s, and %s \n", i_vert, i_control, i_eval, i_geo, i_frag);
	}
}

bool Shader::ReadShaderSource(const char* i_file, std::vector<GLchar>& io_buffer)
{
	if (i_file == nullptr)
	{
		return false;
	}

	std::ifstream file(i_file, std::ios::binary);
	if (file.fail())
	{
		DEBUG_PRINT("Error: Can't open source file: %s \n", i_file);
		return false;
	}

	file.seekg(0L, std::ios::end);
	GLsizei size = static_cast<GLsizei>(file.tellg());

	io_buffer.resize(static_cast<size_t>(size) + 1);
	file.seekg(0L, std::ios::beg);
	file.read(io_buffer.data(), size);
	io_buffer[size] = '\0';

	if (file.fail())
	{
		DEBUG_PRINT("Error: Could not read source file: %s \n", i_file);
		file.close();
		return false;
	}

	file.close();
	return true;
}

GLboolean Shader::PrintShaderInfoLog(GLuint shader, const char* str)
{
	// Get compile status
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
		DEBUG_PRINT("Compile Error in: %s \n", str);

	// Get the length of the compile log
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);
	if (bufSize > 1)
	{
		// Get the contents of the compile log
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		DEBUG_PRINT("%s \n", &infoLog[0]);
	}
	return static_cast<GLboolean>(status);
}

GLboolean Shader::PrintProgramInfoLog(GLuint i_program)
{
	// Get a link result
	GLint status;
	glGetProgramiv(i_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
		DEBUG_PRINT("Link Error.");
	// Get the log of the link result
	GLsizei bufSize;
	glGetProgramiv(i_program, GL_INFO_LOG_LENGTH, &bufSize);
	if (bufSize > 1)
	{
		// Get the contents of the link result
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(i_program, bufSize, &length, &infoLog[0]);
		DEBUG_PRINT("%s \n", &infoLog[0]);
	}
	return static_cast<GLboolean>(status);
}

void Shader::AttachShaderProgram(Shader& io_shader, std::vector<GLchar> i_src, GLint i_shadertype)
{
	if (i_src.data() != nullptr)
	{
		const GLuint shader = glCreateShader(i_shadertype);
		const char* srcponter = i_src.data();
		glShaderSource(shader, 1, &srcponter, NULL);
		glCompileShader(shader);

		switch (i_shadertype)
		{
		case GL_VERTEX_SHADER:
			if (PrintShaderInfoLog(shader, "vertex shader"))
			{
				glAttachShader(io_shader.programid, shader);
			}
			break;
		case GL_TESS_CONTROL_SHADER:
			if (PrintShaderInfoLog(shader, "tessellation control shader"))
			{
				glAttachShader(io_shader.programid, shader);
			}
			break;
		case GL_TESS_EVALUATION_SHADER:
			if (PrintShaderInfoLog(shader, "tessellation evaluation shader"))
			{
				glAttachShader(io_shader.programid, shader);
			}
			break;
		case GL_GEOMETRY_SHADER:
			if (PrintShaderInfoLog(shader, "geometry shader"))
			{
				glAttachShader(io_shader.programid, shader);
			}
			break;
		case GL_FRAGMENT_SHADER:
			if (PrintShaderInfoLog(shader, "fragment shader"))
			{
				glAttachShader(io_shader.programid, shader);
			}
			break;
		default:
			DEBUG_ASSERT(false);
			break;
		}
		glDeleteShader(shader);
	}
}