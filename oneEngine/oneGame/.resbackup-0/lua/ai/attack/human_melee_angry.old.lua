-- AI Variables (These must be set here) --
_ready			= false;
targetAI		= nil;
mCharacter		= nil;
isAngry			= nil; --IO (these are grabbed and sent to game after Execute)
isAlerted		= nil; --IO

aiTarget		= nil; --I (these are input only)
aiTargetTable	= nil; --I

aiPerformedAttack = false; --IO
aiWaitOnAttack	  = false; --IO
aiAttackHit		  = false; --IO
aiWaitResultTimer = 0; 	   --IO

aiCurrentAttackRange = 0; --I

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
m_isMoving = false;

function Initialize (  )
	--print( "======================" );
	--print( " AI ANGRY INIT" );
	--print( "======================" );
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
			-- Setup state
			m_circlingTime = 0;
			m_nextCirclingTime = math.random( 25,45 ) * 0.1;
			m_attackState = 1;
			m_isMoving = false;
		elseif ( m_attackState == 1 ) then
			-- Seeking/Hovering
			local vDeltaPos = character:GetPosition() - vSeekPosition;
			local vTargetPosition = vSeekPosition + (vDeltaPos:normal() * 7);
			
			-- move to a fairly close position
			m_seekPosition = vTargetPosition;
			if ( vDeltaPos:length() > 10 ) then
				m_isMoving = true;
			end
			if ( m_isMoving == true ) then
				if ( _npc_ai_MoveTo( targetAI, vTargetPosition.x,vTargetPosition.y,vTargetPosition.z, true ) ) then
					m_isMoving = false;
				end
			end
			
			--todo: defend
			
			if ( m_isMoving == false ) then	
				-- Defend
				_npc_ai_PerformDefend( targetAI );
				-- Set circling times
				m_circlingTime = m_circlingTime + deltaTime;
				if ( m_circlingTime > m_nextCirclingTime ) then
					m_attackState = math.random(2,3);
					m_circlingTime = 0; --reset timer
					if ( m_attackState == 2 ) then
						-- Circling
						m_nextCirclingTime = math.random( 5,15 ) * 0.1;
					elseif ( m_attackState == 3 ) then
						-- Attacking
						m_nextCirclingTime = math.random( 15,25 ) * 0.1; -- do a short attack
					end
				end
			else
				m_circlingTime = m_circlingTime - deltaTime;
				if ( m_circlingTime < 0 ) then
					m_circlingTime = 0;
				end
			end
		elseif ( m_attackState == 2 ) then
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
				aiPerformedAttack = false;
				m_circlingTime = 0;
				m_nextCirclingTime = math.random( 25,55 ) * 0.1; --do a long attack
				m_attackState = 3; -- Go to attacking
			end
		elseif ( m_attackState == 3 ) then
			-- Attacking. Go to target, attack. Keep attacking.
			if ( aiPerformedAttack == false ) then
				-- Need to go up and perform attack
				local vDeltaPos = character:GetPosition() - vSeekPosition;
				m_seekPosition = vSeekPosition + (vDeltaPos:normal() * aiCurrentAttackRange * 0.8);
				
				_npc_ai_FaceAt( targetAI, vSeekPosition.x,vSeekPosition.y,vSeekPosition.z );
				if ( _npc_ai_MoveTo( targetAI, m_seekPosition.x,m_seekPosition.y,m_seekPosition.z, true ) ) then
					aiPerformedAttack,aiWaitOnAttack,aiWaitResultTimer = _npc_ai_PerformAttack( targetAI );
					aiAttackHit = false;
				end
			else
				if ( aiWaitOnAttack == false or aiAttackHit == true ) then
					if ( aiAttackHit == true ) then
						aiPerformedAttack = false;
					else
						aiPerformedAttack = false;
						-- Keep friggin attacking
					end
				else
					aiWaitResultTimer = aiWaitResultTimer-deltaTime;
					if ( aiWaitResultTimer <= 0.5 ) then --If waited too long with no result, assume failure
						aiWaitOnAttack = false;
						aiAttackHit = false;
						aiPerformedAttack = false;
					end
				end
			end
			-- After amount of time, go back to hovering
			m_circlingTime = m_circlingTime+deltaTime;
			if ( m_circlingTime > m_nextCirclingTime ) then
				--m_circlingTime = 0;
				--m_nextCirclingTime = math.random( 25,55 ) * 0.1;
				--m_attackState = 1; -- Go to hovering
				m_attackState = 4; -- go to backing the hell up
			end
		elseif ( m_attackState == 4 ) then
			-- Seeking. Get close enough to the target.
			local vDeltaPos = character:GetPosition() - vSeekPosition;
			local vTargetPosition = vSeekPosition + (vDeltaPos:normal() * 6);
			
			m_seekPosition = vTargetPosition;
			
			if ( _npc_ai_MoveTo( targetAI, vTargetPosition.x,vTargetPosition.y,vTargetPosition.z ) or (vDeltaPos:length() > 6) ) then	
				m_attackState = 1; -- Go to hovering
				-- Set hovering times
				m_circlingTime = 0;
				m_nextCirclingTime = math.random( 15,45 ) * 0.1;
			end
		end -- attacking state machine
	end -- attacking state
	
	return 0;
end


print( "human_melee_angry.lua loaded" );