-- AI Variables (These must be set here) --
_ready			= false;
targetAI		= nil;
mCharacter		= nil;

aiTarget		= nil; --I (these are input only)
aiTargetTable	= nil; --I

function Initialize (  )
	print( "======================" );
	print( " AI ALERT INIT" );
	print( "======================" );
end
--TODO add a get state to check if searching instead
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
	--if ( _npc_ai_MoveTo( targetAI, vSeekPosition.x,vSeekPosition.y,vSeekPosition.z ) ) then
	--	return 1; -- Got to target position, return true, because can't find shit here.
	--end
	-- Face at target position
	_npc_ai_FaceAt( targetAI, vSeekPosition.x,vSeekPosition.y,vSeekPosition.z );
	
	return 0;
end

print( "default_alert.lua loaded" );