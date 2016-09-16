
#include "Sequence.h"

#include "engine-common/lua/CLuaController.h"

#include "core/time/time.h"
#include "core/utils/StringUtils.h"

#include "after/entities/CCharacterModel.h" // TODO: FILE IS IN engine, BUT RELIES ON CCharacterModel, WHICH IS IN engine-common

Animation::Sequence::Sequence ( void )
{
	m_target = NULL;
	m_layer = 0;
	m_loop = false;

	Reset();
}

Animation::Sequence::~Sequence ( void )
{
	m_target = NULL;
}

void Animation::Sequence::Reset ( void )
{
	m_time = 0;
	isPlaying = false;
	m_current_animation = "";
	m_current_animation_blend = 0.0f;
}

void Animation::Sequence::Update ( void )
{
	// If not playing, leave
	if ( !isPlaying ) {
		return;
	}


	// Increment time
	Real t_prevtime = m_time;
	m_time += Time::deltaTime;

	// Look if passed any events
	for ( uint i = 0; i < m_keys.size(); ++i )
	{
		if ( m_keys[i].m_time >= t_prevtime && m_keys[i].m_time < m_time )
		{
			// Check the type. If it's END and we loop, then we need to run this again from the start.
			if ( m_keys[i].m_type == Animation::SEQ_ACTION_END )
			{
				if ( m_loop )
				{
					m_time -= m_keys[i].m_time + Time::deltaTime;
					Update(); // Call update again (this can cause infinite loop if game is going really slow on short sequences)
				}
				else
				{
					isPlaying = false;
				}
			}
			else if ( m_keys[i].m_type == Animation::SEQ_ACTION_ANIMATION )
			{	// Play animation
				m_current_animation	= m_keys[i].m_value;
				m_current_animation_blend = 0.0f;
				// Parse the subinfo
				if ( m_keys[i].m_subvalue.length() )
				{
					// Tokenize the subvalue string
					std::vector<string> tokens = StringUtils::Split( m_keys[i].m_subvalue, " \t\n\r", false );
					uint tok = 0;
					while ( tok < tokens.size() )
					{
						// Read blending information
						if ( tokens[tok] == "blend" ) {
							tok += 1;
							m_current_animation_blend = (Real) atof( tokens[tok].c_str() );
						}
						// Go to next token
						tok += 1;
					}
				}
				// Play animation here first
				if ( m_current_animation.length() > 0 && m_target)
				{
					m_target->SetNextBlendingValue( m_current_animation_blend );
					m_target->PlayAnimation( m_current_animation, -1 );
				}
			}
			else if ( m_keys[i].m_type == Animation::SEQ_ACTION_LUA )
			{	// Create a Lua Environment and run the code. m_time goes in and out.
				Lua::Controller->SetEnvironment( "system_sequencer" );
				lua_State* L = Lua::Controller->GetState();
				lua_pushnumber( L, m_time );			lua_setfield( L, -2, "m_time" );
				//Lua::Controller->RunLua( m_keys[i].m_value );
					luaL_loadbuffer( L, m_keys[i].m_value.c_str(), m_keys[i].m_value.size(), "line" );
					lua_pushvalue( L, -2 );
					lua_setupvalue( L, -2, 1 );
					lua_pcall( L, 0,0,0 );
				lua_getfield( L, -1, "m_time" );	m_time = (Real) lua_tonumber( L,-1 );	lua_pop(L,1);
				Lua::Controller->ResetEnvironment();
			}
		}
	}

	// Play target animation
	if ( m_current_animation.length() > 0 && m_target )
	{
		m_target->SetNextBlendingValue( m_current_animation_blend );
		m_target->PlayAnimation( m_current_animation, -1 );
	}
}

void Animation::Sequence::Play ( void )
{
	if ( !isPlaying )
	{
		m_time = (Real) -FTYPE_PRECISION;
		isPlaying = true;
	}
}



