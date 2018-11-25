
#include "Quaternion.h"
#include "Rotator.h"

using namespace std;

// Swaps the basis of the quaternion (you should never need to use this)
Quaternion& Quaternion::SwitchBasis ( void )
{
	Rotator temp ( *this );
	temp.SwitchBasis();
	*this = temp.getQuaternion();
	Normalize();
	return *this;
}
// Gets euler angles from the quaternion
Vector3f Quaternion::GetEulerAngles ( void ) const
{
	return Rotator( *this ).getEulerAngles();
}


// Out stream overload
ostream& operator<< (ostream& out, Quaternion const& quat)
{
	out << "(" << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << ")";
	return out;
}

