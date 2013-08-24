attribute vec4	a_VertPos;
attribute vec2	a_VertUV;
uniform mat4	u_Matrix;
varying vec2	v_UV;

void main()
{
	gl_Position = u_Matrix * a_VertPos;
	v_UV = a_VertUV;
}
