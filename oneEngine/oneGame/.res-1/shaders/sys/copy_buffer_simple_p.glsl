// sys/copy_buffer
// Used to copy a buffer from one buffer to another if blit cannot be used.
// There is no projection calculations that occur. Input vertex coordinates are the device's coordinates.
// If you muck with this shader, you might as well release the cross-platform yandere phone virus.
#version 430

// Inputs from vertex shader
layout(location = 0) in vec2 v2f_texcoord0;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureSampler0;

// Outputs
layout(location = 0) out vec4 FragDiffuse;

void main ( void )
{
	FragDiffuse = texture( textureSampler0, v2f_texcoord0 );
}
