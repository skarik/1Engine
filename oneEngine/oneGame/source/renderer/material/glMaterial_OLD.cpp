
// Include
#include "glMaterial.h"
// Includes for shaders
#include "CToBeSeen.h"
#include "CRenderableObject.h"
#include "CCamera.h"
#include "CTime.h"
#include "CLight.h"
#include "CRTCamera.h"
#include "CRenderTexture.h"
#include "CGameSettings.h"
// Include for naming
#include <sstream>
using std::ostringstream;

// Define the default material
glMaterial GLdefaultMaterial;
glMaterial* glMaterial::current = &GLdefaultMaterial;

// Define static variables
short glMaterial::iCurrentTextureCount = 0;
vector<glMaterial*>	glMaterial::materialList;

//==Constructor and Destructor==
glMaterial::glMaterial ( void )
{
	bSaveMe		= true;
	iRefNumber	= 1;

	m_initialized_mesh	= false;

	useTexture			= false;
	useColors			= false;
	useBlendedDiffuse	= false;
	useVertexColors		= false;
	useLighting			= true;
	useSpecular			= false;

	isTransparent		= false;
	useTwoPassAlpha		= false;
	useDepthMask		= true;
	useAlphaTest		= false;

	useSkinning			= false;

	isScreenShader		= false;

	diffuse		= Color( 1,1,1,1 );
	emissive	= Color( 0,0,0,1 );

	iBlendMode		= BM_NORMAL;
	iLightingMode	= LI_NORMAL;
	iBindMode		= BN_DEFAULT;
	iFaceMode		= FM_FRONT;

	/*if ( ActiveGameSettings ) {
		pShader = new glShader( ".res/shaders/sys/default.glsl" );
	}
	else {*/
		pShader = NULL;
	//}
	pTextures = new CTexture* [8];
	for ( int i = 0; i < 8; i++ )
		pTextures[i] = NULL;

	iRenderQueue = 500;
	iRenderQueuePrev	= iRenderQueue;
	//bNeedsReorder		= false;

	sDrawHint	= "diffuse";
	sMaterialFilename = "default";
	{
		++iCurrentTextureCount;
		//materialList.push_back( this );
	}
}

glMaterial::~glMaterial ( void )
{
	if ( pShader != NULL )
	{
		/*pShader->DecrementReference();
		if ( pShader->ReferenceCount() == 0 )
		{
			delete pShader;
		}*/
		pShader->ReleaseReference();
	}
	pShader = NULL;

	// TODO: Free texture references
	if ( pTextures != NULL )
		delete [] pTextures;
	pTextures = NULL;

	{
		--iCurrentTextureCount;
		/*vector<glMaterial*>::iterator it = find( materialList.begin(), materialList.end(), this );
		if ( it != materialList.end() ) {
			materialList.erase( it );
		}*/
	}
}


glMaterial& glMaterial::operator= ( glMaterial const* glmat ) {
	return operator= ( *glmat );
}
glMaterial& glMaterial::operator= ( glMaterial const& source )
{
	useTexture			= source.useTexture;
	useColors			= source.useColors;
	useBlendedDiffuse	= source.useBlendedDiffuse;
	useVertexColors		= source.useVertexColors;
	useLighting			= source.useLighting;
	useSpecular			= source.useSpecular;

	isTransparent		= source.isTransparent;
	useTwoPassAlpha		= source.useTwoPassAlpha;
	useDepthMask		= source.useDepthMask;
	useAlphaTest		= source.useAlphaTest;

	useSkinning			= source.useSkinning;

	isScreenShader		= source.isScreenShader;

	diffuse		= source.diffuse;
	emissive	= source.emissive;

	iBlendMode		= source.iBlendMode;
	iLightingMode	= source.iLightingMode;
	iBindMode		= source.iBindMode;
	iFaceMode		= source.iFaceMode;

	pShader   = source.pShader;
	for ( int i = 0; i < 8; i++ )
		pTextures[i] = source.pTextures[i];

	iRenderQueue = source.iRenderQueue;
	iRenderQueuePrev	= source.iRenderQueuePrev;

	sDrawHint	= source.sDrawHint;
	sMaterialFilename = source.sMaterialFilename;

	m_initialized_mesh = source.m_initialized_mesh;

	return *this;
}
glMaterial* glMaterial::Copy ( void ) const
{
	glMaterial* mat_copy = new glMaterial;
	(*mat_copy) = this;
	return mat_copy;
}


