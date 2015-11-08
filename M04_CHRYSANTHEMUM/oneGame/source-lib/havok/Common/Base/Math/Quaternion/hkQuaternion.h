/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_MATH_QUATERNION_H
#define HK_MATH_QUATERNION_H

#ifndef HK_MATH_MATH_H
#	error Please include Common/Base/hkBase.h instead of this file.
#endif


/// Stores a unit quaternion.
/// "Unitness" is not enforced, but certain functions (such as division
/// and inversion ) DO assume that the quaternions are unit quaternions.
///
/// \b NOTE - The constructor taking (hkVector4, hkReal) does not set real,imag
///                     but assumes [angle,axis] format.
///
/// \b NOTE - getAxis() returns a valid value only if the angle of the rotation
///                     is nonzero, since the axis is undefined for a rotation of zero degrees
///                     and it is not possible to return it even if the Quaternion was created
///                     from an angle and axis. It was decided that returning an arbitrary
///                     axis would be confusing, so it is up to you to check that the
///                     stored axis is nonzero before attempting to get the axis.
///                     Otherwise, a division by zero Will occur.
///
/// Conventions adopted:
///
///    - When specified by an (angle,axis) pair, the standard mathematical convention is
///      followed, with the angle taken to be such that Quaternion(PI/2, Vector4(0,0,1))
///      will rotate the X vector (1,0,0) to the Y vector (0,0,1). If you are converting to/from a system
///      where such a quaternion would rotate X to -Y, then you will need to flip the angle or axis
///      to convert successfully.
///
///    - p/q is assumed to be p * ( q-inverse ). This
///      is an arbitrary decision since ( q-inverse ) * p is also plausible.
///
///    - The angle extracted by getAngle() is ALWAYS in the range 0 -> PI, since we
///      wish to resolve the ambiguity of storing R(a,v) as either q(a,v) or -q(a,v).
///      It would be nice to assume the real part of q was always positive,
///      but operations involving conversion from transform to quaternion or
///      quaternion multiplication may not preserve this. The best we can do
///      without more code checking) is to let R(a,v) be stored as either,
///      and overload the == operator to check both possibilities.
///      Note that the storage is (imag, real): ((xyz), w)
class hkQuaternion
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkQuaternion);
		HK_DECLARE_POD_TYPE();

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
			/// Empty constructor.
		HK_FORCE_INLINE hkQuaternion() { }

			/// Constructs a quaternion directly from its component elements.
			/// The imaginary part is (ix,iy,iz) and the real part r.
		HK_FORCE_INLINE hkQuaternion(hkReal ix, hkReal iy, hkReal iz, hkReal r);
		HK_FORCE_INLINE hkQuaternion(hkSimdRealParameter ix, hkSimdRealParameter iy, hkSimdRealParameter iz, hkSimdRealParameter r);

			/// Constructs a quaternion from the rotation matrix r.
		explicit HK_FORCE_INLINE hkQuaternion(const hkRotation& r);

			/// Constructs a quaternion from an axis and an angle. The rotation
			/// will take place about that axis by angle radians.
			/// N.B. The axis MUST be normalized.
		HK_FORCE_INLINE hkQuaternion(hkVector4Parameter axis, hkReal angle);
