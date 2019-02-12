#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "matrix/CMatrix.h"

namespace core
{
	template <typename Float>
	const Vector3_T<Float> Vector3_T<Float>::zero		= Vector3_T<Float>(0, 0, 0);
	template <typename Float>
	const Vector3_T<Float> Vector3_T<Float>::forward	= Vector3_T<Float>(1, 0, 0);
	template <typename Float>
	const Vector3_T<Float> Vector3_T<Float>::up			= Vector3_T<Float>(0, 0, 1);
	template <typename Float>
	const Vector3_T<Float> Vector3_T<Float>::left		= Vector3_T<Float>(0, 1, 0);

	template <typename Float> template <typename FloatOther>
	Vector3_T<Float>::Vector3_T (Vector2_T<FloatOther> const& old, Float const new_z)
		: x((Float)old.x), y((Float)old.y), z((Float)new_z)
		{}

	template <typename Float> template <typename FloatOther>
	Vector3_T<Float>::Vector3_T (Vector4_T<FloatOther> const& old)
		: x((Float)old.x), y((Float)old.y), z((Float)old.z)
		{}

	template <typename Float>
	std::ostream& operator<< (std::ostream& out, Vector3_T<Float> const& current)
	{
		out << '(' << current.x << ", " << current.y << ", " << current.z << ')';
		return out;
	}

	template <typename Float>
	Vector3_T<Float> Vector3_T<Float>::rvrMultMatx ( Matrix4x4 const& right ) const
	{
		return Vector3_T<Float>(
			right.pData[0]*x + right.pData[4]*y + right.pData[8]*z  + right.pData[3],
			right.pData[1]*x + right.pData[5]*y + right.pData[9]*z  + right.pData[7],
			right.pData[2]*x + right.pData[6]*y + right.pData[10]*z + right.pData[11]
			);
	}

	template <typename Float>
	Vector3_T<Float> Vector3_T<Float>::rvrMultMatx ( Matrix3x3 const& right ) const
	{
		return Vector3_T<Float>(
			right.pData[0]*x + right.pData[3]*y + right.pData[6]*z,
			right.pData[1]*x + right.pData[4]*y + right.pData[7]*z,
			right.pData[2]*x + right.pData[5]*y + right.pData[8]*z
			);
	}

	// Prototypng classes:

	template class Vector3_T<Real32>;
	template class Vector3_T<Real64>;

	template Vector3_T<Real32>::Vector3_T(Vector2_T<Real32> const& old, Real32 const new_z);
	template Vector3_T<Real32>::Vector3_T(Vector2_T<Real64> const& old, Real32 const new_z);
	template Vector3_T<Real64>::Vector3_T(Vector2_T<Real32> const& old, Real64 const new_z);
	template Vector3_T<Real64>::Vector3_T(Vector2_T<Real64> const& old, Real64 const new_z);

	template Vector3_T<Real32>::Vector3_T(Vector4_T<Real32> const& old);
	template Vector3_T<Real32>::Vector3_T(Vector4_T<Real64> const& old);
	template Vector3_T<Real64>::Vector3_T(Vector4_T<Real32> const& old);
	template Vector3_T<Real64>::Vector3_T(Vector4_T<Real64> const& old);

}