//==Render Queue==
void glMaterial::calculateRenderQueue ( void )
{
	if ( iBlendMode == BM_ADD ) {
		isTransparent = true;	// Additive is always transparent
	}

	iRenderQueue = 500;
	if (( isTransparent )||( iBlendMode == BM_ADD )) {
		iRenderQueue += 40;
	}
	if ( !useDepthMask ) {
		iRenderQueue += 50;
	}
	if ( isScreenShader ) { // Screen shaders go last
		iRenderQueue += 300;
	}

	if ( iRenderQueuePrev != iRenderQueue ) {
		iRenderQueuePrev = iRenderQueue;
		//bNeedsReorder = true;
		//CToBeSeen::pActive->ReorderList();
	}
}
#include "CTimeProfiler.h"
//==Bind and Unbind==
void glMaterial::bind ( void )
{
	calculateRenderQueue();

	if ( glMaterial::current == this )
	{
		//cout << "Double" << endl; //
		return;
	}
	else
	{
		//cout << "New" << endl; //
		//cout << this << " versus " << glMaterial::current << " const: " << &GLdefaultMaterial << endl;
		glMaterial::current->unbind();
	}

	glMaterial::current = this;

	if ( iFaceMode == FM_FRONT )
	{
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
	}
	else if ( iFaceMode == FM_FRONTANDBACK )
	{
		glDisable( GL_CULL_FACE );
	}
	else if ( iFaceMode == FM_BACK )
	{
		glEnable( GL_CULL_FACE );
		glCullFace( GL_FRONT );
	}

	if ( iBlendMode == BM_NORMAL )
	{
		glEnable( GL_BLEND );
		if ( iBlendMode == BM_NORMAL ) {
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}

		if ( !isTransparent )
		{
			glDepthMask( GL_TRUE );

			if ( useAlphaTest )
			{
				glAlphaFunc(GL_GREATER, 0.6f);
				glEnable(GL_ALPHA_TEST);
			}
			else
			{
				glDisable(GL_ALPHA_TEST);
			}
		}
		else
		{
			if ( useDepthMask || useTwoPassAlpha )
			{
				glDepthMask( GL_TRUE );
				glAlphaFunc(GL_GREATER, 0.5f);
				glEnable(GL_ALPHA_TEST);
			}
			else
			{
				glDepthMask( GL_FALSE );
				glDisable(GL_ALPHA_TEST);
			}
		}

		GL.EnableFog();
	}
	else if ( iBlendMode == BM_ADD )
	{
		glEnable( GL_BLEND );
		glDisable( GL_ALPHA_TEST );
		//glBlendFunc( GL_ONE, GL_ONE );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		glDepthMask( GL_FALSE );

		GL.DisableFog();
	}
	else if ( iBlendMode == BM_SOFT_ADD )
	{
		glEnable( GL_BLEND );
		glDisable( GL_ALPHA_TEST );
		//glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
		glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
		glDepthMask( GL_FALSE );

		GL.DisableFog();
	}


	if ( useLighting )
		glEnable( GL_LIGHTING );
	else
		glDisable( GL_LIGHTING );

	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders )) {
		bind_default();
	}
	else
	{
		//TimeProfiler.BeginTimeProfile( "materialBind" );
		try {
			bind_shader();
		}
		catch ( std::exception& e )
		{
			cout << "Error in binding: " << e.what() << endl;
		}
		//TimeProfiler.EndPrintTimeProfile( "materialBind" );
	}

}

void glMaterial::bind_default ( void )
{
	// Fixed-function
	glUseProgram( 0 );

	if ( useColors )
	{
		//if (( useLighting )||( useTexture ))
		//{
		if ( !useBlendedDiffuse )
		{
			glEnable( GL_COLOR_MATERIAL );
			glColorMaterial( GL_FRONT, GL_AMBIENT );
			glColor4fv( diffuse.start_point() );
			glMaterialfv( GL_FRONT, GL_EMISSION, emissive.start_point() );
			//glMaterialfv( GL_FRONT, GL_AMBIENT, Color(0,0,0).start_point() );
			glMaterialfv( GL_FRONT, GL_AMBIENT, RenderSettings.ambientColor.start_point() );
		}
		else
		{
			glEnable( GL_COLOR_MATERIAL );
			glColorMaterial( GL_FRONT, GL_AMBIENT );
			glColor4f( 1.0f,1.0f,1.0f,1.0f );
			glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse.start_point() );
			glMaterialfv( GL_FRONT, GL_EMISSION, emissive.start_point() );
			glMaterialfv( GL_FRONT, GL_AMBIENT, RenderSettings.ambientColor.start_point() );
		}
		//}
		/*else
		{
			glEnable( GL_COLOR_MATERIAL );
			glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse.start_point() );
			glMaterialfv( GL_FRONT, GL_EMISSION, emissive.start_point() );
		}*/
	}
	else
	{
		if ( useLighting )
		{
			glDisable( GL_COLOR_MATERIAL );
			glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse.start_point() );
			glMaterialfv( GL_FRONT, GL_EMISSION, emissive.start_point() );
			//glMaterialfv( GL_FRONT, GL_AMBIENT, Color(0,0,0).start_point() );
			glMaterialfv( GL_FRONT, GL_AMBIENT, RenderSettings.ambientColor.start_point() );
		}
		else
		{
			glColor4fv( diffuse.start_point() );
		}
	}

