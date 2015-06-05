uniform mat4 gl_ModelViewMatrix; // From OpenGl
uniform mat4 gl_ProjectionMatrix; // Uniform variables can only be read. They remain constant through the whole collection of vertices (or anything
									// beyond, like per frame)

attribute vec4 gl_Vertex; // Attribute variables are also read-only. 
							//They can change per vertex, and, in this case, hold information per vertex (the vertex's position).

void main()
{
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	// or shortened: gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	// or optimized (not sure if this works): glPosition = ftransform(void);
}