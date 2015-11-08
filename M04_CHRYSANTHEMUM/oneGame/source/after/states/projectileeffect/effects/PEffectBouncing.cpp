
#include "PEffectBouncing.h"
#include "engine-common/entities/CProjectile.h"


PEffectBouncing::PEffectBouncing ( const int nMaxBounceCount )
	: mMaxBounces( nMaxBounceCount ), CProjectileBuff()
{
	mBounces = 0;
}

bool PEffectBouncing::OnHitWorld( const Vector3d& nWorldPos, const Vector3d& nNormal )
{
	if ( mBounces++ < mMaxBounces ) {

		Vector3d nDirection = pTarget->GetHeading().normal();
		Vector3d reflection = nDirection - ( nNormal * nDirection.dot(nNormal) * 2 );
		pTarget->SetHeading( reflection );
		pTarget->transform.position = nWorldPos + ((nNormal*0.7f+reflection*0.3f).normal() * pTarget->GetWidth());

		return false;
	}
	else {
		return true;
	}
}