/*	glEnable( GL_TEXTURE_2D );
	glEnable( GL_TEXTURE_3D );
	glEnable( GL_TEXTURE_CUBE_MAP );
	*/
	if ( useTexture )
	{
		glActiveTexture( GL_TEXTURE0 );
		if ( pTextures[0] ) {
			//if ( pTextures[0]->GetType() == Texture2D ) {
			//}
			//else if ( pTextures[0]->GetType() == Texture3D ) {
			//}
			pTextures[0]->Bind(); 
		}
	}
	else
	{
		glActiveTexture( GL_TEXTURE0 );
		//glDisable( GL_TEXTURE_2D );
		//glDisable( GL_TEXTURE_3D );
		//if ( pTextures[0] )
			CTexture::Unbind(0);
	}
}
void glMaterial::bind_shader ( void )
{
	// todo: bind textures and camera pos
	pShader->begin();

	unsigned int program = pShader->get_program(); // PROBLEM
	int uniformLocation, uniformLocationB;
	
	// == Bind and Send Textures ==
	{
		iCurrentTextureCount = 0;

		// 0 index always goes to the diffuseTexture slot
		uniformLocation = pShader->get_uniform_location( "textureDiffuse" );
		if ( uniformLocation >= 0 )
		{
			glActiveTexture( GL_TEXTURE0 );
			if (( pTextures[0] )&&( useTexture ))
			{
				pTextures[0]->Bind();
				// Increment used texture count
				++iCurrentTextureCount;
			}
			else {
				CTexture::Unbind(0);
			}
			glUniform1i( uniformLocation, 0 );
		}
	}
	// Loop through the rest of the textures
	if ( useTexture ) {
		for ( unsigned int i = 1; i < 8; ++i ) {
			if ( pTextures[i] != NULL ) {
				// Get the uniform location
				string uniformname_sh1 = "textureSampler0";
				uniformname_sh1[14] = '1' + (i-1);
				uniformLocation = pShader->get_uniform_location( uniformname_sh1 );
				// If the spot exists, then use it
				if ( uniformLocation >= 0 )
				{
					glActiveTexture( GL_TEXTURE0+i );
					if ( pTextures[i] )
					{
						pTextures[i]->Bind();
						// Increment used texture count
						++iCurrentTextureCount;
					}
					else {
						CTexture::Unbind(0);
					}
					glUniform1i( uniformLocation, i );
				}
			}
		}
	}

	// == Send Colors ==
	if ( useColors )
	{
		glSecondaryColor3fv( emissive.start_point() );

		glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse.start_point() );
		glMaterialfv( GL_FRONT, GL_EMISSION, emissive.start_point() );
		glMaterialfv( GL_FRONT, GL_AMBIENT, RenderSettings.ambientColor.start_point() );
	}
	else
	{
		//glColor4fv( diffuse.start_point() );
		glSecondaryColor3fv( emissive.start_point() );

		glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse.start_point() );
		glMaterialfv( GL_FRONT, GL_EMISSION, emissive.start_point() );
		glMaterialfv( GL_FRONT, GL_AMBIENT, RenderSettings.ambientColor.start_point() );
	}
	uniformLocation = pShader->get_uniform_location( "sys_DiffuseColor" );
	if ( uniformLocation >= 0 ) {
		glUniform4f( uniformLocation, diffuse.red, diffuse.green, diffuse.blue, diffuse.alpha );
	}
	uniformLocation = pShader->get_uniform_location( "sys_EmissiveColor" );
	if ( uniformLocation >= 0 ) {
		glUniform3f( uniformLocation, emissive.red, emissive.green, emissive.blue );
	}

	// == Send Lights ==
	if ( useLighting )
	{
		// Get the light list
		vector<CLight*>* lightList = CLight::GetActiveLightList();
		// Set the maximum value to iterate through
		unsigned int maxVal = lightList->size();
		maxVal = min( maxVal, (unsigned int)(RenderSettings.maxLights-1) );

		char uniformname1 [] = "sys_LightProperties[x]";
		char uniformname2 [] = "sys_LightPosition[x]";
		char uniformname3 [] = "sys_LightColor[x]";

		char uniformname_sh1 [] = "sys_LightShadowInfo[x]";
		char uniformname_sh2 [] = "sys_LightMatrix[x]";
		char uniformname_sh3 [] = "textureShadowx";
		// Loop through all the lights in the list
		for ( unsigned int i = 0; i < maxVal; i++ )
		{
			// Put the values into those lights
			//ostringstream sstr_lr;
			//sstr_lr << "sys_LightProperties[" << i+1 << "]";
			//uniformLocation = pShader->get_uniform_location( sstr_lr.str() );
			
			uniformname1[20] = '1' + i;
			uniformLocation = pShader->get_uniform_location( string(uniformname1) );

			/*sstr_lr.clear();
			sstr_lr.str("");
			sstr_lr << "sys_LightPosition[" << i+1 << "]";
			uniformLocationB = pShader->get_uniform_location( sstr_lr.str() );*/
			
			uniformname2[18] = '1' + i;
			uniformLocationB = pShader->get_uniform_location( string(uniformname2) );
			if (( uniformLocation >= 0 )&&( uniformLocationB >= 0 ))
			{
				glUniform4f( uniformLocation,
					(1.0f/(*lightList)[i]->range),
					(*lightList)[i]->falloff,
					(*lightList)[i]->pass, 1.0f );
				glUniform4f( uniformLocationB,
					(*lightList)[i]->GetSendVector().red,
					(*lightList)[i]->GetSendVector().green,
					(*lightList)[i]->GetSendVector().blue,
					(*lightList)[i]->GetSendVector().alpha );
			}

			//sstr_lr.clear();
			//sstr_lr.str("");
			//sstr_lr << "sys_LightColor[" << i+1 << "]";
			//uniformLocationB = pShader->get_uniform_location( sstr_lr.str() );
			
			uniformname3[15] = '1' + i;
			uniformLocationB = pShader->get_uniform_location( string(uniformname3) );
			if (( uniformLocation >= 0 )&&( uniformLocationB >= 0 ))
			{
				glUniform4f( uniformLocationB,
					(*lightList)[i]->diffuseColor.red,
					(*lightList)[i]->diffuseColor.green,
					(*lightList)[i]->diffuseColor.blue,
					(*lightList)[i]->diffuseColor.alpha );
			}

			if ( ActiveGameSettings->b_ro_EnableShadows )
			{
				// Send the shadow vals
				//sstr_lr.clear();
				//sstr_lr.str("");
				//sstr_lr << "sys_LightShadowInfo[" << i+1 << "]";
				//uniformLocation = pShader->get_uniform_location( sstr_lr.str() );

				uniformname_sh1[20] = '1' + i;
				uniformLocation = pShader->get_uniform_location( string(uniformname_sh1) );
				if ( uniformLocation >= 0 )
				{
					if ( !(*lightList)[i]->generateShadows )
					{
						glUniform4f( uniformLocation, 0,0,0,0 ); // Then don't send shadow info
					}
					else
					{
						//sstr_lr.clear();
						//sstr_lr.str("");
						//sstr_lr << "sys_LightMatrix[" << i+1 << "]";
						//uniformLocationB = pShader->get_uniform_location( sstr_lr.str() );

						uniformname_sh2[16] = '1' + i;
						uniformLocationB = pShader->get_uniform_location( string(uniformname_sh2) );
						if ( uniformLocationB >= 0 )
						{
							// Set shadow params
							glUniform4f( uniformLocation, 1,
								(*lightList)[i]->shadowStrength, 0,0 );
							// Set shadow matrix
							if ( (*lightList)[i]->GetShadowCamera() )
							{
								//cout << "New matx" << endl;
								glUniformMatrix4fv( uniformLocationB, 1,false, (!((*lightList)[i]->GetShadowCamera()->textureMatrix))[0] );
							}
						
							// Set shadow texture
							//sstr_lr.clear();
							//sstr_lr.str("");
							//sstr_lr << "textureShadow[" << i+1 << "]";
							//uniformLocation = pShader->get_uniform_location( sstr_lr.str() );
							uniformname_sh3[13] = '1' + i;
							//if ( iCurrentTextureCount != 1 )
								uniformLocation = pShader->get_uniform_location( string(uniformname_sh3) );
							//else
							//	uniformLocation = pShader->get_uniform_location( string("textureShadow1") );
							if ( uniformLocation >= 0 )
							{
								glActiveTexture( GL_TEXTURE0+iCurrentTextureCount );
								if ( (*lightList)[i]->GetShadowTexture() )
								{
									(*lightList)[i]->GetShadowTexture()->BindDepth();
									//(*lightList)[i]->GetShadowTexture()->Bind();
								}
								glUniform1i( uniformLocation, iCurrentTextureCount );

								// Increment texture count.
								++iCurrentTextureCount;
							}
						}
					}
					// end if light.generate_shadows
				}
			}
			
		}

		char uniformnameA [] = "sys_LightProperties[x]";
		char uniformnameB [] = "sys_LightColor[x]";
		char uniformnameC [] = "sys_LightShadowInfo[x]";
		// Loop through all the lights NOT in the list, and set their range to zero
		for ( int i = maxVal; i < RenderSettings.maxLights-1; i++ )
		{
			// Put the range into those lights
			/*ostringstream sstr_lr;
			sstr_lr << "sys_LightProperties[" << i+1 << "]";
			uniformLocation = pShader->get_uniform_location( sstr_lr.str() );*/
			
			uniformnameA[20] = '1' + i;
			uniformLocation = pShader->get_uniform_location( string(uniformnameA) );
			if ( uniformLocation >= 0 )
			{
				glUniform4f( uniformLocation, 0.001f, 1.0f, 0.0f, 1.0f );
			}

			/*sstr_lr.clear();
			sstr_lr.str("");
			sstr_lr << "sys_LightColor[" << i+1 << "]";
			uniformLocationB = pShader->get_uniform_location( sstr_lr.str() );*/
			
			uniformnameB[15] = '1' + i;
			uniformLocationB = pShader->get_uniform_location( string(uniformnameB) );
			if (( uniformLocation >= 0 )&&( uniformLocationB >= 0 ))
			{
				glUniform4f( uniformLocationB, 0.0f,0.0f,0.0f,0.0f );
			}

			// Send the shadow vals
			/*sstr_lr.clear();
			sstr_lr.str("");
			sstr_lr << "sys_LightShadowInfo[" << i+1 << "]";
			uniformLocation = pShader->get_uniform_location( sstr_lr.str() );*/
			
			uniformnameC[20] = '1' + i;
			uniformLocation = pShader->get_uniform_location( string(uniformnameC) );
			if ( uniformLocation >= 0 )
			{
				glUniform4f( uniformLocation, 0,0,0,0 );
			}
		}
		// Set the ambient color
		{
			uniformLocation = pShader->get_uniform_location( "sys_LightAmbient" );
			if ( uniformLocation >= 0 )
			{
				glUniform4f( uniformLocation,
					RenderSettings.ambientColor.red,
					RenderSettings.ambientColor.green,
					RenderSettings.ambientColor.blue, 1.0f );
			}
		}
	}
	else
	{
		char uniformnameA [] = "sys_LightProperties[x]";
		char uniformnameB [] = "sys_LightColor[x]";
		char uniformnameC [] = "sys_LightShadowInfo[x]";
		// Loop through all the lights valid, and zero them out
		for ( int i = 0; i < RenderSettings.maxLights-1; i++ )
		{
			uniformnameA[20] = '1' + i;
			uniformLocation = pShader->get_uniform_location( string(uniformnameA) );
			if ( uniformLocation >= 0 ) {
				glUniform4f( uniformLocation, 0.001f, 1.0f, 0.0f, 1.0f );
			}
			
			uniformnameB[15] = '1' + i;
			uniformLocationB = pShader->get_uniform_location( string(uniformnameB) );
			if (( uniformLocation >= 0 )&&( uniformLocationB >= 0 )) {
				glUniform4f( uniformLocationB, 0.0f,0.0f,0.0f,0.0f );
			}

			uniformnameC[20] = '1' + i;
			uniformLocation = pShader->get_uniform_location( string(uniformnameC) );
			if ( uniformLocation >= 0 ) {
				glUniform4f( uniformLocation, 0,0,0,0 );
			}
		}
		// Fullbright on the ambient, though
		uniformLocation = pShader->get_uniform_location( "sys_LightAmbient" );
		if ( uniformLocation >= 0 ) {
			glUniform4f( uniformLocation, 1.0f, 1.0f, 1.0f, 1.0f );
		}
	}

	
	// Set the clear color
	{
		uniformLocation = pShader->get_uniform_location( "sys_ClearColor" );
		if ( uniformLocation >= 0 )
		{
			glUniform4f( uniformLocation,
				RenderSettings.clearColor.red,
				RenderSettings.clearColor.green,
				RenderSettings.clearColor.blue, 1.0f );
		}
	}

	// TODO: Light space matrix.


	// TODO: add option
	//glBindAttribLocation( program, 76, "sys_BoneWeights" );
	//GLint vertexLoc;
	//vertexLoc = glGetAttribLocation( program,"sys_BoneWeights" );

	glActiveTexture( GL_TEXTURE0 ); // Go back to zero texture


	// Bind the queued entries
	/*while ( !uniformQueueFloats.empty() ) {
		UniformEntry<float> val = uniformQueueFloats.back();
		uniformQueueFloats.pop_back();
		setUniform( val.id, val.value );
	}*/
	//uniformMapFloats
	// Bind the stored entries
	/*for ( unordered_map<int,UniformEntry<float>>::iterator entry = uniformMapFloats.begin(); entry != uniformMapFloats.end(); ++entry )
	{
		glUniform1f( entry->first, entry->second.value );
	}*/
}
void glMaterial::setShaderConstants ( CRenderableObject* pRenderableObject )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}
	if ( pRenderableObject != NULL )
	{
		unsigned int program = pShader->get_program();
		int uniformLocation;

		// Find the uniforms

		// Model Matrix (object transform) matrix
		//uniformLocation = glGetUniformLocation( program, "sys_ModelMatrix" );
		uniformLocation = pShader->get_uniform_location( "sys_ModelMatrix" );
		if ( uniformLocation >= 0 )
		{
			Matrix4x4 modelTransform = pRenderableObject->transform.GetTransformMatrix();
			glUniformMatrix4fv( uniformLocation, 1,false, (!modelTransform)[0] );

			//uniformLocation = glGetUniformLocation( program, "sys_ModelMatrixInverse" );
			uniformLocation = pShader->get_uniform_location( "sys_ModelMatrixInverse" );
			if ( uniformLocation >= 0 )
			{
				modelTransform = modelTransform.inverse();
				glUniformMatrix4fv( uniformLocation, 1,false, (!modelTransform)[0] );
			}
		}
		//uniformLocation = glGetUniformLocation( program, "sys_ModelRotationMatrix" );
		uniformLocation = pShader->get_uniform_location( "sys_ModelRotationMatrix" );
		if ( uniformLocation >= 0 )
		{
			Matrix4x4 modelRotation;
			modelRotation.setRotation( pRenderableObject->transform.rotation );
			glUniformMatrix4fv( uniformLocation, 1,false, (!modelRotation)[0] );
		}
		//sys_ModelViewProjectionMatrix
		uniformLocation = pShader->get_uniform_location( "sys_ModelViewProjectionMatrix" );
		if ( uniformLocation >= 0 )
		{
			Matrix4x4 modelViewProjection;
			//modelViewProjection.setRotation( pRenderableObject->transform.rotation );
			modelViewProjection = (!pRenderableObject->transform.GetTransformMatrix()) * CCamera::activeCamera->viewTransform * CCamera::activeCamera->projTransform;
			glUniformMatrix4fv( uniformLocation, 1,false, (modelViewProjection)[0] );
		}

		// Camera Position
		//uniformLocation = glGetUniformLocation( program, "sys_WorldCameraPos" );
		uniformLocation = pShader->get_uniform_location( "sys_WorldCameraPos" );
		if ( uniformLocation >= 0 )
		{
			Vector3d cameraPos = CCamera::activeCamera->transform.position;
			glUniform3fv( uniformLocation, 1, &(cameraPos.x) );
		}

		// Time variables
		//uniformLocation = glGetUniformLocation( program, "sys_Time" );
		uniformLocation = pShader->get_uniform_location( "sys_Time" );
		if ( uniformLocation >= 0 )
		{
			float timevars [4];
			timevars[1] = CTime::currentTime;
			timevars[0] = timevars[1]/20.0f;
			timevars[2] = timevars[1]*2.0f;
			timevars[3] = timevars[1]*3.0f;
			glUniform4fv( uniformLocation, 1, timevars );
		}
		//uniformLocation = glGetUniformLocation( program, "sys_SinTime" );
		uniformLocation = pShader->get_uniform_location( "sys_SinTime" );
		if ( uniformLocation >= 0 )
		{
			float timevars [4];
			timevars[0] = sin( CTime::currentTime / 8.0f );
			timevars[1] = sin( CTime::currentTime / 4.0f );
			timevars[2] = sin( CTime::currentTime / 2.0f );
			timevars[3] = sin( CTime::currentTime / 1.0f );
			glUniform4fv( uniformLocation, 1, timevars );
		}
		//uniformLocation = glGetUniformLocation( program, "sys_CosTime" );
		uniformLocation = pShader->get_uniform_location( "sys_CosTime" );
		if ( uniformLocation >= 0 )
		{
			float timevars [4];
			timevars[0] = cos( CTime::currentTime / 8.0f );
			timevars[1] = cos( CTime::currentTime / 4.0f );
			timevars[2] = cos( CTime::currentTime / 2.0f );
			timevars[3] = cos( CTime::currentTime / 1.0f );
			glUniform4fv( uniformLocation, 1, timevars );
		}

	}
}

