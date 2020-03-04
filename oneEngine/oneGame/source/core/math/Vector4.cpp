#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "matrix/CMatrix.h"

namespace core
{
	template <typename Float> template <typename FloatOther>
	Vector4_T<Float>::Vector4_T (Vector3_T<FloatOther> const& old, FloatOther const& new_w = 0)
		: x((Float)old.x), y((Float)old.y), z((Float)old.z), w((Float)new_w)
		{}

	template <typename Float> template <typename FloatOther>
	Vector4_T<Float>::Vector4_T (const Vector2_T<FloatOther>& part1, const Vector2_T<FloatOther>& part2)
		: x((Float)part1.x), y((Float)part1.y), z((Float)part2.x), w((Float)part2.y)
		{}

	template <typename Float>
	std::ostream& operator<< (std::ostream& out, Vector4_T<Float> const& current)
	{
		out << '(' << current.x << ", " << current.y << ", " << current.z << ", " << current.w << ')';

		return out;
	}

	template <typename Float>
	Vector4_T<Float> Vector4_T<Float>::rvrMultMatx ( Matrix4x4 const& right ) const
	{
		return Vector4_T<Float>(
			right.pData[0]*x + right.pData[4]*y + right.pData[8]*z  + right.pData[12]*w,
			right.pData[1]*x + right.pData[5]*y + right.pData[9]*z  + right.pData[13]*w,
			right.pData[2]*x + right.pData[6]*y + right.pData[10]*z + right.pData[14]*w,
			right.pData[3]*x + right.pData[7]*y + right.pData[11]*z + right.pData[15]*w
			);
	}

	// Prototypng classes:

	template class Vector4_T<Real32>;
	template class Vector4_T<Real64>;

	template Vector4_T<Real32>::Vector4_T(Vector3_T<Real32> const& old, Real32 const& new_w);
	template Vector4_T<Real32>::Vector4_T(Vector3_T<Real64> const& old, Real64 const& new_w);
	template Vector4_T<Real64>::Vector4_T(Vector3_T<Real32> const& old, Real32 const& new_w);
	template Vector4_T<Real64>::Vector4_T(Vector3_T<Real64> const& old, Real64 const& new_w);

	template Vector4_T<Real32>::Vector4_T(const Vector2_T<Real32>& part1, const Vector2_T<Real32>& part2);
	template Vector4_T<Real32>::Vector4_T(const Vector2_T<Real64>& part1, const Vector2_T<Real64>& part2);
	template Vector4_T<Real64>::Vector4_T(const Vector2_T<Real32>& part1, const Vector2_T<Real32>& part2);
	template Vector4_T<Real64>::Vector4_T(const Vector2_T<Real64>& part1, const Vector2_T<Real64>& part2);
}