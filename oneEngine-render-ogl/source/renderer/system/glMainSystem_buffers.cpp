
#include "glMainSystem.h"

void glMainSystem::CreateBuffer ( uint* index, uint count )
{
	glGenBuffers( count, index );
}
void glMainSystem::FreeBuffer ( uint* index, uint count )
{
	if ( *index != 0 ) {
		glDeleteBuffers( count, index );
		*index = 0;
	}
}
void glMainSystem::BindBuffer ( uint target, uint index )
{
	glBindBuffer( target, index );
}
void glMainSystem::UploadBuffer ( uint target, uint size, void* data, uint usage )
{
	glBufferData( target, size, NULL, usage );
	glBufferSubData( target, 0, size, data );
}
void glMainSystem::UnbindBuffer ( uint target )
{
	glBindBuffer( target, 0 );
}

void glMainSystem::BindVertexArray ( uint target )
{
	glBindVertexArray( target );
}

void glMainSystem::DrawElements ( uint type, int count, uint data, void* offset )
{
	glDrawElements( type, count, data, offset );
}
