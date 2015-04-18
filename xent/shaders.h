/* This file was automatically generated;
 * changes made here will be lost.
 */

static char const * shaders_names [] =
{
	"shaders_default_vert",
	"shaders_default_frag",
};

static char const * shaders_sources [] =
{
	"attribute vec4 position;\n"
	"attribute vec4 colour;\n"
	"\n"
	"varying vec4 v_colour;\n"
	"\n"
	"void main()\n"
	"{\n"
	"v_colour = colour;\n"
	"gl_Position = position;\n"
	"}\n"
	,
	"varying vec4 v_colour;\n"
	"\n"
	"void main()\n"
	"{\n"
	"gl_FragColor = v_colour;\n"
	"}\n"
	,
};

enum shaders_type
{
	SHADERS_TYPE_FRAG,
	SHADERS_TYPE_VERT,
};

enum shaders_type shaders_types [] =
{
	SHADERS_TYPE_FRAG,
	SHADERS_TYPE_VERT,
};

// Identifiers for hard-coded access to the shaders above
static int shaders_default_vert_id = 0;
static int shaders_default_frag_id = 1;
