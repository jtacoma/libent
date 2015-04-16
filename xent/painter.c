#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <ent.h>
#include <GL/glew.h>

#include "xent.h"

#include "shaders.h"

struct painter
{
	GLuint program;
	struct ent_table * entities;
	struct ent_processor * processor;
	int pos; // length_xy
	int dead; // uint8_t
};

struct painter *
painter_alloc (
    struct ent_table * entities)
{
	if (!entities)
	{
		errno = EINVAL;
		return NULL;
	}

	struct painter * painter = calloc (1, sizeof (struct painter));
	if (!painter)
	{
		return NULL;
	}

	painter->processor = ent_processor_alloc();
	if (!painter->processor)
	{
		free (painter);
		return NULL;
	}

	painter->pos =
	    ent_processor_use_column (
	        painter->processor, entities, "pos", sizeof (length_xy), "");
	if (painter->pos == -1)
	{
		ent_processor_free (painter->processor);
		free (painter);
		return NULL;
	}

	painter->dead =
	    ent_processor_use_column (
	        painter->processor, entities, "dead", sizeof (uint8_t), "");
	if (painter->dead == -1)
	{
		ent_processor_free (painter->processor);
		free (painter);
		return NULL;
	}

	painter->entities = entities;

	return painter;
}

void
painter_free (
    struct painter * painter)
{
	if (painter)
	{
		if (painter->processor)
		{
			ent_processor_free (painter->processor);
		}

		free (painter);
	}
}

static GLuint
load_shader (
    GLchar const src[],
    GLenum type)
{
	GLuint shader = glCreateShader (type);
	if (shader == 0)
	{
		return 0;
	}

	glShaderSource (shader, 1, &src, NULL);

	glCompileShader (shader);

	GLint compiled = 0;
	glGetShaderiv (shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char * infoLog = malloc (infoLen);
			glGetShaderInfoLog (shader, infoLen, NULL, infoLog);
			printf ("%s:%d: (painter_init) %s", __FILE__, __LINE__, infoLog);
			free (infoLog);
		}
		glDeleteShader (shader);
		return 0;
	}

	return shader;
}

static int
painter_init (
    struct painter * painter)
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		printf (
		    "%s:%d: (%s) %s\n",
		    __FILE__,
		    __LINE__,
		    ENT_VERSION,
		    glewGetErrorString (err));
		return -1;
	}
	printf ("GLEW %s\n", glewGetString (GLEW_VERSION));

	GLchar const * vertexSrc = shaders_sources[shaders_default_vert_id];
	GLuint vertexShader = load_shader (vertexSrc, GL_VERTEX_SHADER);
	if (!vertexShader)
	{
		errno = ENOEXEC;
		return -1;
	}

	GLchar const * fragmentSrc = shaders_sources[shaders_default_frag_id];
	GLuint fragmentShader = load_shader (fragmentSrc, GL_FRAGMENT_SHADER);
	if (!fragmentShader)
	{
		errno = ENOEXEC;
		return -1;
	}

	GLuint program = glCreateProgram();
	if (!program)
	{
		errno = ENOEXEC;
		return -1;
	}

	glAttachShader (program, vertexShader);
	glAttachShader (program, fragmentShader);
	glBindAttribLocation (program, 0, "pos");
	glLinkProgram (program);
	GLint linked = 0;
	glGetProgramiv (program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv (program, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char * log = malloc (infoLen);
			if (!log)
			{
				return -1;
			}

			glGetProgramInfoLog (program, infoLen, NULL, log);
			printf ("%s:%d: (painter_init) %s", __FILE__, __LINE__, log);
			free (log);
		}

		glDeleteProgram (program);
		return -1;
	}

	painter->program = program;
	return 0;
}

int
painter_paint (
    struct painter * painter,
    length_xy size)
{
	if (!painter)
	{
		errno = EINVAL;
		return -1;
	}

	if (!painter->program && painter_init (painter) == -1)
	{
		return -1;
	}

	printf ("visual: %.3fm x %.3fm\n", size[0], size[1]);

	glClearColor (0.0, 0.0, 0.0, 1.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	struct ent_session * session = ent_session_alloc (painter->processor);
	if (session == NULL)
	{
		return -1;
	}

	int rc = -1;
	do
	{
		struct ent_table * entities = painter->entities;
		length_xy const * poss =
		    (length_xy const *)ent_session_column_get_const (
		        session, entities, painter->pos);
		uint8_t const * deads =
		    (uint8_t const *)ent_session_column_get_const (
		        session, entities, painter->dead);
		size_t len = ent_session_table_len (session, entities);

		GLuint pos_buffer;
		glGenBuffers (1, &pos_buffer);
		glBindBuffer (GL_ARRAY_BUFFER, pos_buffer);
		glBufferData (GL_ARRAY_BUFFER, sizeof (*poss)*len, poss, GL_STATIC_DRAW);
		GLint size = 0;
		glGetBufferParameteriv (GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		if (sizeof (*poss)*len != (size_t)size)
		{
			GLint infoLen = 0;
			glGetProgramiv (painter->program, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen > 1)
			{
				char * log = malloc (infoLen);
				if (!log)
				{
					return -1;
				}

				glGetProgramInfoLog (painter->program, infoLen, NULL, log);
				printf ("%s:%d: (painter_paint) %s", __FILE__, __LINE__, log);
			}

			glDeleteBuffers (1, &pos_buffer);
			break;
		}

		glDrawArrays (GL_POINTS, 0, len);

		for (size_t i = 0; i < len; ++i)
		{
			printf (
			    "entity at %.3fm, %.3fm (%d)\n",
			    poss[i][0],
			    poss[i][1],
			    (int)deads[i]);
		}

		glDeleteBuffers (1, &pos_buffer);
		rc = 0;
	}
	while (0);

	ent_session_free (session);

	return rc;
}
