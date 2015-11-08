/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//
//	Sets the vertex bundle to the given vertices

inline void hkFourTransposedPoints::set(hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC, hkVector4Parameter vD)
{
	hkVector4 v0 = vA;
	hkVector4 v1 = vB;
	hkVector4 v2 = vC;
	hkVector4 v3 = vD;

	HK_TRANSPOSE4(v0, v1, v2, v3);

	m_vertices[0] = v0;
	m_vertices[1] = v1;
	m_vertices[2] = v2;
}

//
//	Extracts the vectors from the vertex bundle

inline void hkFourTransposedPoints::extract(hkVector4& vA, hkVector4& vB, hkVector4& vC, hkVector4& vD) const
{
	vA = m_vertices[0];
	vB = m_vertices[1];
	vC = m_vertices[2];
	vD.setZero();

	HK_TRANSPOSE4(vA, vB, vC, vD);
}

inline void hkFourTransposedPoints::extractWithW(hkVector4Parameter wABCD, hkVector4& vA, hkVector4& vB, hkVector4& vC, hkVector4& vD) const
{
	vA = m_vertices[0];
	vB = m_vertices[1];
	vC = m_vertices[2];
	vD = wABCD;

	HK_TRANSPOSE4(vA, vB, vC, vD);
}

//
//	Extracts the vector at index from the bundle

inline void hkFourTransposedPoints::extract(int index, hkVector4& vI) const
{
	HK_ASSERT(0x66e853d, index >= 0 && index < 4);

	{
		hkVector4 v[4];
		extract(v[0], v[1], v[2], v[3]);
		vI = v[index];
	}
}

//
//	Sets the vertex bundle to the same vertex

inline void hkFourTransposedPoints::setAll(hkVector4Parameter v)
{
	m_vertices[0].setPermutation<hkVectorPermutation::XXXX>(v);
	m_vertices[1].setPermutation<hkVectorPermutation::YYYY>(v);
	m_vertices[2].setPermutation<hkVectorPermutation::ZZZZ>(v);
}

//
//	Sets this = v - a

inline void hkFourTransposedPoints::setSub(const hkFourTransposedPoints& v, hkVector4Parameter a)
{
	hkVector4 ax;	ax.setAll(a.getComponent<0>());
	hkVector4 ay;	ay.setAll(a.getComponent<1>());
	hkVector4 az;	az.setAll(a.getComponent<2>());

	m_vertices[0].setSub(v.m_vertices[0], ax);
	m_vertices[1].setSub(v.m_vertices[1], ay);
	m_vertices[2].setSub(v.m_vertices[2], az);
}

//
//	Sets this = a - v

inline void hkFourTransposedPoints::setSub(hkVector4Parameter a, const hkFourTransposedPoints& v)
{
	hkVector4 ax;	ax.setAll(a.getComponent<0>());
	hkVector4 ay;	ay.setAll(a.getComponent<1>());
	hkVector4 az;	az.setAll(a.getComponent<2>());

	m_vertices[0].setSub(ax, v.m_vertices[0]);
	m_vertices[1].setSub(ay, v.m_vertices[1]);
	m_vertices[2].setSub(az, v.m_vertices[2]);
}

//
//	Sets this = v - a

inline void hkFourTransposedPoints::setSub(const hkFourTransposedPoints& v, const hkFourTransposedPoints& a)
{
	m_vertices[0].setSub(v.m_vertices[0], a.m_vertices[0]);
	m_vertices[1].setSub(v.m_vertices[1], a.m_vertices[1]);
	m_vertices[2].setSub(v.m_vertices[2], a.m_vertices[2]);
}

//
//	Sets this = v + a

inline void hkFourTransposedPoints::setAdd(const hkFourTransposedPoints& v, const hkFourTransposedPoints& a)
{
	m_vertices[0].setAdd(v.m_vertices[0], a.m_vertices[0]);
	m_vertices[1].setAdd(v.m_vertices[1], a.m_vertices[1]);
	m_vertices[2].setAdd(v.m_vertices[2], a.m_vertices[2]);
}

//
//	Sets this = vi * a, i = 0..3

inline void hkFourTransposedPoints::setMul(const hkFourTransposedPoints& v, hkVector4Parameter a)
{
	m_vertices[0].setMul(v.m_vertices[0], a);
	m_vertices[1].setMul(v.m_vertices[1], a);
	m_vertices[2].setMul(v.m_vertices[2], a);
}

//
//	Sets this = v * a

inline void hkFourTransposedPoints::setMul(const hkFourTransposedPoints& v, const hkFourTransposedPoints& a)
{
	m_vertices[0].setMul(v.m_vertices[0], a.m_vertices[0]);
	m_vertices[1].setMul(v.m_vertices[1], a.m_vertices[1]);
	m_vertices[2].setMul(v.m_vertices[2], a.m_vertices[2]);
}

//
//	Sets v0 = b*c0, v1 = b*c1, etc.

