/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_DYNAMICS2_CONSTRAINT_ATOM_H
#define HK_DYNAMICS2_CONSTRAINT_ATOM_H


#include <Physics/ConstraintSolver/Constraint/Contact/hkpSimpleContactConstraintInfo.h>
#include <Common/Base/Types/Physics/ContactPoint/hkContactPoint.h>
#include <Physics/ConstraintSolver/Solve/hkpSolve.h>
#include <Physics/Dynamics/Constraint/hkpConstraintInfo.h>
#include <Physics/ConstraintSolver/Constraint/Atom/hkpSimpleConstraintAtom.h>

extern const class hkClass hkpMovingSurfaceModifierConstraintAtomClass;

extern const class hkClass hkpViscousSurfaceModifierConstraintAtomClass;

extern const class hkClass hkpCenterOfMassChangerModifierConstraintAtomClass;

extern const class hkClass hkpMassChangerModifierConstraintAtomClass;

extern const class hkClass hkpSoftContactModifierConstraintAtomClass;

extern const class hkClass hkpModifierConstraintAtomClass;

extern const class hkClass hkpPulleyConstraintAtomClass;

extern const class hkClass hkpLinMotorConstraintAtomClass;

extern const class hkClass hkpLinFrictionConstraintAtomClass;

extern const class hkClass hkpRagdollMotorConstraintAtomClass;

extern const class hkClass hkpAngMotorConstraintAtomClass;

extern const class hkClass hkpAngFrictionConstraintAtomClass;

extern const class hkClass hkpConeLimitConstraintAtomClass;

extern const class hkClass hkpTwistLimitConstraintAtomClass;

extern const class hkClass hkpAngLimitConstraintAtomClass;

extern const class hkClass hkpAngConstraintAtomClass;

extern const class hkClass hkp2dAngConstraintAtomClass;

extern const class hkClass hkpLinLimitConstraintAtomClass;

extern const class hkClass hkpLinSoftConstraintAtomClass;

extern const class hkClass hkpLinConstraintAtomClass;

extern const class hkClass hkpOverwritePivotConstraintAtomClass;

extern const class hkClass hkpSetLocalRotationsConstraintAtomClass;

extern const class hkClass hkpSetLocalTranslationsConstraintAtomClass;

extern const class hkClass hkpSetLocalTransformsConstraintAtomClass;

extern const class hkClass hkpStiffSpringConstraintAtomClass;

extern const class hkClass hkpBallSocketConstraintAtomClass;

extern const class hkClass hkpSimpleContactConstraintAtomClass;

extern const class hkClass hkpBridgeAtomsClass;

extern const class hkClass hkpBridgeConstraintAtomClass;

extern const class hkClass hkpConstraintAtomClass;

extern const class hkClass hkpIgnoreModifierConstraintAtomClass;

extern const class hkClass hkpSetupStabilizationAtomClass;

HK_REFLECTION_CLASSFILE_DESTINATION("../../../Dynamics/Constraint/Atom")

class hkContactPoint;
class hkpConstraintMotor;
class hkpContactPointPropertiesStream;

typedef void (HK_CALL *hkConstraintAtomBuildJacobianFunc) ( class hkpConstraintData* m_constraintData, const hkpConstraintQueryIn &in, hkpConstraintQueryOut &out );


/// Special atom used to implement a custom atomic constraint that generates its jacobians calling the buildJacobian 
/// function in the hkpConstraintData member.
/// This atom is used to allow to call the old hkpConstraintData classes
struct hkpBridgeConstraintAtom: public hkpConstraintAtom
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONSTRAINT, hkpBridgeConstraintAtom );
	HK_DECLARE_REFLECTION();

	hkpBridgeConstraintAtom(  ): hkpConstraintAtom( TYPE_BRIDGE ){ }

	hkpBridgeConstraintAtom(hkFinishLoadedObjectFlag f) : hkpConstraintAtom(f)
	{
		if( f.m_finishing )
		{
			init(m_constraintData);
		}
	}

		// call this to do stuff not done in the constructor yet
	void init (class hkpConstraintData* m_constraintData);

	HK_FORCE_INLINE hkpConstraintAtom* next()		{ return (this+1); }

		// bridge atoms are always the last atom, so no need to increment solver result, just make sure the
		// program crashes if the result of this function is used
	HK_FORCE_INLINE int numSolverResults() const    { return 100000; }

	// addToConstraintInfo not needed

	hkConstraintAtomBuildJacobianFunc       m_buildJacobianFunc;	//+nosave +overridetype(void*)

	class hkpConstraintData* m_constraintData; //+owned(false)
};

