uniform float time;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * (43758.5453 + 43758.5415 * time));
}

void main() {
	float res = 1.0;
	float randval = rand(vec2(gl_FragCoord.x / 1024.0, gl_FragCoord.y / 768.0) * res);
	gl_FragColor.rgb = vec3(randval, randval, randval);
}