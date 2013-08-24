attribute vec4 vPos;
uniform mat4 mMat;

void main()
{
	gl_Position = mMat * vPos;
}