#include "CModelData.h"
void glMaterial::setShaderAttributesDefault ( void )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader attributes for inactive material" << endl;
		return;
	}
	
	unsigned int program = pShader->get_program();
	int attributeLocation;

	attributeLocation = pShader->get_attrib_location( "mdl_Vertex" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*0) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_TexCoord" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*3) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_Color" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			4, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*15) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_Normal" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*6) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_Tangents" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*9) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_Binormals" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*12) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_BoneWeights" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			4, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*20) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_BoneIndices" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribIPointer( attributeLocation,
			4, GL_UNSIGNED_BYTE, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*19) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_TexCoord2" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*24) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_TexCoord3" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*27) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_TexCoord4" );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			2, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*30) );
	}
}
void glMaterial::setAttribute ( string const& sAttributeName, uint count, GLsizei stride, const void* offset )
{
	int attributeLocation = pShader->get_attrib_location( sAttributeName );
	if ( attributeLocation >= 0 )
	{
		glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			count, GL_FLOAT, false, stride,
			offset );
	}
}

void glMaterial::initializeMeshAttributes ( void )
{
	m_initialized_mesh = true;
	setShaderAttributesDefault();
}
void glMaterial::initializeParticleMeshAttributes ( void )
{
	m_initialized_mesh = true;
	forwardParticleMeshAttributes( true );
}
void glMaterial::forwardMeshAttributes ( void )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader attributes for inactive material" << endl;
		return;
	}
	
	unsigned int program = pShader->get_program();
	int attributeLocation;

	attributeLocation = pShader->get_attrib_location( "mdl_Vertex" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*0) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_TexCoord" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*3) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_Color" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribPointer( attributeLocation,
			4, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*15) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_Normal" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, true, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*6) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_Tangents" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*9) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_Binormals" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*12) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_BoneWeights" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribPointer( attributeLocation,
			4, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*20) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_BoneIndices" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribIPointer( attributeLocation,
			4, GL_UNSIGNED_BYTE, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*19) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_TexCoord2" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*24) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_TexCoord3" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*27) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_TexCoord4" );
	if ( attributeLocation >= 0 )
	{
		glVertexAttribPointer( attributeLocation,
			2, GL_FLOAT, false, sizeof(CModelVertex),
			((char*)0) + (sizeof(float)*30) );
	}
}
bool glMaterial::hasInitializedMeshAttributes ( void ) const
{
	return m_initialized_mesh;
}
void glMaterial::forwardParticleMeshAttributes ( bool initialize )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader attributes for inactive material" << endl;
		return;
	}
	
	unsigned int program = pShader->get_program();
	int attributeLocation;

	attributeLocation = pShader->get_attrib_location( "mdl_Vertex" );
	if ( attributeLocation >= 0 )
	{
		if ( initialize ) glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CParticleVertex),
			((char*)0) + (sizeof(float)*0) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_TexCoord" );
	if ( attributeLocation >= 0 )
	{
		if ( initialize ) glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			2, GL_FLOAT, false, sizeof(CParticleVertex),
			((char*)0) + (sizeof(float)*6) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_Color" );
	if ( attributeLocation >= 0 )
	{
		if ( initialize ) glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			4, GL_FLOAT, false, sizeof(CParticleVertex),
			((char*)0) + (sizeof(float)*8) );
	}
	attributeLocation = pShader->get_attrib_location( "mdl_Normal" );
	if ( attributeLocation >= 0 )
	{
		if ( initialize ) glEnableVertexAttribArray( attributeLocation );
		glVertexAttribPointer( attributeLocation,
			3, GL_FLOAT, false, sizeof(CParticleVertex),
			((char*)0) + (sizeof(float)*3) );
	}
}

