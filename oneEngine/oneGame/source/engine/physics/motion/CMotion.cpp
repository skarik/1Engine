#include "CMotion.h"

template<>
CGameBehavior* CMotion::GetOwner ( void )
{
	if (ownerType == core::kBasetypeGameBehavior) {
		return static_cast<CGameBehavior*>(owner);
	}
	return NULL;
}
template<>
IPrGameMotion* CMotion::GetOwner ( void )
{
	if (ownerType == core::kBasetypeIPrMotion) {
		return static_cast<IPrGameMotion*>(owner);
	}
	return NULL;
}
template<>
CLogicObject* CMotion::GetOwner ( void )
{
	if (ownerType == core::kBasetypeRrLogicObject) {
		return static_cast<CLogicObject*>(owner);
	}
	return NULL;
}
template<>
CRenderableObject* CMotion::GetOwner ( void )
{
	if (ownerType == core::kBasetypeRrRenderObject) {
		return static_cast<CRenderableObject*>(owner);
	}
	return NULL;
}


template <>
void CMotion::SetOwner ( CGameBehavior* n_owner )
{
	owner = static_cast<void*>(n_owner);
	ownerType = core::kBasetypeGameBehavior;
}
template <>
void CMotion::SetOwner ( IPrGameMotion* n_owner )
{
	owner = static_cast<void*>(n_owner);
	ownerType = core::kBasetypeIPrMotion;
}
template <>
void CMotion::SetOwner ( CLogicObject* n_owner )
{
	owner = static_cast<void*>(n_owner);
	ownerType = core::kBasetypeRrLogicObject;
}
template <>
void CMotion::SetOwner ( CRenderableObject* n_owner )
{
	owner = static_cast<void*>(n_owner);
	ownerType = core::kBasetypeRrRenderObject;
}