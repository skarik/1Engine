

// Include the main system
#include "glMainSystem.h"

#include "core/system/Screen.h"
#include "core/settings/CGameSettings.h"	// For shader settings

#include "renderer/state/Settings.h"

void	glMainSystem::prepareDraw ( void )
{
	if ( OldSchool ) {
		throw core::DeprecatedCallException();
		//glPushMatrix();
	}
}
void	glMainSystem::cleanupDraw ( void )
{
	if ( OldSchool ) {
		throw core::DeprecatedCallException();
		//glPopMatrix();
	}
	while ( hasModelMatrix() ) {
		popModelMatrix();
	}
}

void	glMainSystem::beginOrtho ( void )
{
	beginOrtho( 0,0, (Real)Screen::Info.width,(Real)Screen::Info.height, -45.0f, 45.0f );
}
void	glMainSystem::beginOrtho ( ftype left, ftype top, ftype width, ftype height, ftype minz, ftype maxz, bool flipped )
{
	//if ( iCurrentProjectionMode == 1 )
	//	return;
	//iCurrentProjectionMode = 1;
	iCurrentProjectionMode += 1;

	//glDisable( GL_LIGHTING );
	//glDisable( GL_CULL_FACE ); //not it
	
	Matrix4x4 projection, view;

	float x_max, y_max, z_max;
	x_max = width; //right-left
	y_max = height; //top-bottom
	z_max = maxz-minz;

	projection[0][0] = 2/x_max;
	projection[0][1] = 0;
	projection[0][2] = 0;
	projection[0][3] = 0;

	projection[1][0] = 0;
	projection[1][1] = (flipped? (-2/y_max) : (2/y_max));
	projection[1][2] = 0;
	projection[1][3] = 0;

	projection[2][0] = 0;
	projection[2][1] = 0;
	projection[2][2] = 2/(z_max);
	projection[2][3] = 0;

	projection[3][0] = (left+left+width)/(-x_max);
	projection[3][1] = (flipped? (1) : (-1)) * ((top+top+height)/(y_max));
	projection[3][2] = (minz+maxz)/(-z_max);
	projection[3][3] = 1;

	view = Matrix4x4();

	pushProjection( view * projection );
}
void	glMainSystem::endOrtho ( void )
{
	if ( iCurrentProjectionMode == 0 )
		return;
	iCurrentProjectionMode -= 1;

	popProjection();
}
bool glMainSystem::inOrtho ( void )
{
	return iCurrentProjectionMode>0;
}

void glMainSystem::pushProjection ( const Matrix4x4& matrix )
{
	viewprojection_stack.push_back( matrix );
}
void glMainSystem::popProjection ( void )
{
	viewprojection_stack.pop_back();
}

const Matrix4x4& glMainSystem::getProjection ( void )
{
	return viewprojection_stack.back();
}

bool glMainSystem::hasModelMatrix ( void )
{
	return (!modelmatrix_stack.empty());
}
void glMainSystem::pushModelMatrix ( const Matrix4x4& matrix )
{
	modelmatrix_stack.push_back( matrix );
}
void glMainSystem::popModelMatrix ( )
{
	modelmatrix_stack.pop_back();
}

const Matrix4x4& glMainSystem::getModelMatrix ( void )
{
	return modelmatrix_stack.back();
}



void	glMainSystem::setupAmbient ( void )
{
	throw core::DeprecatedCallException();
	// Set the ambient lighting
	if ( !CGameSettings::Active()->b_ro_EnableShaders ) {
		//glLightfv( GL_LIGHT0, GL_AMBIENT, renderer::Settings.ambientColor.start_point() );
	}
}

void	glMainSystem::setupViewport ( int x, int y, int width, int height )
{
	glViewport( x, y, width, height );
	glScissor( x, y, width, height );
}

void glMainSystem::scissorViewport ( int x, int y, int width, int height )
{
	glScissor( x, y, width, height );
}



