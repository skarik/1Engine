
#include "CSkillTree.h"
#include "core/system/io/FileUtils.h"
#include "core/system/io/CBinaryFile.h"
#include "core-ext/system/io/Resources.h"
#include "after/entities/item/skill/SkillList.h"

// Include XML loader
#include "core-ext/system/io/Resources.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

CSkillTree::CSkillTree(void)
{
	ClearTree();
}

void CSkillTree::ManagedLoad ( const char* skillTree )
{
	ClearTree();
	try 
	{
		LoadSkillTree( skillTree );
	}
	catch ( const rapidxml::parse_error& e )
	{
		std::cout << "Invalid XML file at " << e.where<char>() << ": " << e.what() << std::endl;
		throw std::exception();
	}
}

CSkillTree::~CSkillTree()
{
	
}

void CSkillTree::ClearTree ( void )
{
	elements.clear();
	for ( auto node = nodes.begin(); node != nodes.end(); node++ )
	{
		delete (*node);
	}
	nodes.clear();
}
void CSkillTree::ReloadSkillTree ( void )
{
	ClearTree();
	LoadSkillTree(m_skilltreefile);
}

void CSkillTree::LoadSkillTree ( const char* skillTree )
{
	m_skilltreefile = skillTree;

	// Open base skill tree
	rapidxml::file<> xmlFile( Core::Resources::PathTo( skillTree ).c_str() ); // Default template is char
    rapidxml::xml_document<> doc;
    doc.parse<0>( xmlFile.data() );

	// Create temporary buffer
	char newSet [256];

	for ( auto node = doc.first_node(); node; node = node->next_sibling() )
	{
		// Load up tree info node
		if ( strcmp( node->name(), "skilltree" ) == 0 )
		{
			for ( auto attr = node->first_attribute(); attr; attr = attr->next_attribute() )
			{
				if ( strcmp( attr->name(), "title" ) == 0 )
				{
					treeName = attr->value();
				}
				else if ( strcmp( attr->name(), "background_color" ) == 0 )
				{
					treeBackground = Color( .85,.85,.7, 1.0 );
					strcpy( newSet, attr->value() );
					char* pos = strtok( newSet, " ," );
					treeBackground.red = (ftype)atof(pos);
					pos = strtok( NULL, " ," );
					if ( pos ) {
						treeBackground.green = (ftype)atof(pos);
						pos = strtok( NULL, " ," );
					}
					if ( pos ) {
						treeBackground.blue = (ftype)atof(pos);
						pos = strtok( NULL, " ," );
					}
					if ( pos ) {
						treeBackground.alpha = (ftype)atof(pos);
						pos = strtok( NULL, " ," );
					}
				} 
			}
		}
		// Load up a new skill node
		if ( strcmp( node->name(), "skill" ) == 0 )
		{
			skillNode* t_skillnode = new skillNode;
			t_skillnode->currentSkill	= NULL;
			t_skillnode->skillPos		= Rect( 0,0,64,64 );

			for ( auto attr = node->first_attribute(); attr; attr = attr->next_attribute() )
			{
				if ( strcmp( attr->name(), "id" ) == 0 )
				{
					//t_skillnode->currentSkill = (CSkill*)CSkill::Instantiate( attr->value() );
					t_skillnode->currentSkill = WeaponItem::WeaponItemFactory.skill_refs[WeaponItem::WeaponItemFactory.witem_swap[attr->value()]];
					if ( t_skillnode->currentSkill == NULL )
					{
						//throw Engine::NullReferenceException();

						//	Lua::Controller->LoadLuaFile( std::string("") + m_entityName.c_str() + ".lua", m_environment.c_str(), m_environment.c_str() );
						//	s = luaL_loadfile( luaVM, (".res/lua/"+filename).c_str());		// s:c
						//	tring file = ".res/lua/" + "skills" + m_entityName.c_str() + ".lua"

						// Now we check for the lua bit
						string skillName = attr->value();
						string luaFile = string("lua/") + "skills/" + skillName + ".lua";
						luaFile = Core::Resources::PathTo( luaFile ).c_str();
						if ( IO::FileExists( luaFile ) )
						{
							// Skill exists.
							t_skillnode->currentSkill = new CSkillReference<CLuaSkill,-3>();
							t_skillnode->currentSkill->SetEntityName( skillName );
						}
						else
						{
							// If no lua bit, give it a null skill
							t_skillnode->currentSkill = new CSkillReference<CSkill,0>();
						}
					}
					t_skillnode->icon = new CTexture( t_skillnode->currentSkill->GetIconFilename() );
				}
				else if ( strcmp( attr->name(), "pos" ) == 0 )
				{
					strcpy( newSet, attr->value() );
					char* pos = strtok( newSet, " ," );
					t_skillnode->skillPos.pos.x = (ftype)atof(pos);
					pos = strtok( NULL, " ," );
					t_skillnode->skillPos.pos.y = (ftype)atof(pos);
				}
				else if ( strcmp( attr->name(), "parent" ) == 0 )
				{
					strcpy( newSet, attr->value() );
					char* pos = strtok( newSet, " ,;" );
					while ( pos != NULL )
					{	// Read in all of the keyvalue pieces
						char searchSet [64];
						strcpy( searchSet, pos );
						t_skillnode->parentSkill.push_back( FindSkillnodeWithName( searchSet ) );
						pos = strtok( NULL, " ,;" );
						if ( pos ) {
							t_skillnode->parentSkillReq.push_back( atoi(pos) );
							pos = strtok( NULL, " ,;" );
						}
						else {
							t_skillnode->parentSkillReq.push_back( 1 );
						}
					}
				}
			} // End loop through attributes

			// Modify size for screen
			t_skillnode->skillPos.pos /= 720.0f;
			t_skillnode->skillPos.size /= 720.0f;

			// Calculate proxy steps
			for ( uint i = 0; i < t_skillnode->parentSkill.size(); ++i )
			{
				if ( t_skillnode->parentSkill[i] )
				{
					if ( (t_skillnode->parentSkill[i]->skillPos.pos - t_skillnode->skillPos.pos).magnitude() > 0.8f ) {
						t_skillnode->parentSkillProxy.push_back( true );
					}
					else {
						t_skillnode->parentSkillProxy.push_back( false );
					}
				}
				else
				{
					t_skillnode->parentSkillProxy.push_back( false );
				}
			}

			// Push new skill node to the list
			nodes.push_back( t_skillnode );
		}
		// Load up a postlink node
		if ( strcmp( node->name(), "postlink" ) == 0 )
		{
		}
		// Load up a title
		if ( strcmp( node->name(), "title" ) == 0 )
		{
			skilltree_element element;
			element.type = 0; // Title element

			for ( auto attr = node->first_attribute(); attr; attr = attr->next_attribute() )
			{
				if ( strcmp( attr->name(), "type" ) == 0 )
				{
					element.subtype = atoi( attr->value() );
				}
				else if ( strcmp( attr->name(), "content" ) == 0 )
				{
					element.content = attr->value();
				}
				else if ( strcmp( attr->name(), "pos" ) == 0 )
				{
					strcpy( newSet, attr->value() );
					char* pos = strtok( newSet, " ," );
					element.position.x = (ftype)atof(pos);
					pos = strtok( NULL, " ," );
					element.position.y = (ftype)atof(pos);
				}
			}

			// Modify position for screen
			element.position /= 720.0f;

			// Push new element to the list
			elements.push_back( element );
		}
	}

	std::cout << "Done." << std::endl;
}