void glMaterial::unbind ( void )
{
	if (( pShader )&&( ActiveGameSettings->b_ro_EnableShaders ))
	{
		pShader->end();

		// For every single texture, should probably unbind it
		for ( short i = 0; i < iCurrentTextureCount; ++i )
		{
			glActiveTexture( GL_TEXTURE0+i );
			CTexture::Unbind(0);
		}
		glActiveTexture( GL_TEXTURE0 );
		// Reset used texture count back to zero
		iCurrentTextureCount = 0;
	}
	if ( glMaterial::current != &GLdefaultMaterial )
	{
		glMaterial::current = &GLdefaultMaterial;
		GLdefaultMaterial.bind();
	}
}

// == Texture Binding ==
void glMaterial::bindTexture ( CTexture* pInTexture, string const& sUniformName )
{
	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		glUniform1i( uniformLocation, iCurrentTextureCount );
		glActiveTexture( GL_TEXTURE0+iCurrentTextureCount );
		/*if ( (*lightList)[i]->GetShadowTexture() )
		{
			(*lightList)[i]->GetShadowTexture()->BindDepth();
			(*lightList)[i]->GetShadowTexture()->Bind();
		}*/
		pInTexture->Bind();

		// Increment texture count.
		++iCurrentTextureCount;
	}
}
void glMaterial::bindDepth ( CRenderTexture* pInTexture, string const& sUniformName )
{
	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		glUniform1i( uniformLocation, iCurrentTextureCount );
		glActiveTexture( GL_TEXTURE0+iCurrentTextureCount );
		pInTexture->BindDepth();

		// Increment texture count.
		++iCurrentTextureCount;
	}
}
void glMaterial::bindTextureBuffer ( GLuint iInTexture, string const& sUniformName )
{
	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		glUniform1i( uniformLocation, iCurrentTextureCount );
		glActiveTexture( GL_TEXTURE0+iCurrentTextureCount );

		//glEnable( GL_TEXTURE_BUFFER );
		glBindTexture( GL_TEXTURE_BUFFER, iInTexture );

		// Increment texture count.
		++iCurrentTextureCount;
	}
}

