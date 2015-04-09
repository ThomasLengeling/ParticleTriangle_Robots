uniform sampler2D texScene;
uniform sampler2D texWire;

uniform float alphaFrameWire;


void main()
{
	vec4 t0 = texture(texScene,  gl_TexCoord[0].st);
	vec4 t1 = texture(texWire,  gl_TexCoord[0].st);

	if(t1.r > 0.5)
		t1 = vec4(1.0, 1.0, 1.0, alphaFrameWire);
	else
		t1 = vec4(0.0, 0.0, 0.0, 0.0);

	if(t1.r == 0.0)
		gl_FragColor = t0;
	else{
		if( alphaFrameWire > 0.0 && alphaFrameWire <= 0.15)
			gl_FragColor = vec4(t1.r, t1.g, t1.b, alphaFrameWire);
		else
			gl_FragColor = mix(t1, t0, alphaFrameWire);
	}
}