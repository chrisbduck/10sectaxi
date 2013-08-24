precision mediump float;

uniform vec4 u_Colour;
uniform sampler2D u_Texture;
varying vec2 v_UV;

void main()
{
	vec4 lTexCol = texture2D(u_Texture, v_UV);
	gl_FragColor = u_Colour * lTexCol;
}
