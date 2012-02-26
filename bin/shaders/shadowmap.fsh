//Fragment shader for a shadowmap doesn't need to do anything,
//because there is no color to write. The depth component
//is done by OpenGL when setting gl_Position in the vertex shader
//We could manually do the depth component by setting gl_FragDepth
//but this is not neccesary.
void main(){
}