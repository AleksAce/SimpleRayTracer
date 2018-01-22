#version 330

out vec4 outputColor;

uniform sampler2D gSampler;
void main()
{
vec2 UV = vec2(gl_FragCoord.x/1920, gl_FragCoord.y/1080);
vec4 vTexColor = texture2D(gSampler,UV);
 outputColor = vTexColor;
 //outputColor = vec4(1.0,0.0,0.0,1.0);
}