struct hkpBridgeAtoms
{
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONSTRAINT, hkpBridgeAtoms );

	struct hkpBridgeConstraintAtom m_bridgeAtom;

	hkpBridgeAtoms(){}

	hkpBridgeAtoms(hkFinishLoadedObjectFlag f) : m_bridgeAtom(f) {}

	// get a pointer to the first atom
	const hkpConstraintAtom* getAtoms() const { return &m_bridgeAtom; }

	// get the size of all atoms (we can't use sizeof(*this) because of align16 padding)
	int getSizeOfAllAtoms() const               { return hkGetByteOffsetInt(this, &m_bridgeAtom+1); }
};


	/// hkpSimpleContactConstraintAtom holds contact information for a single hkpSimpleContactConstraintData.
	///
	/// It is for internal use only and is unique in the following ways:
	///     - it is not a member of the owning hkpConstraintData, it is allocated externally
	///     - its size is dynamic and varies depending on the number of contact points in the constraint
	///     - it is a stand-alone constraint, therefore it derives from hkpConstraintAtom and cannot be followed by any other atom
struct hkpSimpleContactConstraintAtom : public hkpConstraintAtom
{
		HK_DECLARE_REFLECTION();

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONSTRAINT, hkpSimpleContactConstraintAtom );

	public:
		// Size of hkpSimpleContactConstraintAtom is dynamically changed by the engine. It holds up to 256 contact points.
		// We initialize the size of the atom to what it is when no contact points are present.
		hkpSimpleContactConstraintAtom() : hkpConstraintAtom(TYPE_CONTACT) {}

		hkpSimpleContactConstraintAtom(hkFinishLoadedObjectFlag f);

		HK_FORCE_INLINE hkContactPoint* getContactPoints() const { return const_cast<hkContactPoint*>( reinterpret_cast<const hkContactPoint*>( this+1 ) ); }
		HK_FORCE_INLINE int getContactPointPropertiesStriding() const;
		HK_FORCE_INLINE hkpContactPointPropertiesStream* getContactPointPropertiesStream() const { return const_cast<hkpContactPointPropertiesStream*>( reinterpret_cast<const hkpContactPointPropertiesStream*>( hkAddByteOffsetConst( getContactPoints(), sizeof(hkContactPoint) * m_numReservedContactPoints ) ) ); }
		HK_FORCE_INLINE hkpContactPointPropertiesStream* getContactPointPropertiesStream(int i) const;

	public:
		HK_FORCE_INLINE hkpConstraintAtom* next() const { HK_ASSERT2(0x5b5a6955, false, "Not implemented. Need to compute the entire size of contact points & properties."); return HK_NULL; }
		HK_FORCE_INLINE int numSolverResults() const    { return m_numContactPoints+3; }

		HK_FORCE_INLINE void addToConstraintInfo(hkpConstraintInfo& infoOut) const
		{
			int size = m_numContactPoints;

			infoOut.m_sizeOfSchemas    += HK_SIZE_OF_JACOBIAN_HEADER_SCHEMA 
										+ (size >> 1) * HK_SIZE_OF_JACOBIAN_PAIR_CONTACT_SCHEMA 
				                        + (size & 1) * HK_SIZE_OF_JACOBIAN_SINGLE_CONTACT_SCHEMA 
										+ HK_SIZE_OF_JACOBIAN_2D_FRICTION_SCHEMA;

			infoOut.m_maxSizeOfSchema = infoOut.m_sizeOfSchemas + (HK_SIZE_OF_JACOBIAN_3D_FRICTION_SCHEMA - HK_SIZE_OF_JACOBIAN_2D_FRICTION_SCHEMA);
			infoOut.m_numSolverResults   += size + 2;
			infoOut.m_numSolverElemTemps += size + (2 + 1); // extra one for friction

			if ( size >= 2 )
			{
				infoOut.m_sizeOfSchemas   += HK_SIZE_OF_JACOBIAN_3D_FRICTION_SCHEMA - HK_SIZE_OF_JACOBIAN_2D_FRICTION_SCHEMA;
				infoOut.m_numSolverResults   += 1; // is that needed ?? solver results are stroed in the info struct
				infoOut.m_numSolverElemTemps += 1 + 0; // just one elem for both 2d & 3d friction anyways.
			}

			if (m_info.m_rollingFrictionMultiplier != 0.0f)
			{
				infoOut.m_sizeOfSchemas += HK_SIZE_OF_JACOBIAN_2D_ROLLING_FRICTION_SCHEMA;
				infoOut.m_maxSizeOfSchema += HK_SIZE_OF_JACOBIAN_2D_ROLLING_FRICTION_SCHEMA;
				infoOut.m_numSolverResults += 2; 
				infoOut.m_numSolverElemTemps += 2 + 1;
			}
		}

	public:
		hkUint16 m_sizeOfAllAtoms;
		hkUint16 m_numContactPoints;
		hkUint16 m_numReservedContactPoints;

			// 8 bytes left from here

			// Extra storage for bodyA. Holds hkpShapeKey hierarchy, by default.
		hkUint8 m_numUserDatasForBodyA;

			// Extra storage for bodyA. Holds hkpShapeKey hierarchy, by default.
		hkUint8 m_numUserDatasForBodyB;

			// store striding in here to avoid recomputing in getContactPointPropsStriding()..
		hkUint8 m_contactPointPropertiesStriding;

			// Maximum number for contact points. The limitation is driven by buffer size on SPU, and by contactPointProperties size.
		hkUint16 m_maxNumContactPoints; 


		HK_ALIGN16(class hkpSimpleContactConstraintDataInfo m_info);

};


