
local m_time = 0.0;

function OnCreate ( )
	m_timer = 0.0;
end


function Update ( deltaTime )

	m_timer = m_timer + deltaTime;
	print( m_timer );

end

