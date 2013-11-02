uniform sampler2DRect image;
uniform float th;
uniform float sig;
uniform float lm;
uniform float ps;
uniform float ks;
varying vec2 texcoordM;
    
const float PI = 3.14159265358979323846264;

void main() {
    float theta = th*PI/180.0;
	float psi = ps*PI/180.0;
    float del = 2.0/(ks-1.0);
	float lmbd = lm;
    float sigma = sig/ks;
    float x_theta;
    float y_theta;
    float hks = (ks - 1.0) / 2.0;
    float x = texcoordM.x - hks;
    float y = texcoordM.y - hks;

	x_theta = x*del*cos(theta)+y*del*sin(theta);
	y_theta = -x*del*sin(theta)+y*del*cos(theta);

	float outvalue = exp(-0.5 * (pow(x_theta,2.0) + pow(y_theta,2.0)) / pow(sigma,2.0)) * cos(2.0 * PI * x_theta / lmbd + psi);
	outvalue /= 1.0;
	outvalue += 0.5;
	gl_FragColor.rgb = vec3(outvalue);
}