#include "core/system/io/FileUtils.h"
#include "core/system/io/CBinaryFile.h"
#include "core-ext/system/io/Resources.h"

bool Animation::Sequence::LoadFromFile ( const char* filename )
{
	char	t_line [256];
	char*	t_token;
	SequenceAction	tc_action;

	//FILE*	t_file = fopen( filename, "rb" );
	FILE*	t_file = Core::Resources::Open( filename, "rb" );
	if ( !t_file ) return false;
	CBinaryFile tu_file (t_file); // used to automatically close the file when exit scope

	do
	{
		// Looking for a TIME:TYPE value. Read in a line at a time.
		IO::ReadLine( t_file, t_line );
		// Look for a ##.##:ABC format
		t_token = strtok( t_line, " \t:\n\r" );
		if ( t_token == NULL ) continue;
		tc_action.m_time = (Real) atof( t_token );
		t_token = strtok( NULL, " \t:\n\r" );
		if ( t_token == NULL ) continue;

		if ( strcmp( t_token, "animation" ) == 0 ) {
			tc_action.m_type = Animation::SEQ_ACTION_ANIMATION;
		}
		else if ( strcmp( t_token, "lua" ) == 0 ) {
			tc_action.m_type = Animation::SEQ_ACTION_LUA;
		}
		else if ( strcmp( t_token, "end" ) == 0 ) {
			tc_action.m_type = Animation::SEQ_ACTION_END;
			tc_action.m_value		= "";
			tc_action.m_subvalue	= "";
		}
		else {
			tc_action.m_type = Animation::SEQ_ACTION_INVALID;
		}

		// Had a ##.##:ABC format. Now read in the data.
		if ( tc_action.m_type == Animation::SEQ_ACTION_ANIMATION || tc_action.m_type == Animation::SEQ_ACTION_LUA )
		{
			// Look for the first {
			{
				int tempV;
				do {
					tempV = getc(t_file);
				}
				while ( tempV != '{' && !feof(t_file) );
				if ( feof(t_file) ) return false;
			}
			// Read in the data
			if ( tc_action.m_type == Animation::SEQ_ACTION_ANIMATION )
			{
				// Read in until hit non-space
				char animation_name [256];
				{
					int tempV;
					do {
						tempV = getc(t_file);
					}
					while ( isspace(tempV) && !feof(t_file) );
					if ( feof(t_file) ) return false;
					animation_name[0] = tempV;
				}
				// Read in until hit space
				{
					int str_pos = 1;
					int tempV;
					do {
						tempV = getc(t_file);
						animation_name[str_pos++] = tempV;
					}
					while ( !isspace(tempV) && !feof(t_file) );
					if ( feof(t_file) ) return false;
					animation_name[str_pos-1] = '\0';
					tc_action.m_value = animation_name;
				}
				// Read in everything else until the }
				tc_action.m_subvalue = "";
				{
					int tempV;
					do {
						tempV = getc(t_file);
						if ( tempV != '}' ) {
							tc_action.m_subvalue += (char)tempV;
						}
					}
					while ( tempV != '}' && !feof(t_file) );
					if ( feof(t_file) ) return false;
				}
			}
			else if ( tc_action.m_type == Animation::SEQ_ACTION_LUA )
			{
				// Read in everything until the }
				tc_action.m_value = "";
				tc_action.m_subvalue = "";
				char tempV;
				do {
					tempV = getc(t_file);
					if ( tempV != '}' ) {
						tc_action.m_value += tempV;
					}
				}
				while ( tempV != '}' && !feof(t_file) );
				if ( feof(t_file) ) return false;
			}
		}
		// Done reading that sequence key.
		// If it's not an invalid key, add it in.
		if ( tc_action.m_type != Animation::SEQ_ACTION_INVALID )
		{
			m_keys.push_back( tc_action );
		}
	} 
	while ( !feof(t_file) );

	return true;
}