// Only include it after the hkpSimpleContactConstraintAtom is defined
#include <Physics/ConstraintSolver/Constraint/Contact/hkpContactPointProperties.h>

HK_FORCE_INLINE int hkpSimpleContactConstraintAtom::getContactPointPropertiesStriding() const { return m_contactPointPropertiesStriding; }
HK_FORCE_INLINE hkpContactPointPropertiesStream* hkpSimpleContactConstraintAtom::getContactPointPropertiesStream(int i) const 
{
	const hkContactPoint* endContactPoint = hkAddByteOffsetConst( getContactPoints(), sizeof(hkContactPoint) * m_numReservedContactPoints);
	const hkpContactPointPropertiesStream* beginningOfProperties = reinterpret_cast<const hkpContactPointPropertiesStream*>( endContactPoint );
	return const_cast<hkpContactPointPropertiesStream*>( hkAddByteOffsetConst(  beginningOfProperties, HK_HINT_SIZE16(i) * HK_HINT_SIZE16( getContactPointPropertiesStriding())  ) );
}



#	define HK_GET_LOCAL_CONTACT_ATOM(ATOM) ATOM
#	define HK_CONTACT_ATOM_SET_PPU(ATOM)



	//	************************ Soft Contact **************************
	//	************************ Soft Contact **************************
	//	************************ Soft Contact **************************

struct hkpSoftContactModifierConstraintAtom : public hkpModifierConstraintAtom
{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CONSTRAINT, hkpSoftContactModifierConstraintAtom );
		HK_DECLARE_REFLECTION();

	public:

		hkpSoftContactModifierConstraintAtom() : hkpModifierConstraintAtom(TYPE_MODIFIER_SOFT_CONTACT, sizeof(hkpSoftContactModifierConstraintAtom)),  m_tau(0.1f), m_maxAcceleration( 20.0f) { }
		hkpSoftContactModifierConstraintAtom(hkFinishLoadedObjectFlag f) : hkpModifierConstraintAtom(f) {}

		void collisionResponseBeginCallback( const hkContactPoint& cp, struct hkpSimpleConstraintInfoInitInput& inA, struct hkpBodyVelocity& velA, hkpSimpleConstraintInfoInitInput& inB, hkpBodyVelocity& velB);
		void collisionResponseEndCallback(   const hkContactPoint& cp, hkReal impulseApplied, struct hkpSimpleConstraintInfoInitInput& inA, struct hkpBodyVelocity& velA, hkpSimpleConstraintInfoInitInput& inB, hkpBodyVelocity& velB);
		HK_FORCE_INLINE int numSolverResults() const { return 0; }

		static const hkUint16 ADDITIONAL_SCHEMA_SIZE = 0;

		int getConstraintInfo( hkpConstraintInfo& info ) const	{ return hkpConstraintAtom::CALLBACK_REQUEST_NONE; }

	public:

		hkReal m_tau;

			/// The maximum acceleration, the solver will apply
		hkReal m_maxAcceleration;
};


	//	************************ Viscous Surface **************************
	//	************************ Viscous Surface **************************
	//	************************ Viscous Surface **************************

