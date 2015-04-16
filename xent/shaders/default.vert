attribute vec4 position;
attribute vec4 colour;

varying vec4 v_colour;

void main()
{
	v_colour = colour;
	gl_Position = position;
}
