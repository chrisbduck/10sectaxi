attribute vec4	a_VertPos;
uniform mat4	u_Matrix;

void main()
{
	gl_Position = u_Matrix * a_VertPos;
}
