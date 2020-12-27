#include <glew.h>
#include <freeglut.h>

class GLShader
{
public:
	GLuint ShaderProgram;
private:
	GLuint vertex_shader;
	GLuint fragment_shader;

	void printInfoLogShader(GLuint shader);
	void printInfoProgram(GLuint shader);

	GLuint compileSource(const char* source, GLuint shader_type)
	{
		GLuint shader = glCreateShader(shader_type);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);

//#if _DEBUG
//		printInfoLogShader(shader);
//#endif
		return shader;
	}

	void linkProgram()
	{
		ShaderProgram = glCreateProgram();
		glAttachShader(ShaderProgram, vertex_shader);
		glAttachShader(ShaderProgram, fragment_shader);
		glLinkProgram(ShaderProgram);
	}
public:
	GLShader() :ShaderProgram(0) {}
	~GLShader()
	{
		glUseProgram(0);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteProgram(ShaderProgram);
	}
	void load(const char* vertext_src, const char* fragment_src)
	{
		vertex_shader = compileSource(vertext_src, GL_VERTEX_SHADER);
		fragment_shader = compileSource(fragment_src, GL_FRAGMENT_SHADER);
		linkProgram();
	}
	GLuint getIDProgram() { return ShaderProgram; }

	GLint getAttribLocation(const char* name)const
	{
		return glGetAttribLocation(ShaderProgram, name);
	}
	void setUniformLocation(GLuint param, const float ** matrix )
	{
		glUniformMatrix4fv(param, 1, false, &matrix[0][0]);
	}
	GLuint getUniformLocation(const char* name)const
	{
		return glGetUniformLocation(ShaderProgram, name);
	}
	void use() { glUseProgram(ShaderProgram); }
	void off() { glUseProgram(0); }
};
