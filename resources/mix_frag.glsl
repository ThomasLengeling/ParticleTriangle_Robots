uniform sampler2D texScene;
uniform sampler2D texBloom;
uniform sampler2D texTed;

uniform float alphaMix;


void main()
{
	vec4 scene = texture(texScene,  gl_TexCoord[0].st);
	vec4 bloom = texture(texBloom,  gl_TexCoord[0].st);
	vec4 ted   = texture(texTed,  gl_TexCoord[0].st);
}