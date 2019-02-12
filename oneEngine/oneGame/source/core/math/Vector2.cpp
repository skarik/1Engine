#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "matrix/CMatrix.h"

namespace core
{
	template <typename Float> template <typename FloatOther>
	Vector2_T<Float>::Vector2_T (Vector3_T<FloatOther> const& old)
		: x((Float)old.x), y((Float)old.y)
		{}

	template <typename Float>
	std::ostream& operator<< (std::ostream& out, Vector2_T<Float> const& current)
	{
		out << '(' << current.x << ", " << current.y << ')';
		return out;
	}

	template <typename Float>
	Vector2_T<Float> Vector2_T<Float>::rvrMultMatx ( Matrix2x2 const& right ) const
	{
		return Vector2_T<Float>(
			right.pData[0]*x+right.pData[1]*y,
			right.pData[2]*x+right.pData[3]*y
			);
	}

	// Prototypng classes:

	template class Vector2_T<Real32>;
	template class Vector2_T<Real64>;

	template Vector2_T<Real32>::Vector2_T(Vector3_T<Real32> const& old);
	template Vector2_T<Real32>::Vector2_T(Vector3_T<Real64> const& old);
	template Vector2_T<Real64>::Vector2_T(Vector3_T<Real32> const& old);
	template Vector2_T<Real64>::Vector2_T(Vector3_T<Real64> const& old);
}