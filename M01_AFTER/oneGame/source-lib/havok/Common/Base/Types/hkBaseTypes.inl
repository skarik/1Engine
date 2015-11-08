/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


HK_FORCE_INLINE hkUint16 HK_CALL hkUFloat8::getEncodedFloat(hkUint8 index)
{
	// This table is declared as a local static variable to make sure it is initialized when it is first used
	static HK_ALIGN16(const hkUint16) encodedFloats[hkUFloat8::MAX_VALUE] = 
	{
		(hkUint16)0x0000, (hkUint16)0x0A3D, (hkUint16)0x123D, (hkUint16)0x175C, (hkUint16)0x1A3D, (hkUint16)0x1CCC, (hkUint16)0x1F5C, (hkUint16)0x20F5, (hkUint16)0x2193, (hkUint16)0x223B, (hkUint16)0x22EF, (hkUint16)0x23AF, (hkUint16)0x247D, (hkUint16)0x2559, (hkUint16)0x2643, (hkUint16)0x273E, 
		(hkUint16)0x2825, (hkUint16)0x28B4, (hkUint16)0x294D, (hkUint16)0x29F1, (hkUint16)0x2AA0, (hkUint16)0x2B5A, (hkUint16)0x2C22, (hkUint16)0x2CF7, (hkUint16)0x2DDB, (hkUint16)0x2ECF, (hkUint16)0x2FD4, (hkUint16)0x3075, (hkUint16)0x3109, (hkUint16)0x31A8, (hkUint16)0x3252, (hkUint16)0x3308, 
		(hkUint16)0x33CA, (hkUint16)0x3499, (hkUint16)0x3577, (hkUint16)0x3663, (hkUint16)0x3760, (hkUint16)0x3837, (hkUint16)0x38C8, (hkUint16)0x3962, (hkUint16)0x3A07, (hkUint16)0x3AB7, (hkUint16)0x3B74, (hkUint16)0x3C3D, (hkUint16)0x3D15, (hkUint16)0x3DFB, (hkUint16)0x3EF0, (hkUint16)0x3FF7, 
		(hkUint16)0x4088, (hkUint16)0x411E, (hkUint16)0x41BE, (hkUint16)0x4269, (hkUint16)0x4321, (hkUint16)0x43E4, (hkUint16)0x44B5, (hkUint16)0x4595, (hkUint16)0x4684, (hkUint16)0x4783, (hkUint16)0x484A, (hkUint16)0x48DB, (hkUint16)0x4977, (hkUint16)0x4A1E, (hkUint16)0x4AD0, (hkUint16)0x4B8E, 
		(hkUint16)0x4C59, (hkUint16)0x4D32, (hkUint16)0x4E1A, (hkUint16)0x4F12, (hkUint16)0x500D, (hkUint16)0x509B, (hkUint16)0x5132, (hkUint16)0x51D4, (hkUint16)0x5281, (hkUint16)0x533A, (hkUint16)0x53FF, (hkUint16)0x54D2, (hkUint16)0x55B3, (hkUint16)0x56A4, (hkUint16)0x57A6, (hkUint16)0x585C, 
		(hkUint16)0x58EF, (hkUint16)0x598C, (hkUint16)0x5A34, (hkUint16)0x5AE8, (hkUint16)0x5BA8, (hkUint16)0x5C75, (hkUint16)0x5D50, (hkUint16)0x5E3A, (hkUint16)0x5F34, (hkUint16)0x601F, (hkUint16)0x60AE, (hkUint16)0x6147, (hkUint16)0x61EA, (hkUint16)0x6298, (hkUint16)0x6353, (hkUint16)0x641A, 
		(hkUint16)0x64EF, (hkUint16)0x65D2, (hkUint16)0x66C5, (hkUint16)0x67C9, (hkUint16)0x686F, (hkUint16)0x6903, (hkUint16)0x69A2, (hkUint16)0x6A4B, (hkUint16)0x6B00, (hkUint16)0x6BC2, (hkUint16)0x6C91, (hkUint16)0x6D6E, (hkUint16)0x6E5A, (hkUint16)0x6F56, (hkUint16)0x7032, (hkUint16)0x70C2, 
		(hkUint16)0x715C, (hkUint16)0x7200, (hkUint16)0x72B0, (hkUint16)0x736C, (hkUint16)0x7435, (hkUint16)0x750C, (hkUint16)0x75F1, (hkUint16)0x76E6, (hkUint16)0x77ED, (hkUint16)0x7882, (hkUint16)0x7918, (hkUint16)0x79B8, (hkUint16)0x7A62, (hkUint16)0x7B19, (hkUint16)0x7BDC, (hkUint16)0x7CAD, 
		(hkUint16)0x7D8C, (hkUint16)0x7E7A, (hkUint16)0x7F79, (hkUint16)0x8044, (hkUint16)0x80D5, (hkUint16)0x8171, (hkUint16)0x8217, (hkUint16)0x82C8, (hkUint16)0x8386, (hkUint16)0x8451, (hkUint16)0x8529, (hkUint16)0x8611, (hkUint16)0x8708, (hkUint16)0x8808, (hkUint16)0x8895, (hkUint16)0x892C, 
		(hkUint16)0x89CD, (hkUint16)0x8A7A, (hkUint16)0x8B32, (hkUint16)0x8BF7, (hkUint16)0x8CC9, (hkUint16)0x8DAA, (hkUint16)0x8E9B, (hkUint16)0x8F9B, (hkUint16)0x9057, (hkUint16)0x90E9, (hkUint16)0x9186, (hkUint16)0x922E, (hkUint16)0x92E1, (hkUint16)0x93A0, (hkUint16)0x946C, (hkUint16)0x9547, 
		(hkUint16)0x9630, (hkUint16)0x972A, (hkUint16)0x981A, (hkUint16)0x98A8, (hkUint16)0x9941, (hkUint16)0x99E3, (hkUint16)0x9A91, (hkUint16)0x9B4B, (hkUint16)0x9C12, (hkUint16)0x9CE6, (hkUint16)0x9DC9, (hkUint16)0x9EBB, (hkUint16)0x9FBF, (hkUint16)0xA069, (hkUint16)0xA0FD, (hkUint16)0xA19C, 
		(hkUint16)0xA244, (hkUint16)0xA2F9, (hkUint16)0xA3BA, (hkUint16)0xA488, (hkUint16)0xA565, (hkUint16)0xA650, (hkUint16)0xA74C, (hkUint16)0xA82C, (hkUint16)0xA8BC, (hkUint16)0xA956, (hkUint16)0xA9FA, (hkUint16)0xAAA9, (hkUint16)0xAB65, (hkUint16)0xAC2D, (hkUint16)0xAD03, (hkUint16)0xADE8, 
		(hkUint16)0xAEDD, (hkUint16)0xAFE2, (hkUint16)0xB07C, (hkUint16)0xB112, (hkUint16)0xB1B1, (hkUint16)0xB25C, (hkUint16)0xB312, (hkUint16)0xB3D4, (hkUint16)0xB4A4, (hkUint16)0xB583, (hkUint16)0xB670, (hkUint16)0xB76E, (hkUint16)0xB83F, (hkUint16)0xB8D0, (hkUint16)0xB96B, (hkUint16)0xBA10, 
		(hkUint16)0xBAC1, (hkUint16)0xBB7E, (hkUint16)0xBC48, (hkUint16)0xBD20, (hkUint16)0xBE07, (hkUint16)0xBEFE, (hkUint16)0xC003, (hkUint16)0xC08F, (hkUint16)0xC126, (hkUint16)0xC1C7, (hkUint16)0xC273, (hkUint16)0xC32B, (hkUint16)0xC3EF, (hkUint16)0xC4C1, (hkUint16)0xC5A1, (hkUint16)0xC691, 
		(hkUint16)0xC791, (hkUint16)0xC851, (hkUint16)0xC8E3, (hkUint16)0xC980, (hkUint16)0xCA27, (hkUint16)0xCAD9, (hkUint16)0xCB98, (hkUint16)0xCC64, (hkUint16)0xCD3E, (hkUint16)0xCE27, (hkUint16)0xCF20, (hkUint16)0xD015, (hkUint16)0xD0A3, (hkUint16)0xD13B, (hkUint16)0xD1DD, (hkUint16)0xD28A, 
		(hkUint16)0xD344, (hkUint16)0xD40A, (hkUint16)0xD4DE, (hkUint16)0xD5C0, (hkUint16)0xD6B2, (hkUint16)0xD7B4, (hkUint16)0xD864, (hkUint16)0xD8F7, (hkUint16)0xD995, (hkUint16)0xDA3E, (hkUint16)0xDAF2, (hkUint16)0xDBB2, (hkUint16)0xDC80, (hkUint16)0xDD5C, (hkUint16)0xDE47, (hkUint16)0xDF42, 
	};
	return encodedFloats[index];
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
