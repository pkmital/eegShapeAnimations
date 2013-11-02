uniform sampler2DRect image;
uniform float th;
uniform float sig;
uniform float lm;
uniform float ps;
uniform float ks;
varying vec2 texcoordM;
    
const float PI = 3.14159265358979323846264;

void main() {
    // get the homogeneous 2d position
    gl_Position = ftransform();
    
    // transform texcoord	
	vec2 texcoord = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
	
	// get sample positions
    texcoordM = texcoord;
}
