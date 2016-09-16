
#include "CLipsyncSequence.h"

#include "core/math/Math.h"
#include "core/system/io/CBinaryFile.h"
#include "core/system/io/FileUtils.h"

#include "renderer/logic/model/morpher/CMorpher.h"

#include "after/entities/character/CMccCharacterModel.h"


void CLipsyncSequence::Load ( const std::string& n_morphfile )
{
	if ( IO::FileExists( n_morphfile ) )
	{
		m_keyframes.clear();

		// Load in the morphs
		CBinaryFile file;
		file.Open( n_morphfile.c_str(), CBinaryFile::IO_READ );

		uint32_t frameCount = file.ReadUInt32();
		for ( uint i = 0; i < frameCount; ++i )
		{
			keyframe_t keyframe;
			file.ReadData( (char*)(&keyframe), sizeof(keyframe_t) );
			m_keyframes.push_back( keyframe );
		}
	}
}
void CLipsyncSequence::Save ( const string& n_morphfile )
{
	CBinaryFile file;
	file.Open( n_morphfile.c_str(), CBinaryFile::IO_WRITE );

	file.WriteUInt32( m_keyframes.size() );
	for ( uint i = 0; i < m_keyframes.size(); ++i )
	{
		file.WriteData( (char*)(&(m_keyframes[i])), sizeof(keyframe_t) );
	}
}


void CLipsyncSequence::UpdateMorphs ( void )
{
	// If no keyframes, leave
	if ( m_keyframes.empty() ) {
		return;
	}

	Matrix4x4 startBlend, endBlend, targetBlend;
	Real zeroValues [] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	targetBlend = Matrix4x4(zeroValues);
	startBlend = targetBlend;
	endBlend = targetBlend;

	// Get the morpher
	CMorpher* morpher = NULL;
	morpher = m_model->GetFaceMorpher();
	if ( !morpher ) {
		return;
	}

	// Get the two keyframes to use
	keyframe_t previous, next;
	// Search for the two for m_position
	int targetPosition = 0;
	if ( m_keyframes[0].time < m_position ) {
		for ( targetPosition = 1; targetPosition < (signed)m_keyframes.size(); ++targetPosition ) {
			if ( m_keyframes[targetPosition-1].time <= m_position && m_position <= m_keyframes[targetPosition].time ) {
				break;
			}
		}
	}
	if ( targetPosition == 0 ) {
		previous.phonetic[0] = 'm';
		previous.phonetic[1] = 0;
		previous.time = 0;
		previous.strength = 1;
	}
	else {
		previous = m_keyframes[targetPosition-1];
	}
	if ( targetPosition == m_keyframes.size() ) {
		next.phonetic[0] = 'm';
		next.phonetic[1] = 0;
		next.time = m_sequence_length;
		next.strength = 1;
	}
	else {
		next = m_keyframes[targetPosition];
	}

	// Generate a start and end blend based on the phonetic
	CreatePhonetic( previous.phonetic, startBlend );
	CreatePhonetic( next.phonetic, endBlend );

	startBlend = startBlend * previous.strength;
	endBlend = endBlend * next.strength;
	
	// Blend between them based on the position
	targetBlend = startBlend.LerpTo( endBlend, (m_position-previous.time)/(next.time-previous.time) );

	// Set blends based on result
	//targetBlend = targetBlend * (1/12.0f); // scale down properly
	CMorphAction *mouthA, *mouthE, *mouthAE, *mouthO, *mouthF;

	mouthA	= morpher->FindAction("mouthA");
	mouthE	= morpher->FindAction("mouthE");
	mouthAE	= morpher->FindAction("mouthAE");
	mouthO	= morpher->FindAction("mouthO");
	mouthF	= morpher->FindAction("mouthF");

	if ( mouthA )	mouthA->weight	= targetBlend.pData[0];
	if ( mouthE )	mouthE->weight	= targetBlend.pData[1];
	if ( mouthAE )	mouthAE->weight	= targetBlend.pData[2];
	if ( mouthO )	mouthO->weight	= targetBlend.pData[3];
	if ( mouthF )	mouthF->weight	= targetBlend.pData[4];
}

void CLipsyncSequence::CreatePhonetic ( const char* n_phonetic, Matrix4x4& o_blend )
{
	arstring<16> phonetic( n_phonetic );
	Real zeroValues [] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

	if ( phonetic.compare("m") ) {
		o_blend = Matrix4x4(zeroValues);
	}
	else if ( phonetic.compare("a") ) {
		o_blend.pData[0] = 1;
	}
	else if ( phonetic.compare("e") ) {
		o_blend.pData[1] = 1;
	}
	else if ( phonetic.compare("ae") ) {
		o_blend.pData[2] = 1;
	}
	else if ( phonetic.compare("i") ) {
		o_blend.pData[2] = 0.3f;
	}
	else if ( phonetic.compare("o") ) {
		o_blend.pData[3] = 1;
	}
	else if ( phonetic.compare("f") ) {
		o_blend.pData[4] = 1;
	}
	else if ( phonetic.compare("s") || phonetic.compare("st") ) {
		o_blend.pData[0] = 0.1f;
		o_blend.pData[1] = 0.3f;
		o_blend.pData[4] = 0.3f;
	}
	else if ( phonetic.compare("we") ) {
		o_blend.pData[2] = 0.5f;
	}
	else if ( phonetic.compare("l") || phonetic.compare("ll") ) {
		o_blend.pData[1] = 0.3f;
		o_blend.pData[3] = 0.3f;
	}
	else if ( phonetic.compare("th") ) {
		o_blend.pData[1] = 0.3f;
		o_blend.pData[3] = 0.2f;
		o_blend.pData[4] = 0.2f;
	}
	else if ( phonetic.compare("n") ) {
		o_blend.pData[1] = 0.2f;
		o_blend.pData[3] = 0.1f;
	}
	else {
		o_blend.pData[0] = 0.3f;
		o_blend.pData[1] = 0.3f;
	}
}