// ==Shader Constants==
void glMaterial::setUniform ( string const& sUniformName, float const fInput )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}//uniformMapFloats
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		/*UniformEntry<float> newEntry;
		newEntry.id = sUniformName;
		newEntry.value = fInput;
		uniformQueueFloats.push_back( newEntry );
		return;*/
	}

	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		/*if ( glMaterial::current != this )
		{
			UniformEntry<float> newEntry;
			newEntry.id = sUniformName;
			newEntry.value = fInput;
			uniformMapFloats[uniformLocation] = newEntry;
		}
		else
		{*/
			glUniform1f( uniformLocation, fInput );
		//}
	}
}
void glMaterial::setUniform ( string const& sUniformName, Vector2d const& vInput )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}

	//unsigned int program = pShader->get_program();
	//int uniformLocation = glGetUniformLocation( program, sUniformName.c_str() );
	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		glUniform2f( uniformLocation, vInput.x, vInput.y );
	}
	/*else
	{
		cout << "Warning in: " << this << ": can't find shader uniform '" << sUniformName << "'" << endl;
	}*/
}
void glMaterial::setUniform ( string const& sUniformName, Vector3d const& vInput )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}

	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		glUniform3f( uniformLocation, vInput.x, vInput.y, vInput.z );
	}
}
void glMaterial::setUniform ( string const& sUniformName, Color const& cInput )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}

	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		//glUniform4fv( uniformLocation, 1, cInput.start_point() );
		glUniform4f( uniformLocation, cInput.red, cInput.green, cInput.blue, cInput.alpha );
	}
}
void glMaterial::setUniform ( string const& sUniformName, Matrix4x4 const& matxInput )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}

	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		glUniformMatrix4fv( uniformLocation, 1,false, matxInput[0]  );
	}
}

