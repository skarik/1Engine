-- Focus Variables (These must be set here) --
_ready			= false;
targetAI		= nil;
mCharacter		= nil;
--isAngry			= nil;
--isAlerted		= nil;
angerState		= nil;

toldPosition = nil;

-- Initialize.
-- Given the handles of the character and AI objects.
function Initialize (  )
	local tempObject = ZonedCharacter.new(mCharacter);
	toldPosition = tempObject:GetPosition();
	print( toldPosition );
end

function Execute ( deltaTime )
	-- Only execute focus when not angry or alerted
	if ( angerState == 0 ) then

		-- For now, just don't do anything. kind of just stand there.
		_npc_ai_MoveTo( targetAI, toldPosition.x,toldPosition.y,toldPosition.z );
		
	end
	-- And return that can do aggro code when not angry or alerted 
	local result;
	if ( angerState = 0 ) then
		return 0; -- return cannot aggro
	else
		return 1; -- return can aggro
	end
end

print( "Wander AI loaded." );