-- AI Variables (These must be set here) --
_ready			= false;
targetAI		= nil;
mCharacter		= nil;
isAngry			= nil; --IO (these are grabbed and sent to game after Execute)
isAlerted		= nil; --IO

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
m_attackStatePrev = 0;
--m_circlingTime  = 0;
m_fleeDirection = nil;

m_stateTime = 0;

m_circleDistance = 0;
m_circleDirection = 0;

--m_nextCirclingTime = 0;
m_seekPosition  = nil;
m_searchOffset  = nil;
m_isMoving = false;

m_AtkfeedbackWaitingState = 0;

-- States for attack mode
local AI_SETUP			= 0;
local AI_STANDING		= 1;
local AI_RUN_AROUND		= 2;
local AI_RUN_UP			= 3;
local AI_RUN_AWAY		= 4;
local AI_HIDE			= 5;
local AI_ATTACK			= 6;

function Initialize (  )
	--print( "======================" );
	--print( " AI ANGRY INIT" );
	--print( "======================" );
	m_attackState = AI_SETUP;
end

function Execute ( deltaTime )
	if ( aiTarget == -1 or aiTarget == nil ) then
		return 0;
	end

	local aggroTarget = aiTargetTable;
	local character = ZonedCharacter.new(mCharacter);
	
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
		
		local t_previousState = m_attackState;
		
		if ( m_attackState == AI_SETUP ) then
			-- Setup state
			m_circlingTime = 0;
			m_stateTime = 0;
			m_nextCirclingTime = math.random( 25,45 ) * 0.1;
			m_attackState = AI_STANDING;
			m_isMoving = false;
			
		elseif ( m_attackState == AI_STANDING ) then
			-- Face the target, don't move
			_npc_ai_FaceAt( targetAI, vSeekPosition.x,vSeekPosition.y,vSeekPosition.z );
			m_isMoving = false;
			
			-- If change in state, reset timer
			if ( m_attackStatePrev ~= m_attackState ) then
				m_stateTime = 0;
			end
			
			-- After 2 seconds of standing, go run
			m_stateTime = m_stateTime + deltaTime;
			if ( m_stateTime > 2 ) then
				-- Go to running away state
				m_attackState = AI_RUN_AWAY;
			end
			-- TODO: Check for incoming damage
			
		elseif ( m_attackState == AI_RUN_AWAY ) then
			-- If change in state, reset timer
			if ( m_attackStatePrev ~= m_attackState ) then
				m_stateTime = 0;
				m_fleeDirection = Vector3d.new( math.random(-1,1), math.random(-1,1), 0);
			end
			
			-- Fuck facing the target, run away
			local vDeltaPos = character:GetPosition() - vSeekPosition;
			local t_fleeResult = nil;
			--
			vDeltaPos.z = 0;
			--t_fleeResult = vDeltaPos:normal();
			t_fleeResult = m_fleeDirection + (vDeltaPos:normal() * 1.2);
			t_fleeResult = t_fleeResult:normal();
			local vTargetPosition = character:GetPosition() + (t_fleeResult * 17); -- Run away
			--print( "Flee: " .. t_fleeResult.x .. " " .. t_fleeResult.y .. " " .. t_fleeResult.z  );
			-- Run away
			--_npc_ai_FaceAt( targetAI, vTargetPosition.x,vTargetPosition.y,vTargetPosition.z );
			_npc_ai_MoveTo( targetAI, vTargetPosition.x,vTargetPosition.y,vTargetPosition.z, true );
			m_circleDistance = vDeltaPos:length();
			--print( "Current circle dist: " .. m_circleDistance );
			if ( m_circleDistance > 43 ) then
				m_stateTime = m_stateTime + 50 * deltaTime;
			end
			
			-- After 3 seconds of running, either stand, hide, or run around
			m_stateTime = m_stateTime + deltaTime;
			if ( m_stateTime > 3 ) then
				local chancer = math.random();
				if ( chancer < 0.25 ) then
					m_attackState = AI_HIDE;
				elseif ( chancer < 0.75 ) then
					m_attackState = AI_RUN_AROUND;
				else 
					m_attackState = AI_STANDING;
				end
			end
			
		elseif ( m_attackState == AI_RUN_AROUND ) then
			-- If change in state, reset values
			if ( m_attackStatePrev ~= m_attackState ) then
				m_stateTime = math.random( 25,85 ) * 0.1;
				if ( math.random() < 0.5 ) then
					m_circleDirection = 1;
				else
					m_circleDirection = -1;
				end
			end
		
			-- Circle around the target. (Generate a target position that curves around)
			local vDeltaPos = character:GetPosition() - vSeekPosition;
			vDeltaPos.z = 0;
			vDeltaPos = (character:GetPosition() + vDeltaPos:normal() * m_circleDistance * 0.9);
			vDeltaPos = vDeltaPos + (vDeltaPos:normal():cross( Vector3d.new(0,0,1) ) * 18 * m_circleDirection);

			-- Go to the circling distance
			m_seekPosition = vSeekPosition + vDeltaPos;
			--_npc_ai_FaceAt( targetAI, m_seekPosition.x,m_seekPosition.y,m_seekPosition.z );
			_npc_ai_MoveTo( targetAI, m_seekPosition.x,m_seekPosition.y,m_seekPosition.z, true );
			
			-- Attack after amount of time
			m_stateTime = m_stateTime - deltaTime;
			if ( m_stateTime < 0 ) then
				m_attackState = AI_RUN_UP;
			end
			-- TODO: check for incoming damage
			
		elseif ( m_attackState == AI_HIDE ) then
			-- Face the target, don't move
			_npc_ai_FaceAt( targetAI, vSeekPosition.x,vSeekPosition.y,vSeekPosition.z );
			m_isMoving = false;
			-- Go to reduced damage mode
			_npc_ai_PerformDefend( targetAI );
			-- If change in state, reset timer
			if ( m_attackStatePrev ~= m_attackState ) then
				m_stateTime = 0;
			end
			-- After 1 seconds of standing, go attack
			m_stateTime = m_stateTime + deltaTime;
			if ( m_stateTime > 1 ) then
				-- Go to running away state
				m_attackState = AI_RUN_UP;
			end
			
		elseif ( m_attackState == AI_RUN_UP ) then
			-- Need to go up and perform attack
			local vDeltaPos = character:GetPosition() - vSeekPosition;
			-- Get into melee range
			m_seekPosition = vSeekPosition + (vDeltaPos:normal() * atkRangeTable.meleeRange * 0.8);
			
			-- Face target
			--_npc_ai_FaceAt( targetAI, vSeekPosition.x,vSeekPosition.y,vSeekPosition.z );
			if ( _npc_ai_MoveTo( targetAI, m_seekPosition.x,m_seekPosition.y,m_seekPosition.z, true ) ) then
				-- Go to attack state
				m_attackState = AI_ATTACK;
			end
		
		elseif ( m_attackState == AI_ATTACK ) then
			-- If change in state, reset timer
			if ( m_attackStatePrev ~= m_attackState ) then
				m_stateTime = 0;
				m_AtkfeedbackWaitingState = 0;
			end
			-- Attacking. Go to target, attack. Keep attacking.
			if ( m_AtkfeedbackWaitingState == 0 ) then
				-- Need to go up and perform attack
				local vDeltaPos = character:GetPosition() - vSeekPosition;
				m_seekPosition = vSeekPosition + (vDeltaPos:normal() * atkRangeTable.meleeRange * 0.8);
				
				_npc_ai_FaceAt( targetAI, vSeekPosition.x,vSeekPosition.y,vSeekPosition.z );
				if ( _npc_ai_MoveTo( targetAI, m_seekPosition.x,m_seekPosition.y,m_seekPosition.z, true ) ) then
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
					m_stateTime = m_stateTime + 1;
				end
			else
				if ( atkFeedbackTable.atk_waitForResult == false or atkFeedbackTable.atk_hit == true ) then
					if ( aiAttackHit == true ) then
						-- attack hit. who cares. keep attacking
						--m_AtkfeedbackWaitingState = 0;
						m_attackState = AI_RUN_AWAY;
					else
						-- keep attacking
						--m_AtkfeedbackWaitingState = 0;
						-- Missed, run away
						m_attackState = AI_RUN_AWAY;
					end
				end
			end
			
			-- This counts the misses here. If there's 2 or more failures, run away
			if ( m_stateTime >= 2 ) then
				m_attackState = AI_RUN_AWAY;
			end
			
		end -- attacking state machine
		
		m_attackStatePrev = t_previousState;
	end -- attacking state
	
--	if ( m_attackState == AI_SETUP ) then
--		print( "Current attack state: AI_SETUP" );
--	elseif ( m_attackState == AI_STANDING ) then
--		print( "Current attack state: AI_STANDING" );
--	elseif ( m_attackState == AI_RUN_AROUND ) then
--		print( "Current attack state: AI_RUN_AROUND" );
--	elseif ( m_attackState == AI_RUN_UP ) then
--		print( "Current attack state: AI_RUN_UP" );
--	elseif ( m_attackState == AI_RUN_AWAY ) then
--		print( "Current attack state: AI_RUN_AWAY" );
--	elseif ( m_attackState == AI_HIDE ) then
--		print( "Current attack state: AI_HIDE" );
--	elseif ( m_attackState == AI_ATTACK ) then
--		print( "Current attack state: AI_ATTACK" );
--	else
--		print( "Current attack state: UNKNOWN" );
--	end
	
	return 0;
end


print( "human_melee_angry.lua loaded" );