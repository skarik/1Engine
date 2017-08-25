-- AI Variables (These must be set here) --
_ready			= false;
targetAI		= nil;
mCharacter		= nil;

aiTarget		= nil; --I (these are input only)
aiTargetTable	= nil; --I

-- Inputs
atkFeedbackTable	= nil;
atkRangeTable		= nil;

--only new function is _npc_ai_PerformAttack

-- Local variables
m_isSearching	= false;
m_searchState	= 0;
m_searchTimer   = 0;
m_attackState	= 0;
m_circlingTime  = 0;
m_nextCirclingTime = 0;
m_seekPosition  = nil;
m_searchOffset  = nil;

m_AtkfeedbackWaitingState = 0;

function Initialize (  )
	--print( "======================" );
	--print( " AI ANGRY INIT" );
	--print( "======================" );
end

function Execute ( deltaTime )
	if ( aiTarget == nil ) then
		return 0;
	end

	if ( aiTarget == -1 ) then
		-- Face at next best guess
		return 0;
	end
	
	local aggroTarget = aiTargetTable;
	local character = GameObject.new(mCharacter);
	
	-- Create seek position, and check if looking for the target again now
	local vSeekPosition = {};
	if ( aggroTarget.timeSinceSeen <= aggroTarget.timeSinceHeard ) then
		vSeekPosition = Vector3d.fromTable(aggroTarget.lastSeenPosition);
		if ( aggroTarget.timeSinceSeen < 3 ) then
			m_isSearching = false;
		else
			m_isSearching = true;
		end
	else
		vSeekPosition = Vector3d.fromTable(aggroTarget.lastHeardPosition);
		m_isSearching = true;
	end
	
	
	if ( m_isSearching ) then
		-- Search mode, search for the target
		m_attackState = 0; -- Reset attack mode
		
		if ( m_searchState == 0 ) then
			-- Move to the seek position
			if ( _npc_ai_MoveTo( targetAI, vSeekPosition.x,vSeekPosition.y,vSeekPosition.z ) ) then
				m_searchState = 1; -- If at target position, then go to waiting for a moment state
				m_searchTimer = 0;
				m_searchOffset = nil; -- Reset searching offset
			end
		elseif ( m_searchState == 1 ) then
			-- Wait at the seek position
			m_searchTimer = m_searchTimer + deltaTime;
			if ( m_searchTimer > 2 ) then --Wait for 2 seconds, then start drifting randomly
				m_searchState = 2;
				-- Create an offset to look at
				if ( m_searchOffset == nil ) then
					m_searchOffset = Vector3d.new( math.random(-10,10), math.random(-10,10), math.random(-2,2) );
				else
					m_searchOffset = m_searchOffset+Vector3d.new( math.random(-10,10), math.random(-10,10), math.random(-2,2) );
				end
			end
		elseif ( m_searchState == 2 ) then
			-- Go to the new offset
			m_seekPosition = vSeekPosition + m_searchOffset;
			-- Search to that random position
			if ( _npc_ai_MoveTo( targetAI, m_seekPosition.x,m_seekPosition.y,m_seekPosition.z ) ) then
				m_searchState = 1; -- If at target position, then go to waiting for a moment state
				m_searchTimer = 0;
			end
		end
	else
		-- Attack mode, move up to target and attack
		m_searchState = 0; -- Reset search mode
		
		if ( m_attackState == 0 ) then
			-- Seeking. Get close enough to the target.
			local vDeltaPos = character:GetPosition() - vSeekPosition;
			local vTargetPosition = vSeekPosition + (vDeltaPos:normal() * 10);
			
			m_seekPosition = vTargetPosition;
			
			if ( _npc_ai_MoveTo( targetAI, vTargetPosition.x,vTargetPosition.y,vTargetPosition.z ) or (vDeltaPos:length() < 10) ) then	
				m_attackState = 1; -- Go to circling when close enough
				-- Set circling times
				m_circlingTime = 0;
				m_nextCirclingTime = math.random( 25,55 ) * 0.1;
			end
		elseif ( m_attackState == 1 ) then
			
			-- Circling. Circle around the target.
			local vDeltaPos = character:GetPosition() - vSeekPosition;
			vDeltaPos = vDeltaPos + (vDeltaPos:normal():cross( Vector3d.new(0,0,1) ) * 9);
			
			local circlingDistance = vDeltaPos:length();
			if ( circlingDistance < 9.0 ) then
				vDeltaPos = (vDeltaPos/circlingDistance) * 9;
			elseif ( circlingDistance > 13.0 ) then
				vDeltaPos = (vDeltaPos/circlingDistance) * 13;
			end
			-- Go to the circling distance
			m_seekPosition = vSeekPosition + vDeltaPos;
			_npc_ai_MoveTo( targetAI, m_seekPosition.x,m_seekPosition.y,m_seekPosition.z );
			
			-- Attack after amount of time
			m_circlingTime = m_circlingTime+deltaTime;
			if ( m_circlingTime > m_nextCirclingTime ) then
				m_AtkfeedbackWaitingState = 0;
			
				m_circlingTime = 0;
				m_nextCirclingTime = math.random( 25,55 ) * 0.1;
				
				m_attackState = 2; -- Go to attacking
			end
		elseif ( m_attackState == 2 ) then
			-- Attacking. Go to target, attack. If attack fails, go back to circling.
			
			-- First move up and request attack
			if ( m_AtkfeedbackWaitingState == 0 ) then
				-- Need to go up and perform attack
				local vDeltaPos = character:GetPosition() - vSeekPosition;
				m_seekPosition = vSeekPosition + (vDeltaPos:normal() * atkRangeTable.meleeRange);
				
				_npc_ai_FaceAt( targetAI, vSeekPosition.x,vSeekPosition.y,vSeekPosition.z );
				if ( _npc_ai_MoveTo( targetAI, m_seekPosition.x,m_seekPosition.y,m_seekPosition.z ) ) then
					-- request attack
					_npc_ai_PerformAttack( targetAI );
					-- go to attack request check
					m_AtkfeedbackWaitingState = 1;
				end
			elseif ( m_AtkfeedbackWaitingState == 1 ) then
				-- check if attack request when through
				if ( atkFeedbackTable.atk_waitForResult == true ) then
					-- request went through. go to attack result check.
					m_AtkfeedbackWaitingState = 2;
				else
					-- didn't go through, go back to request
					m_AtkfeedbackWaitingState = 0;
				end
			else
				--print( tostring(aiWaitOnAttack) .. " " .. tostring(aiAttackHit) );
				if ( atkFeedbackTable.atk_waitForResult == false or atkFeedbackTable.atk_hit == true ) then
					if ( atkFeedbackTable.atk_hit == true ) then
						-- attack hit
					else
						-- attack didn't hit (either miss or cooldown)
						m_attackState = 1; -- go to circling if attack missed
					end
				end
			end
			-- end m_AtkfeedbackWaitingState
			
		end -- attacking state machine
	end -- attacking state
	
	return 0;
end


print( "default_angry.lua loaded" );