void glMaterial::setUniformV( string const& sUniformName, unsigned int count, const Matrix4x4 * pMatxInput )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}

	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		glUniformMatrix4fv( uniformLocation, count,false, pMatxInput[0][0]  );
	}
}
void glMaterial::setUniformV( string const& sUniformName, unsigned int count, const Matrix3x3 * pMatxInput )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}

	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		glUniformMatrix3fv( uniformLocation, count,false, pMatxInput[0][0]  );
	}
}

void glMaterial::setUniformV ( string const& sUniformName, unsigned int count, const Vector3d* pVInput )
{
	if (( pShader == NULL )||( !ActiveGameSettings->b_ro_EnableShaders ))
	{
		return;
	}
	if ( glMaterial::current != this )
	{
		cout << "Warning in: " << this << ": can't set shader constants for inactive material" << endl;
		return;
	}

	int uniformLocation = pShader->get_uniform_location( sUniformName );
	if ( uniformLocation >= 0 )
	{
		glUniform3fv( uniformLocation, count, &(pVInput[0].x)  );
	}
}


//==Load a Texture to use==
void glMaterial::loadTexture ( const string& sInFilename )
{
	/*if ( pTextures != NULL )
		delete [] pTextures;
	pTextures = new CTexture ( sInFilename );

	useTexture = true;*/
	pTextures[0] = new CTexture ( sInFilename );
	useTexture = true;
}

//==Set a texture to use==
void glMaterial::setTexture ( CTexture* pInTexture )
{
	/*if ( pTextures != NULL )
		delete [] pTextures;
	//pTextures = new CTexture [1];

	pTextures = pInTexture;

	useTexture = true;*/
	pTextures[0] = pInTexture;
	useTexture = true;
}

//==Set a Shader==
void glMaterial::setShader ( glShader * newShader )
{
	if ( pShader != NULL )
		pShader->ReleaseReference();
	//	pShader->DecrementReference();
	pShader = newShader;
	//pShader->AddReference();
}