inline void hkFourTransposedPoints::setOuterProduct(hkVector4Parameter b, hkVector4Parameter c)
{
	m_vertices[0].setMul(c, b.getComponent<0>());
	m_vertices[1].setMul(c, b.getComponent<1>());
	m_vertices[2].setMul(c, b.getComponent<2>());
}


//
//	Sets v0 += b*c0, v1 = b*c1, etc.

inline void hkFourTransposedPoints::addOuterProduct(hkVector4Parameter b, hkVector4Parameter c)
{
	m_vertices[0].addMul(c, b.getComponent<0>());
	m_vertices[1].addMul(c, b.getComponent<1>());
	m_vertices[2].addMul(c, b.getComponent<2>());
}


//
//	Sets v0 -= b*c0, v1 = b*c1, etc.

inline void hkFourTransposedPoints::subOuterProduct(hkVector4Parameter b, hkVector4Parameter c)
{
	m_vertices[0].subMul(c, b.getComponent<0>());
	m_vertices[1].subMul(c, b.getComponent<1>());
	m_vertices[2].subMul(c, b.getComponent<2>());
}


//
//	Returns the dot3 of this and a

inline void hkFourTransposedPoints::dot3(hkVector4Parameter a, hkVector4& dotOut) const
{
	hkVector4 ax;	ax.setBroadcast<0>(a);
	hkVector4 ay;	ay.setBroadcast<1>(a);
	hkVector4 az;	az.setBroadcast<2>(a);

	ax.mul(m_vertices[0]);
	ay.mul(m_vertices[1]);
	az.mul(m_vertices[2]);

	dotOut.setAdd(ax, ay);
	dotOut.add(az);
}

//
//	Returns the dot3 of this and a

inline void hkFourTransposedPoints::dot3(const hkFourTransposedPoints& a, hkVector4& dotOut) const
{
	hkVector4 vx;	vx.setMul(m_vertices[0], a.m_vertices[0]);
	hkVector4 vy;	vy.setMul(m_vertices[1], a.m_vertices[1]);
	hkVector4 vz;	vz.setMul(m_vertices[2], a.m_vertices[2]);

	dotOut.setAdd(vx, vy);
	dotOut.add(vz);
}

//
//	Sets this = v + u * a

inline void hkFourTransposedPoints::setAddMul(const hkFourTransposedPoints& v, const hkFourTransposedPoints& u, hkVector4Parameter a)
{
	m_vertices[0].setAddMul(v.m_vertices[0], u.m_vertices[0], a);
	m_vertices[1].setAddMul(v.m_vertices[1], u.m_vertices[1], a);
	m_vertices[2].setAddMul(v.m_vertices[2], u.m_vertices[2], a);
}

//
//	Sets this = v - u * a

inline void hkFourTransposedPoints::setSubMul(const hkFourTransposedPoints& v, const hkFourTransposedPoints& u, hkVector4Parameter a)
{
	m_vertices[0].setSubMul(v.m_vertices[0], u.m_vertices[0], a);
	m_vertices[1].setSubMul(v.m_vertices[1], u.m_vertices[1], a);
	m_vertices[2].setSubMul(v.m_vertices[2], u.m_vertices[2], a);
}

//
//	Sets this += ui * a, i = 0..3

inline void hkFourTransposedPoints::addMul(const hkFourTransposedPoints& u, hkVector4Parameter a)
{
	m_vertices[0].addMul(u.m_vertices[0], a);
	m_vertices[1].addMul(u.m_vertices[1], a);
	m_vertices[2].addMul(u.m_vertices[2], a);
}

//
//	Sets this *= a

inline void hkFourTransposedPoints::mul(hkVector4Parameter a)
{
	m_vertices[0].mul(a);
	m_vertices[1].mul(a);
	m_vertices[2].mul(a);
}

//
//	Sets this = cross(n, v)

inline void hkFourTransposedPoints::setCross(hkVector4Parameter n, const hkFourTransposedPoints& v)
{
	const hkSimdReal nx = n.getComponent<0>();
	const hkSimdReal ny = n.getComponent<1>();
	const hkSimdReal nz = n.getComponent<2>();

	hkVector4 v0;	v0.setMul(v.m_vertices[2], ny);	// ny * z0123
	hkVector4 v1;	v1.setMul(v.m_vertices[0], nz);	// nz * x0123
	hkVector4 v2;	v2.setMul(v.m_vertices[1], nx);	// nx * y0123

	v0.subMul(v.m_vertices[1], nz);	// ny * z0123 - nz * y0123
	v1.subMul(v.m_vertices[2], nx);	// nz * x0123 - nx * z0123
	v2.subMul(v.m_vertices[0], ny);	// nx * y0123 - ny * x0123

	m_vertices[0] = v0;
	m_vertices[1] = v1;
	m_vertices[2] = v2;
}

//
//	Flips the signs, i.e. v0 = sign(ax) * v0, ..., v3 = sign(aw) * v3