void	glMainSystem::Transform ( const XrTransform* pTransform )
{
#if 0
#ifdef _ENGINE_DEBUG
#ifdef _ENGINE_SAFE_CHECK_
	fnl_assert( VALID_FLOAT(pTransform->position.x) );
	fnl_assert( VALID_FLOAT(pTransform->position.y) );
	fnl_assert( VALID_FLOAT(pTransform->position.z) );
	fnl_assert( VALID_FLOAT(pTransform->scale.x) );
	fnl_assert( VALID_FLOAT(pTransform->scale.y) );
	fnl_assert( VALID_FLOAT(pTransform->scale.z) );
	static int i;
	for ( i = 0; i < 9; ++i ) {
		fnl_assert( VALID_FLOAT(pTransform->rotation.pData[i]) );
	}
#else
	if ( !VALID_FLOAT(pTransform->position.x) ) pTransform->position.x = 0;
	if ( !VALID_FLOAT(pTransform->position.y) ) pTransform->position.y = 0;
	if ( !VALID_FLOAT(pTransform->position.z) ) pTransform->position.z = 0;
	if ( !VALID_FLOAT(pTransform->scale.x) ) pTransform->scale.x = 1;
	if ( !VALID_FLOAT(pTransform->scale.y) ) pTransform->scale.y = 1;
	if ( !VALID_FLOAT(pTransform->scale.z) ) pTransform->scale.z = 1;
	bool invalid = false;
	for ( int i = 0; i < 9; ++i ) {
		invalid = invalid || !VALID_FLOAT(pTransform->rotation.pData[i]);
	}
	if ( invalid ) pTransform->rotation = Matrix3x3();
#endif
#endif
#endif

	/*glPushMatrix();
	glLoadIdentity();

	glTranslatef( pTransform->position.x,pTransform->position.y,pTransform->position.z );
	//Vector3d rotEuler = pTransform->rotation.getEulerAngles();
	//glRotatef( rotEuler.z, 0,0,1 );
	//glRotatef( rotEuler.y, 0,1,0 );
	//glRotatef( rotEuler.x, 1,0,0 );
	Matrix4x4 temp;
	temp.setRotation(!(pTransform->rotation));
	glMultMatrixf( temp.pData );
	glScalef( pTransform->scale.x,pTransform->scale.y,pTransform->scale.z );

	float MD[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, MD);
	Matrix4x4 modelMatrix = Matrix4x4( MD );

	glPopMatrix();*/
	Matrix4x4 translation;
	translation.setTranslation( pTransform->position );
	Matrix4x4 rotation;
	rotation.setRotation( pTransform->rotation );
	Matrix4x4 scale;
	scale.setScale( pTransform->scale );
	Matrix4x4 modelMatrix = translation * rotation * scale;

	pushModelMatrix( modelMatrix );
}
/*
void	glMainSystem::Transform ( const Matrix2x2& inMatrix )
{
	//pushModelMatrix( );
}*/

void	glMainSystem::Transform ( const Matrix4x4& inMatrix )
{
	pushModelMatrix( inMatrix );
}


void	glMainSystem::Translate ( Vector3d const& vPosition )
{
	if ( !hasModelMatrix() )
	{
		//throw core::DeprecatedCallException();
		/*
		glPushMatrix();		//TODO: The fuck is this?
		glLoadIdentity();

		glTranslatef( vPosition.x,vPosition.y,vPosition.z );

		float MD[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, MD);
		Matrix4x4 modelMatrix = Matrix4x4( MD );

		glPopMatrix();

		pushModelMatrix( !modelMatrix );*/
		Matrix4x4 modelMatrix = Matrix4x4();
		Matrix4x4 offsetMatrix;
		offsetMatrix.setTranslation( vPosition );
		modelMatrix = modelMatrix * offsetMatrix;
		pushModelMatrix( !modelMatrix );
	}
	else
	{
		Matrix4x4 modelMatrix = !(getModelMatrix());
		Matrix4x4 offsetMatrix;
		offsetMatrix.setTranslation( vPosition );
		modelMatrix = modelMatrix * offsetMatrix;
		popModelMatrix();
		pushModelMatrix( !modelMatrix );
	}
}
