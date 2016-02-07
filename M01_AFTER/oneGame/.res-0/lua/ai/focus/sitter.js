// Focus Variables (These must be set here)
NPC = {
	ai:			undefined,
	isAngry:	false,
	isAlerted:	false,
	angerState:	0,
	
	character:	undefined
}

toldPosition = {x:0,y:0,z:0};

// Initialize.
// Given the handles of the character and AI objects.
function Initialize (  )
{
	toldPosition = GameObject_GetPosition( NPC.character );
	toldPosition.print(); // Debug print 
}

function Execute ( deltaTime )
{
	// Only execute focus when not angry or alerted
	print( NPC.character );
	NPC_AI_PlayAnimation( NPC.character, "idle_sit_01" );
	if ( NPC.angerState == 0 ) {
		// For now, just don't do anything. kind of just stand there.
		NPC_AI_MoveTo( NPC.ai, toldPosition.x,toldPosition.y,toldPosition.z );
	}	
	// And return that can do aggro code when not angry or alerted 
	if ( NPC.angerState == 0 ) {
		return 0; // return cannot aggro
	else {
		return 0; // return 1 for can aggro
	}
}

print( "Sitter AI loaded." );