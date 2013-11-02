uniform float time;

void main() {
    // get the homogeneous 2d position
    gl_Position = ftransform();
}