struct hkpViscousSurfaceModifierConstraintAtom : public hkpModifierConstraintAtom
{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CONSTRAINT, hkpViscousSurfaceModifierConstraintAtom );
		HK_DECLARE_REFLECTION();

	public:

		hkpViscousSurfaceModifierConstraintAtom() : hkpModifierConstraintAtom(TYPE_MODIFIER_VISCOUS_SURFACE, sizeof(hkpViscousSurfaceModifierConstraintAtom)) { }

		hkpViscousSurfaceModifierConstraintAtom(hkFinishLoadedObjectFlag f) : hkpModifierConstraintAtom(f) {}

		static const hkUint16 ADDITIONAL_SCHEMA_SIZE = 0;

		int getConstraintInfo( hkpConstraintInfo& info ) const	{ return hkpConstraintAtom::CALLBACK_REQUEST_NONE; }

		HK_FORCE_INLINE int numSolverResults() const { return 0; }
};


	//	************************ Moving Surface **************************
	//	************************ Moving Surface **************************
	//	************************ Moving Surface **************************

struct hkpMovingSurfaceModifierConstraintAtom : public hkpModifierConstraintAtom
{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CONSTRAINT, hkpMovingSurfaceModifierConstraintAtom );
		HK_DECLARE_REFLECTION();

	public:

		hkpMovingSurfaceModifierConstraintAtom() : hkpModifierConstraintAtom(TYPE_MODIFIER_MOVING_SURFACE, sizeof(hkpMovingSurfaceModifierConstraintAtom)) { }
		hkpMovingSurfaceModifierConstraintAtom(hkFinishLoadedObjectFlag f) : hkpModifierConstraintAtom(f) {}

		HK_FORCE_INLINE int numSolverResults() const { return 0; }

		static const hkUint16 ADDITIONAL_SCHEMA_SIZE = 2 * HK_SIZE_OF_JACOBIAN_ADD_VELOCITY_SCHEMA + HK_SIZE_OF_JACOBIAN_HEADER_SCHEMA;

		int getConstraintInfo( hkpConstraintInfo& info ) const
		{
			info.m_sizeOfSchemas += ADDITIONAL_SCHEMA_SIZE;
			return hkpConstraintAtom::CALLBACK_REQUEST_NONE;
		}

		void collisionResponseBeginCallback( const hkContactPoint& cp, struct hkpSimpleConstraintInfoInitInput& inA, struct hkpBodyVelocity& velA, hkpSimpleConstraintInfoInitInput& inB, hkpBodyVelocity& velB);
		void collisionResponseEndCallback(   const hkContactPoint& cp, hkReal impulseApplied, struct hkpSimpleConstraintInfoInitInput& inA, struct hkpBodyVelocity& velA, hkpSimpleConstraintInfoInitInput& inB, hkpBodyVelocity& velB);

		hkVector4& getVelocity() { return m_velocity; }
		const hkVector4& getVelocity() const { return m_velocity; }

	public:

 		hkVector4 m_velocity;
};


	//	************************ Ignore Constraint **************************
	//	************************ Ignore Constraint **************************
	//	************************ Ignore Constraint **************************

struct hkpIgnoreModifierConstraintAtom : public hkpModifierConstraintAtom
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CONSTRAINT, hkpIgnoreModifierConstraintAtom );
	HK_DECLARE_REFLECTION();

public:

	hkpIgnoreModifierConstraintAtom() : hkpModifierConstraintAtom(TYPE_MODIFIER_IGNORE_CONSTRAINT, sizeof(hkpIgnoreModifierConstraintAtom)) { }
	hkpIgnoreModifierConstraintAtom(hkFinishLoadedObjectFlag f) : hkpModifierConstraintAtom(f) {}

	HK_FORCE_INLINE int numSolverResults() const { return 0; }

	int getConstraintInfo( hkpConstraintInfo& info ) const;
};





#endif // HK_DYNAMICS2_CONSTRAINT_ATOM_H

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
