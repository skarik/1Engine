
#include "glMainSystem.h"
/*
glHandle mActiveTexture = 0;
void ActivateTexture ( glHandle tex )
{
	if ( mActiveTexture != tex )
	{
		glBindTexture( GL_TEXTURE2D, tex );
		tex = mActiveTexture;
	}
}

glHandle glMainSystem::GetNewTexture( void )
{
	unsigned int newTex;
	glGenTextures( 1, &newTex );
	return (glHandle)newTex;
}
void glMainSystem::FreeTexture( glHandle tex )
{
	glDeleteTextures( 1, &tex );
}

void glMainSystem::TextureSetWrapping( const glHandle tex, const eWrappingType wrapX, const eWrappingType wrapY )
{
	switch ( wrapX )
	{
		case Repeat: glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			break;
		case Clamp: glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
			break;
		case MirroredRepeat: glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
			break;
	}
	switch ( wrapY )
	{
		case Repeat: glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			break;
		case Clamp: glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
			break;
		case MirroredRepeat: glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
			break;
	}
}
void glMainSystem::TextureSetWrapping( const glHandle tex, const eWrappingType wrapX, const eWrappingType wrapY, const eWrappingType wrapZ )
{
	TextureSetWrapping( tex, wrapX, wrapY );
	switch ( wrapZ )
	{
		case Repeat: glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT );
			break;
		case Clamp: glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP );
			break;
		case MirroredRepeat: glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT );
			break;
	}
}*/

//==============================================================================================//
// FORMATS (specific to openGL)
//==============================================================================================//
//
//glEnum glMainSystem::Enum ( const eColorFormat fmt )
//{
//	switch ( fmt ) {
//	case RGB8:		return GL_RGB8;
//	case RGBA8:		return GL_RGBA8;
//	case RGB16:		return GL_RGB16;
//	case RGB16F:	return GL_RGB16F;
//	case RGBA16F:	return GL_RGBA16F;
//	case RGBA16:	return GL_RGBA16;
//	}
//	throw core::InvalidArgumentException();
//	return 0x0;
//}
//glEnum glMainSystem::Enum ( const eTextureType fmt )
//{
//	switch ( fmt ) {
//	case Texture2D:		return GL_TEXTURE_2D;
//	case Texture3D:		return GL_TEXTURE_3D;
//	case TextureCube:	return GL_TEXTURE_CUBE_MAP;
//	}
//	throw core::InvalidArgumentException();
//	return 0x0;
//}
//glEnum glMainSystem::Enum ( const eDepthFormat fmt )
//{
//	switch ( fmt ) {
//	case DepthNone:		return 0x0;
//	case Depth16:		return GL_DEPTH_COMPONENT16;
//	case Depth24:		return GL_DEPTH_COMPONENT24;
//	case Depth32:		return GL_DEPTH_COMPONENT32;
//	}
//	throw core::InvalidArgumentException();
//	return 0x0;
//}
//glEnum glMainSystem::Enum ( const eStencilFormat fmt )
//{
//	switch ( fmt ) {
//	case StencilNone:		return 0x0;
//	case StencilIndex1:		return GL_STENCIL_INDEX1;
//	case StencilIndex4:		return GL_STENCIL_INDEX4;
//	case StencilIndex8:		return GL_STENCIL_INDEX8;
//	case StencilIndex16:	return GL_STENCIL_INDEX16;
//	}
//	throw core::InvalidArgumentException();
//	return 0x0;
//}
//glEnum glMainSystem::Enum ( const eWrappingType fmt )
//{
//	switch ( fmt ) {
//	case Repeat:			return GL_REPEAT;
//	case Clamp:				return GL_CLAMP_TO_EDGE;
//	case MirroredRepeat:	return GL_MIRRORED_REPEAT;
//	}
//	throw core::InvalidArgumentException();
//	return 0x0;
//}
//glEnum glMainSystem::Enum ( const eSamplingFilter fmt )
//{
//	switch ( fmt ) {
//	case SamplingLinear:	return GL_LINEAR;
//	case SamplingPoint:		return GL_NEAREST;
//	}
//	throw core::InvalidArgumentException();
//	return 0x0;
//}