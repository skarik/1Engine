-- Focus Variables (These must be set here) --
_ready			= false;
targetAI		= nil;
mCharacter		= nil;
--isAngry			= nil;
--isAlerted		= nil;
angerState		= nil;

-- Focus info
isMoving		= false;
isFollowing		= false;
toldPosition	= Vector3d.new(0,0,0);

-- Initialize.
-- Given the handles of the character and AI objects.
function Initialize (  )
	local tempObject = ZonedCharacter.new(mCharacter);
	toldPosition = tempObject:GetPosition();
	print( toldPosition );
end

function Execute ( deltaTime )
	-- For now, only execute focus when not angry or alerted
	if ( angerState == 0 ) then
		if ( isFollowing ) then
			_npc_ai_Follow( targetAI );
		else
			if ( isMoving ) then
				_npc_ai_MoveTo( targetAI, toldPosition.x,toldPosition.y,toldPosition.z );
			end
		end
	end
	-- And return that can do aggro code when not angry or alerted 
	local result;
	if ( angerState == 0 ) then
		return 0; -- return cannot aggro
	else
		return 1; -- return can aggro
	end
end

function PartyCmdFollowMe ( character )
	print( "Told to follow " );
	print( character );
	
	isFollowing = true;
	isMoving = true;
	_npc_ai_SetFollowTarget( targetAI, character );
end

function PartyCmdMoveTo ( x, y, z )
	toldPosition = Vector3d.new(x,y,z);
	--print( "Told to move" );
	print( "Told to move to " );
	print(toldPosition);
	
	isFollowing = false;
	isMoving = true;
end



print( "Companion AI loaded." );