skillNode* CSkillTree::FindSkillnodeWithName ( const char* name )
{
	skillNode* currentNode;
	for ( auto node = nodes.begin(); node != nodes.end(); node++ )
	{
		currentNode = *node;

		if ( WeaponItem::WeaponItemFactory.witem_name[currentNode->currentSkill->GetID()].find( name ) != string::npos )
		{
			return currentNode;
		}
		else if ( currentNode->currentSkill->GetID() == -3 )
		{
			if ( currentNode->currentSkill->GetEntityName().find( name ) != string::npos )
			{
				return currentNode;
			}
		}
	}
	return NULL;
}

// Save the skill tree levels to the map in the file. Doesn't overwrite, only edits.
void CSkillTree::Save ( const arstring<256>& n_location )
{
	typedef short SkillID;
	typedef short Level;
	std::map<SkillID,Level> levelMap;
	// First read in the entire file into a map
	CBinaryFile file;
	if ( IO::FileExists( n_location ) )
	{
		file.Open( n_location, CBinaryFile::IO_READ );
		{
			uint32_t entryCount = file.ReadUInt32();
			for ( uint i = 0; i < entryCount; ++i )
			{
				SkillID readID = file.ReadShort();
				Level	readLevel = file.ReadShort();
				levelMap[readID] = readLevel;
			}
		}
		file.Close();
	}

	// Modify the map with our stuff
	for ( auto node = nodes.begin(); node != nodes.end(); node++ )
	{
		if ( (*node)->currentSkill->m_level )
		{
			levelMap[(*node)->currentSkill->GetID()] = (*node)->currentSkill->m_level;
		}
	}

	// Write the entire map to the file
	file.Open( n_location, CBinaryFile::IO_WRITE );
	{
		file.WriteUInt32( levelMap.size() );
		for ( auto levelEntry = levelMap.begin(); levelEntry != levelMap.end(); ++levelEntry )
		{
			file.WriteShort( levelEntry->first );
			file.WriteShort( levelEntry->second );
		}
	}
	file.Close();
}
// Load the skill tree levels from a map in the file. Invalid skills are ignored.
bool CSkillTree::Load ( const arstring<256>& n_location )
{
	typedef short SkillID;
	typedef short Level;
	std::map<SkillID,Level> levelMap;
	// Read in the entire file into a map
	CBinaryFile file;
	if ( !file.Exists( n_location ) ) {
		return false;
	}
	file.Open( n_location, CBinaryFile::IO_READ );
	{
		uint32_t entryCount = file.ReadUInt32();
		for ( uint i = 0; i < entryCount; ++i )
		{
			SkillID readID = file.ReadShort();
			Level	readLevel = file.ReadShort();
			levelMap[readID] = readLevel;
		}
	}
	file.Close();

	// Apply the map to the skill tree
	for ( auto node = nodes.begin(); node != nodes.end(); node++ )
	{
		auto levelResult = levelMap.find( (*node)->currentSkill->GetID() );
		if ( levelResult != levelMap.end() )
		{
			(*node)->currentSkill->m_level = levelResult->second;
		}
	}

	return true;
}