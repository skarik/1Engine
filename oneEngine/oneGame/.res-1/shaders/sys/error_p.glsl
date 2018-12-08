// sys/error
// Simple shader meant for use when actual target shader has an error.
#version 430

// Outputs
layout(location = 0) out vec4 FragDiffuse;

void main ( void )  
{
	FragDiffuse = vec4( 1,1,0,1 );
}