#endif

			/// Copy this quaternion.
		HK_FORCE_INLINE void operator= (const hkQuaternion& q);

			/// Set this quaternion with imaginary part (ix,iy,iz) and real part r.
		HK_FORCE_INLINE void set(hkReal ix, hkReal iy, hkReal iz, hkReal r);
		HK_FORCE_INLINE void set(hkSimdRealParameter ix, hkSimdRealParameter iy, hkSimdRealParameter iz, hkSimdRealParameter r);

			/// Get a reference to an identity quaternion.
			/// Note that setIdentity will likely be faster as it does not incur a memory access.
		HK_FORCE_INLINE static const hkQuaternion& HK_CALL getIdentity();

			/// Sets this quaternion to the unit quaternion. (this = (0, 0, 0, 1))
		HK_FORCE_INLINE void setIdentity();

			/// Sets this quaternion to be the inverse of the quaternion q. (this = q^-1)
		HK_FORCE_INLINE void setInverse( hkQuaternionParameter q );



			/// Sets this quaternion to the product of r by q. (this = r * q)
		HK_FORCE_INLINE void setMul(hkSimdRealParameter r, hkQuaternionParameter q);
			/// Adds the product of r and q to this quaternion. (this += r * q)
		HK_FORCE_INLINE void addMul(hkSimdRealParameter r, hkQuaternionParameter q);
			/// Sets this quaternion to the product of q0 and q1. (this = q0 * q1)
		HK_FORCE_INLINE void setMul(hkQuaternionParameter q0, hkQuaternionParameter q1);
			/// Multiplies this quaternion by q. (this *= q)
		HK_FORCE_INLINE void mul(hkQuaternionParameter q);

			/// Set this quaternion to be the product of q0 and the inverse of q1.
			/// (this = (q0 * q1^-1)
		HK_FORCE_INLINE void setMulInverse(hkQuaternionParameter q0, hkQuaternionParameter q1);

			/// Set this quaternion to be the product of inverse q0 and q1.
			/// (this = (q0^-1 * q1)
		HK_FORCE_INLINE void setInverseMul(hkQuaternionParameter q0, hkQuaternionParameter q1);

			/// returns an estimate for an angle to get from 'this' to 'to'.
			/// This function has a pretty good accuracy for angles less than 20 degrees
			/// and underestimates the angle for bigger values.
		HK_FORCE_INLINE void estimateAngleTo(hkQuaternionParameter to, hkVector4& angleOut) const;

			/// Sets this quaternion to be the shortest rotation which brings 'from' to 'to'.
			/// NOTE: The vectors 'from ' and 'to' must be normalized.
		HK_FORCE_INLINE void setShortestRotation(hkVector4Parameter from, hkVector4Parameter to);

			/// Sets this quaternion to be the shortest rotation which brings 'from' to 'to'.
			/// NOTE: The vectors 'from ' and 'to' must be normalized.
			/// This version is damped and the result interpolates from 'from' to to' as gain goes from 0 to 1.
			/// This is similar to scaling the angle of rotation according to the gain.
		
		HK_FORCE_INLINE void setShortestRotationDamped(hkVector4Parameter from, hkVector4Parameter to, hkSimdRealParameter gain);

			/// Sets/initializes this quaternion given a rotation axis and angle.
			/// N.B. The axis MUST be normalized.
		void setAxisAngle(hkVector4Parameter axis, hkReal angle);
		void setAxisAngle(hkVector4Parameter axis, hkSimdRealParameter angle);
		void setAxisAngle_Approximate(hkVector4Parameter axis, hkReal angle);
		void setAxisAngle_Approximate(hkVector4Parameter axis, hkSimdRealParameter angle);

			/// Sets self to a rotation around the given Euler angles, assuming the angles are in radians.
		void setFromEulerAngles(hkReal roll, hkReal pitch, hkReal yaw);
		void setFromEulerAngles(hkSimdRealParameter roll, hkSimdRealParameter pitch, hkSimdRealParameter yaw);
		void setFromEulerAngles_Approximate(hkReal roll, hkReal pitch, hkReal yaw);
		void setFromEulerAngles_Approximate(hkSimdRealParameter roll, hkSimdRealParameter pitch, hkSimdRealParameter yaw);

			/// Sets/initializes this quaternion from a given rotation matrix.
			/// The rotation r must be orthonormal.
		void set(const hkRotation& r);

			/// HK_FORCE_INLINEd. Sets/initializes this quaternion from a given rotation matrix.
			/// The rotation r must be orthonormal.
		HK_FORCE_INLINE void _set(const hkRotation& r);

			/// Removes the component of this quaternion that represents a rotation (twist) around the given axis.
			/// In more accurate terms, it converts this quaternion (q) to the smallest (smallest angle)
			/// rotation (q') that still satisfies q * axis = q' * axis.
			/// This is done by applying calculating axis' = q * axis. Then, from axis and axis' a perpendicular
			/// vector (v) is calculated, as well as the angle between axis and axis' (ang). The result of the
			/// operation is the rotation specified by the angle (ang) and the axis (v).
		void removeAxisComponent (hkVector4Parameter axis);
	
			/// Decomposes the quaternion and returns the amount of rotation around the given axis, and the rest
			/// (this) == Quaternion(axis, angle) * rest , where "rest" is the rest of rotation. This is done
			/// by calculating "rest" using removeAxisComponent() and then calculating (axis,angle) as
			/// this * inv (rest).
		
		void decomposeRestAxis (hkVector4Parameter axis, hkQuaternion& restOut, hkSimdReal& angleOut) const;

			/// Calculates the spherical linear interpolation between q0 and q1
			/// parameterized by t. If t is 0 then the result will be q0.
		void setSlerp(hkQuaternionParameter q0, hkQuaternionParameter q1, hkSimdRealParameter t);
		
		/* access access */

			/// Sets the real component of this quaternion.
		HK_FORCE_INLINE void setReal(hkReal r);
			/// Returns the real component of this quaternion.
		HK_FORCE_INLINE hkReal getReal() const;

		HK_FORCE_INLINE const hkSimdReal getRealPart() const;
		HK_FORCE_INLINE void setRealPart(hkSimdRealParameter r);

			/// Sets the imaginary component of this quaternion.
		HK_FORCE_INLINE void setImag(hkVector4Parameter i);
			/// Returns a read only reference to the imaginary component of this quaternion.
		HK_FORCE_INLINE const hkVector4& getImag() const;

			/// This method extracts the angle of rotation, always returning it as a real in
			/// the range [0,PI].
			/// NOTE - The standard mathematical convention is followed, with the angle taken to be
			/// such that Quaternion((0,0,1), PI/2,) will rotate the X vector (1,0,0) to the Y vector (0,0,1).
			/// If you are converting to/from a system where such a quaternion would rotate X to -Y,
			/// then you will need to flip the angle or axis to convert successfully.
			/// \code
			/// hkQuaternion q((1,0,0), -0.7);
			/// hkReal ang = q.getAngle() ;
			/// // ang is now 0.7 radians (and a call to getAxis() will return (-1,0,0) )
			/// \endcode
		HK_FORCE_INLINE hkReal getAngle() const;

			/// Return the normalized axis of rotation IF DEFINED.
			/// The axis is not defined for a zero rotation (getAngle() returns 0.0f, or getReal() returns 1 or -1).
			/// The direction of the axis is determined by the sign of the angle returned by getAngle(), so that
			/// getAxis() and getAngle() return consistent values. N.B. getAngle() always returns a *positive* value.
			///
			/// NOTE - The standard mathematical convention is followed, with the angle taken to be
			/// such that Quaternion((0,0,1), PI/2) will rotate the X vector (1,0,0) to the Y vector (0,0,1).
			/// If you are converting to/from a system where such a quaternion would rotate X to -Y,
			/// then you will need to flip the angle or axis to convert successfully.
			///
			/// NOTE - getAxis() returns a valid value ONLY if the angle of the rotation
			/// is (numerically) nonzero, since the axis is undefined for a rotation of zero degrees
			/// and it is not possible to return it even if the Quaternion was created
			/// from an angle and axis. It was decided that returning an arbitrary
			/// axis would be confusing, so it is up to the user to check that the
			/// stored axis is nonzero before attempting to get the axis.
			/// Otherwise, a division by zero will occur. The function hasValidAxis()
			/// may be called to determine if calls to getAxis() will return a valid vector.
			/// e.g., hkQuaternion((1,0,0), 1.5 PI) will return:
			///     - 0.5 PI as angle
			///     - (-1,0,0) as axis.
		HK_FORCE_INLINE void getAxis(hkVector4 &axis) const;

			/// Determines if the quaternion has a valid axis of rotation. See getAxis()
		HK_FORCE_INLINE hkBool32 hasValidAxis() const;

			/// Read only access to the i'th component of this quaternion. (stored as (ix,iy,iz,real))
		HK_FORCE_INLINE const hkReal& operator()(int i) const;
		template <int I> HK_FORCE_INLINE const hkSimdReal getComponent() const;

			/// Checks that all elements are valid numbers, and length is 1.0 (within 1e-3)
		hkBool32 isOk() const;

			/// Sets the quaternion to the given quaternion q, eventually transforming it so q and qReference are in the same hemisphere
		HK_FORCE_INLINE void setClosest(hkQuaternionParameter q, hkQuaternionParameter qReference);

			///	Computes the logarithm of a quaternion
		HK_FORCE_INLINE void getLog(hkVector4& vOut) const;

			///	Set the quaternion as the exponential of the given (axis-angle) vector
		HK_FORCE_INLINE void setExp(hkVector4Parameter vIn);
		HK_FORCE_INLINE void setExp_Approximate(hkVector4Parameter vIn);

			/// Set self to a quaternion with an axis perpendicular to \a from and no rotation angle.
		void setFlippedRotation(hkVector4Parameter from);

			/// Normalizes the quaternion. (this = q/|q|)
		HK_FORCE_INLINE void normalize();

			/// Sets/initializes this quaternion from a given rotation matrix.
		HK_FORCE_INLINE void setAndNormalize(const hkRotation& r);

		//
		// advanced interface
		//

			/// Normalizes the quaternion. (this = q/|q|)
		template <hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void normalize();

			/// Sets/initializes this quaternion from a given rotation matrix.
		template <hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void setAndNormalize(const hkRotation& r);


	public:

		hkVector4 m_vec;

};


#endif //HK_MATH_QUATERNION_H

/*
 * Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20120119)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2012
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available at www.havok.com/tryhavok.
 * 
 */
