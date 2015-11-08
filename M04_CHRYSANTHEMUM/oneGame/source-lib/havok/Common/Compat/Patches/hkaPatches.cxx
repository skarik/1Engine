/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// Animation specific product patches.
// This file is #included by hkaPatches.cpp

//=======
// 650b1
//=======

HK_PATCH_BEGIN("hkaSkeletonMapperData", 0, "hkaSkeletonMapperData", 1)
	HK_PATCH_MEMBER_ADDED("extractedMotionMapping", TYPE_VEC_12, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED_INT("mappingType", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaSkeleton", 0, "hkaSkeleton", 1)
	HK_PATCH_MEMBER_RENAMED("bones", "old_bones")
	HK_PATCH_MEMBER_ADDED("bones", TYPE_ARRAY_STRUCT, "hkaBone", 0)
	HK_PATCH_FUNCTION(hkaSkeleton_0_to_1)
	HK_PATCH_MEMBER_REMOVED("old_bones", TYPE_ARRAY_OBJECT, "hkaBone", 0)
	HK_PATCH_DEPENDS("hkaBone", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaSkeleton", 1, "hkaSkeleton", 2)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
HK_PATCH_END()

HK_PATCH_BEGIN("hkaAnimationBinding", 0, "hkaAnimationBinding", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
HK_PATCH_END()

HK_PATCH_BEGIN("hkaBoneAttachment", 0, "hkaBoneAttachment", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
HK_PATCH_END()

HK_PATCH_BEGIN("hkaMeshBinding", 0, "hkaMeshBinding", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
HK_PATCH_END()

HK_PATCH_BEGIN("hkaAnimation", 0, "hkaAnimation", 1)
	HK_PATCH_MEMBER_RENAMED("annotationTracks", "old_annotationTracks")
	HK_PATCH_MEMBER_ADDED("annotationTracks", TYPE_ARRAY_OBJECT, "hkaAnnotationTrack", 0)
	HK_PATCH_FUNCTION(hkaAnimation_0_to_1)
	HK_PATCH_MEMBER_REMOVED("old_annotationTracks", TYPE_ARRAY_OBJECT, "hkaAnnotationTrack", 0)
	HK_PATCH_DEPENDS("hkaAnnotationTrack", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaAnimationContainer", 0, "hkaAnimationContainer", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
HK_PATCH_END()


HK_PATCH_BEGIN("hkaAnimationPreviewColorContainer", 0, "hkaAnimationPreviewColorContainer", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
	HK_PATCH_MEMBER_REMOVED("previewColor", TYPE_ARRAY_OBJECT, "hkaAnimationPreviewColor", 0)
	HK_PATCH_MEMBER_ADDED("previewColor", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkaAnimationPreviewColor", 0)
	// This class cannot be exported.  Hence it is not versioned.
HK_PATCH_END()



HK_PATCH_BEGIN("hkaAnimationPreviewColor", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("color", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()


HK_PATCH_BEGIN("hkaFootstepAnalysisInfo", 0, "hkaFootstepAnalysisInfo", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
HK_PATCH_END()

HK_PATCH_BEGIN("hkaFootstepAnalysisInfoContainer", 0, "hkaFootstepAnalysisInfoContainer", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
HK_PATCH_END()

HK_PATCH_BEGIN("hkaSkeleton", 2, "hkaSkeleton", 3)
	HK_PATCH_MEMBER_ADDED("referenceFloats", TYPE_ARRAY_REAL, HK_NULL, 0)
	HK_PATCH_FUNCTION(hkaSkeleton_2_to_3)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkaQuantizedAnimationTrackCompressionParams", 0)
	HK_PATCH_MEMBER_ADDED("rotationTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("translationTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("scaleTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("floatingTolerance", TYPE_REAL, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkaQuantizedAnimation", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkaAnimation")
	HK_PATCH_MEMBER_ADDED("data", TYPE_ARRAY_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("endian", TYPE_INT, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkaAnimation", 1)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaMeshBinding", 1, "hkaMeshBinding", 2)
	HK_PATCH_MEMBER_ADDED("name", TYPE_CSTRING, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaWaveletCompressedAnimationCompressionParams", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("quantizationBits", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("blockSize", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("preserve", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("truncProp", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("useOldStyleTruncation", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("absolutePositionTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("relativePositionTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("rotationTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("scaleTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("absoluteFloatTolerance", TYPE_REAL, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaDeltaCompressedAnimationQuantizationFormat", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("maxBitWidth", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("preserved", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("numD", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("offsetIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("scaleIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("bitWidthIdx", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaWaveletCompressedAnimationQuantizationFormat", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("maxBitWidth", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("preserved", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("numD", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("offsetIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("scaleIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("bitWidthIdx", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaDeltaCompressedAnimation", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkaAnimation", HK_NULL)
	HK_PATCH_MEMBER_REMOVED("numberOfPoses", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("blockSize", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("qFormat", TYPE_STRUCT, "hkaDeltaCompressedAnimationQuantizationFormat", 0)
	HK_PATCH_MEMBER_REMOVED("quantizedDataIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("quantizedDataSize", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("staticMaskIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("staticMaskSize", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("staticDOFsIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("staticDOFsSize", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("numStaticTransformDOFs", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("numDynamicTransformDOFs", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("totalBlockSize", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("lastBlockSize", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("dataBuffer", TYPE_ARRAY_BYTE, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkaDeltaCompressedAnimationQuantizationFormat", 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkaAnimation", 2)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaWaveletCompressedAnimation", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkaAnimation", HK_NULL)
	HK_PATCH_MEMBER_REMOVED("numberOfPoses", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("blockSize", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("qFormat", TYPE_STRUCT, "hkaWaveletCompressedAnimationQuantizationFormat", 0)
	HK_PATCH_MEMBER_REMOVED("staticMaskIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("staticDOFsIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("numStaticTransformDOFs", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("numDynamicTransformDOFs", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("blockIndexIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("blockIndexSize", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("quantizedDataIdx", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("quantizedDataSize", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("dataBuffer", TYPE_ARRAY_BYTE, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkaAnimation", 2)
	HK_PATCH_DEPENDS("hkaWaveletCompressedAnimationQuantizationFormat", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaAnimation", 1, "hkaAnimation", 2)
	HK_PATCH_FUNCTION(hkaAnimation_1_to_2)
HK_PATCH_END()


HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkaPredictiveCompressedAnimationTrackCompressionParams", 0)
	HK_PATCH_MEMBER_ADDED("staticTranslationTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("staticRotationTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("staticScaleTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("staticFloatTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("dynamicTranslationTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("dynamicRotationTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("dynamicScaleTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("dynamicFloatTolerance", TYPE_REAL, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkaPredictiveCompressedAnimation", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkaAnimation")
	HK_PATCH_MEMBER_ADDED("compressedData", TYPE_ARRAY_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("intData", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("intArrayOffsets", TYPE_TUPLE_INT, HK_NULL, 8)
	HK_PATCH_MEMBER_ADDED("floatData", TYPE_ARRAY_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("floatArrayOffsets", TYPE_TUPLE_INT, HK_NULL, 3)
	HK_PATCH_MEMBER_ADDED("numBones", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("numFloatSlots", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("numFrames", TYPE_INT, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkaAnimation", 2)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

// Fixes for new serialized type system

HK_PATCH_BEGIN("hkaBoneAttachment", 1, "hkaBoneAttachment", 2)
	HK_PATCH_MEMBER_RENAMED("attachment", "old_attachment")
	HK_PATCH_MEMBER_ADDED("attachment",  TYPE_OBJECT, "hkReferencedObject", 0)
	HK_PATCH_FUNCTION(hkaBoneAttachment_1_to_2)		
	HK_PATCH_MEMBER_REMOVED("old_attachment", TYPE_OBJECT, "hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaMeshBinding", 2, "hkaMeshBinding", 3)
	HK_PATCH_MEMBER_RENAMED("mappings", "old_mappings")
	HK_PATCH_MEMBER_ADDED("mappings",  TYPE_ARRAY_STRUCT, "hkaMeshBindingMapping", 0)
	HK_PATCH_FUNCTION(hkaMeshBinding_2_to_3)
	HK_PATCH_MEMBER_REMOVED("old_mappings", TYPE_ARRAY_OBJECT, "hkaMeshBindingMapping", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaAnimation", 2, "hkaAnimation", 3)
	HK_PATCH_MEMBER_RENAMED("annotationTracks", "old_annotationTracks")
	HK_PATCH_MEMBER_ADDED("annotationTracks",  TYPE_ARRAY_STRUCT, "hkaAnnotationTrack", 0)
	HK_PATCH_FUNCTION(hkaAnimation_2_to_3)
	HK_PATCH_MEMBER_REMOVED("old_annotationTracks", TYPE_ARRAY_OBJECT, "hkaAnnotationTrack", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkaPredictiveCompressedAnimation", 0, "hkaPredictiveCompressedAnimation", 1)
	HK_PATCH_MEMBER_RENAMED("intArrayOffsets", "old_intArrayOffsets")
	HK_PATCH_MEMBER_ADDED("intArrayOffsets", TYPE_TUPLE_INT, HK_NULL, 9)
	HK_PATCH_MEMBER_ADDED("firstFloatBlockScaleAndOffsetIndex", TYPE_INT, HK_NULL, 0)
	HK_PATCH_FUNCTION(hkaPredictiveCompressedAnimation_0_to_1)
	HK_PATCH_MEMBER_REMOVED("old_intArrayOffsets", TYPE_TUPLE_INT, HK_NULL, 8)
HK_PATCH_END()

// tuple/tuple fix

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