inline void hkFourTransposedPoints::flipSigns(hkVector4Parameter a)
{
	m_vertices[0].setFlipSign(m_vertices[0], a);
	m_vertices[1].setFlipSign(m_vertices[1], a);
	m_vertices[2].setFlipSign(m_vertices[2], a);
}

//
//	Selects a set of vectors i.e. vi = mask(i) ? ai : bi, for i = 0..3

inline void hkFourTransposedPoints::setSelect(hkVector4ComparisonParameter mask, const hkFourTransposedPoints& trueVecs, const hkFourTransposedPoints& falseVecs)
{
	m_vertices[0].setSelect(mask, trueVecs.m_vertices[0], falseVecs.m_vertices[0]);
	m_vertices[1].setSelect(mask, trueVecs.m_vertices[1], falseVecs.m_vertices[1]);
	m_vertices[2].setSelect(mask, trueVecs.m_vertices[2], falseVecs.m_vertices[2]);
}

//
//	Sets this = Transpose(m) * v

inline void hkFourTransposedPoints::_setTransformedInverseDir(const hkMatrix3& m, const hkFourTransposedPoints& v)
{
	hkVector4 vDots0;	v.dot3(m.getColumn<0>(), vDots0);
	hkVector4 vDots1;	v.dot3(m.getColumn<1>(), vDots1);
	hkVector4 vDots2;	v.dot3(m.getColumn<2>(), vDots2);

	m_vertices[0] = vDots0;
	m_vertices[1] = vDots1;
	m_vertices[2] = vDots2;
}

//
//	Sets this = Transpose(Rotation(m)) * (v - Translation(m))

inline void hkFourTransposedPoints::_setTransformedInversePos(const hkTransform& m, const hkFourTransposedPoints& v)
{
	hkFourTransposedPoints tmp;
	tmp.setSub(v, m.getTranslation());

	setTransformedInverseDir(m.getRotation(), tmp);
}

//
//	Sets this = m * v

inline void hkFourTransposedPoints::_setRotatedDir(const hkMatrix3& m, const hkFourTransposedPoints& v)
{
	hkVector4 outLine0, outLine1, outLine2;
	{
		const hkVector4& c0 = m.getColumn<0>();
		const hkVector4& l0 = v.m_vertices[0];
		const hkSimdReal m00 = c0.getComponent<0>();
		const hkSimdReal m10 = c0.getComponent<1>();
		const hkSimdReal m20 = c0.getComponent<2>();

		outLine0.setMul(m00, l0);	// l0 = (m00 * ax, m00 * bx, m00 * cx, m00 * dx)
		outLine1.setMul(m10, l0);	// l1 = (m10 * ax, m10 * bx, m10 * cx, m10 * dx)
		outLine2.setMul(m20, l0);	// l2 = (m20 * ax, m20 * bx, m20 * cx, m20 * dx)
	}

	{
		const hkVector4& c1 = m.getColumn<1>();
		const hkVector4& l1 = v.m_vertices[1];
		const hkSimdReal m01 = c1.getComponent<0>();
		const hkSimdReal m11 = c1.getComponent<1>();
		const hkSimdReal m21 = c1.getComponent<2>();

		outLine0.addMul(m01, l1);	// l0 = (m00 * ax + m01 * ay, m00 * bx + m01 * by, m00 * cx + m01 * cy, m00 * dx + m01 * dy)
		outLine1.addMul(m11, l1);	// l1 = (m10 * ax + m11 * ay, m10 * bx + m11 * by, m10 * cx + m11 * cy, m10 * dx + m11 * dy)
		outLine2.addMul(m21, l1);	// l2 = (m20 * ax + m21 * ay, m20 * bx + m01 * by, m20 * cx + m21 * cy, m20 * dx + m21 * dy)
	}

	{
		const hkVector4& c2 = m.getColumn<2>();
		const hkVector4& l2 = v.m_vertices[2];
		const hkSimdReal m02 = c2.getComponent<0>();
		const hkSimdReal m12 = c2.getComponent<1>();
		const hkSimdReal m22 = c2.getComponent<2>();

		outLine0.addMul(m02, l2);	// l0 = (mLine0 * a, mLine0 * b, mLine0 * c, mLine0 * d)
		outLine1.addMul(m12, l2);	// l1 = (mLine1 * a, mLine1 * b, mLine1 * c, mLine1 * d)
		outLine2.addMul(m22, l2);	// l2 = (mLine2 * a, mLine2 * b, mLine2 * c, mLine2 * d)
	}

	m_vertices[0] = outLine0;
	m_vertices[1] = outLine1;
	m_vertices[2] = outLine2;
}

//
// 	Normalizes all 4 points

HK_FORCE_INLINE void hkFourTransposedPoints::normalize()
{
	hkVector4 lengthsSquared;
	dot3(*this, lengthsSquared);
	
	hkVector4 lengthsInv;
	
	lengthsInv.setSqrtInverse(lengthsSquared);

	m_vertices[0].mul(lengthsInv);
	m_vertices[1].mul(lengthsInv);
	m_vertices[2].mul(lengthsInv);
}

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
