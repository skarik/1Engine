-- AI Variables (These must be set here) --
_ready			= false;
targetAI		= nil;
mCharacter		= nil;
isAngry			= nil; --these are grabbed and sent to game after Execute
isAlerted		= nil;

aiTarget		= nil; --I (these are input only)
aiTargetTable	= nil; --I

function Initialize (  )
	print( "======================" );
	print( " AI ALERT INIT" );
	print( "======================" );
end

function Execute ( deltaTime )
	if ( aiTarget == -1 or aiTarget == nil ) then
		return 0;
	end

	local aggroTarget = aiTargetTable;
	local character = GameObject.new(mCharacter);
	
	-- Create seek position, and check if looking for the target again now
	local vSeekPosition = {};
	if ( aggroTarget.timeSinceSeen <= aggroTarget.timeSinceHeard ) then
		vSeekPosition = Vector3d.fromTable(aggroTarget.lastSeenPosition);
		--if ( aggroTarget.timeSinceSeen < 3 ) then
		--	m_isSearching = false;
		--else
		--	m_isSearching = true;
		--end
	else
		vSeekPosition = Vector3d.fromTable(aggroTarget.lastHeardPosition);
		--m_isSearching = true;
	end

	-- Move to target position
	if ( _npc_ai_MoveTo( targetAI, vSeekPosition.x,vSeekPosition.y,vSeekPosition.z ) ) then
		return 1; -- Got to target position, return true, because can't find shit here.
	end
	
	return 0;
end

print( "default_alert.lua loaded" );