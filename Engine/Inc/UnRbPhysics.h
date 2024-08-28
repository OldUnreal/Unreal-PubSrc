/*=============================================================================
	UnRbPhysics.cpp: RigidBody physics implementation.

	Revision history:
		* Created by Marco 11/2020
=============================================================================*/
#pragma once

struct PX_ShapeObject;
struct PX_PhysicsObject;
class UPX_RigidBodyData;
class UPXC_CollisionShape;
class UPX_PhysicsDataBase;
struct PX_MeshShape;
struct PX_JointBase;
struct PX_JointObject;
struct FActorRBPhysicsBase;
struct PX_TickableBase;
struct PX_ArticulationList;
struct PX_ArticulationLink;

struct ENGINE_API FRigidBodyProperties
{
	class AActor* Actor;
	FVector Scale3D, COMOffset, InertiaTensor;
	const FVector& LinearVelocity;
	const FVector& AngularVelocity;
	FLOAT MinImpactThreshold;
	FLOAT Mass;
	class UPXC_CollisionShape* CollisionShape;

	FRigidBodyProperties(AActor* A, const FVector& Scale, const FVector& COM, const FVector& LinVel, const FVector& AngVel, FLOAT ObjMass, UPXC_CollisionShape* Shape)
		: Actor(A), Scale3D(Scale), COMOffset(COM), InertiaTensor(1.f, 1.f, 1.f), LinearVelocity(LinVel), AngularVelocity(AngVel), MinImpactThreshold(1.f), Mass(ObjMass), CollisionShape(Shape)
	{}
	FRigidBodyProperties(class UPX_RigidBodyData* Data);
};
struct FArticulationProperties : public FRigidBodyProperties
{
	PX_ArticulationLink* ParentLink; // Parent link on the articulation body (should be null for root).

	FArticulationProperties(AActor* A, const FVector& Scale, const FVector& COM, const FVector& LinVel, const FVector& AngVel, FLOAT ObjMass, UPXC_CollisionShape* Shape)
		: FRigidBodyProperties(A, Scale, COM, LinVel, AngVel, ObjMass, Shape), ParentLink(nullptr)
	{}
	FArticulationProperties(class UPX_RigidBodyData* Data)
		: FRigidBodyProperties(Data), ParentLink(nullptr)
	{}
};

// Shapes are automatically destroyed along with PX_PhysicsObject.
// Deleting a shape should automatically detach it from PX_PhysicsObject too!
struct ENGINE_API PX_ShapeObject
{
	friend struct PX_PhysicsObject;

	INT PublicTag; // Custom tag that can be set to this shape, for Engine's internal purposes.
protected:
	UBOOL bNotifyCallback, bIsContact;
	FVector ContactLocation, ContactNormal, ContactVelocity;

	FLOAT StaticFriction, DynamicFriction, Restitution;
	PX_PhysicsObject* Owner;
	PX_ShapeObject* Next;

	PX_ShapeObject(UBOOL bContactNotify, FLOAT Frict, FLOAT Rest, PX_PhysicsObject* S);
public:
	UBOOL bContactNotifyActor; // Contact notification should happen to actor.

	virtual ~PX_ShapeObject() noexcept(false);

	inline PX_ShapeObject* GetNext() const
	{
		return Next;
	}

	inline UBOOL GetNotifyContact() const // Shape is currently notified of contact with world.
	{
		return bNotifyCallback;
	}
	inline UBOOL GetIsContact() const // Is currently in contact with world (only if bNotifyCallback).
	{
		return bIsContact;
	}
	inline FVector GetContactLocation() const
	{
		return ContactLocation;
	}
	inline FVector GetContactNormal() const
	{
		return ContactNormal;
	}
	inline FVector GetContactVelocity() const // Obtain user override contact velocity (useful for vehicle wheels).
	{
		return ContactVelocity;
	}

	inline FLOAT GetStaticFriction() const
	{
		return StaticFriction;
	}
	inline FLOAT GetDynamicFriction() const
	{
		return DynamicFriction;
	}
	inline FLOAT GetRestitution() const
	{
		return Restitution;
	}
	inline PX_PhysicsObject* GetOwner() const
	{
		return Owner;
	}
	virtual FCoords GetLocalPose() _VF_BASE_RET(GMath.UnitCoords);

	virtual void SetContactNotify(UBOOL bEnable) _VF_BASE;
	virtual void SetFriction(FLOAT NewStatic, FLOAT NewDynmaic, FLOAT NewRestitution) _VF_BASE;
	virtual void SetLocalPose(const FCoords& NewCoords) _VF_BASE;
	virtual void SetContactVelocity(const FVector& NewCVelocity) _VF_BASE;
	virtual void DrawDebug(const FPlane& ShapeColor) _VF_BASE;

	PX_ShapeObject() = delete;
};

struct ENGINE_API PX_SceneBase
{
	friend class FSceneObjectIterator;
	friend struct PX_TickableBase;

protected:
	ULevel* Level;
	PX_TickableBase* TickablesList;
	UBOOL bPendingDelete;

public:
	PX_SceneBase* NextTick; // Next scene to be ticked.

	PX_SceneBase(ULevel* L)
		: Level(L), TickablesList(nullptr), bPendingDelete(FALSE), NextTick(nullptr)
	{}
	virtual ~PX_SceneBase() noexcept(false);

	// Pre-physics tick.
	virtual void TickScene(FLOAT DeltaTime);

	inline ULevel* GetLevel() const
	{
		return Level;
	}
	inline UBOOL IsPendingDelete() const
	{
		return bPendingDelete;
	}

	virtual PX_PhysicsObject* CreatePlatform(AActor* Owner, const FVector& Pos, const FRotator& Rot) _VF_BASE_RET(NULL);
	virtual PX_PhysicsObject* CreateRigidBody(const FRigidBodyProperties& Parms, const FVector& Pos, const FRotator& Rot) _VF_BASE_RET(NULL);
	virtual PX_PhysicsObject* CreateStaticBody(AActor* Owner, const FVector& Pos, const FRotator& Rot) _VF_BASE_RET(NULL);
	virtual PX_ArticulationList* CreateArticulation(AActor* Owner, INT NumIterations) _VF_BASE_RET(NULL);

	virtual void SendToScene(PX_PhysicsObject* Obj, PX_SceneBase* NewScene);

	virtual void ShutDownScene() {} // Notify the scene its about to be deleted!
private:
	PX_SceneBase() {}

	void AddTickable(PX_TickableBase* T);
	void RemoveTickable(PX_TickableBase* T);
};

struct ENGINE_API PX_TickableBase
{
	friend struct PX_SceneBase;

private:
	PX_SceneBase* CurrentScene;
	PX_TickableBase* NextTickable;
	UBOOL bRequestsTick;

	PX_TickableBase() {}

public:
	PX_TickableBase(PX_SceneBase* S, UBOOL bNeedsTick=TRUE);
	virtual ~PX_TickableBase() noexcept(false);

	inline PX_SceneBase* GetScene() const
	{
		return CurrentScene;
	}

	virtual void Tick(FLOAT DeltaTime) {}

	inline UBOOL NeedsTick() const // Does this object need tick notification.
	{
		return bRequestsTick;
	}

	void SetNeedsTick(UBOOL bEnable); // Enable/Disable ticking.
	virtual void SetScene(PX_SceneBase* S); // Change scene.
};

struct ENGINE_API PX_PhysicsObject
{
	friend struct PX_SceneBase;
	friend struct PX_JointObject;
	friend struct PX_ShapeObject;

	enum ERBObjectType : DWORD
	{
		RBTYPE_Undefined, // ERROR!
		RBTYPE_Platform, // This is a mover.
		RBTYPE_RigidBody, // This is a regular rigidbody object.
		RBTYPE_StaticBody, // This is an immobile object (like level decos).
		RBTYPE_Articulation, // This is a multibody rigidbody object.
		RBTYPE_ArticulationLink, // This is an articulation link.
		RBTYPE_MAX,
	};
private:
	PX_JointObject* JointList; // Linked list of joints where this rbactor is the owner of it.
	PX_ShapeObject* ShapeList; // Linked list of shapes attached to this rbactor!

protected:
	class AActor* Actor;
	PX_SceneBase* Scene;
	void* rbActor;
	FVector rbGravity;

public:
	PX_PhysicsObject(AActor* A, PX_SceneBase* S)
		: JointList(nullptr), ShapeList(nullptr), Actor(A), Scene(S), rbActor(nullptr), rbGravity(0.f, 0.f, 0.f)
	{}
	virtual ~PX_PhysicsObject() noexcept(false);

	inline AActor* GetActor() const
	{
		return Actor;
	}
	inline PX_SceneBase* GetScene() const
	{
		return Scene;
	}
	inline PX_JointObject* GetJoints() const
	{
		return JointList;
	}
	inline PX_ShapeObject* GetShapes() const
	{
		return ShapeList;
	}
	inline void* GetRbActor() const
	{
		return rbActor;
	}
	virtual DWORD GetType() const _VF_BASE_RET(RBTYPE_Undefined);

	// Set/Get values.
	virtual FVector GetLinearVelocity() { return FVector(0, 0, 0); }
	virtual void SetLinearVelocity(const FVector& NewVel) {}
	virtual FVector GetAngularVelocity() { return FVector(0, 0, 0); }
	virtual void SetAngularVelocity(const FVector& NewVel) {}
	virtual void GetPosition(FVector* Pos, FRotator* Rot) {}
	virtual void SetPosition(const FVector* NewPos, const FRotator* NewRot) {}
	virtual FQuat GetRotation() { return FQuat(1.f, 0.f, 0.f, 0.f); }
	virtual UBOOL IsSleeping() { return TRUE; }
	virtual void WakeUp() {}
	virtual void PutToSleep() {}
	virtual void Impulse(const FVector& Force, const FVector* Pos = NULL, UBOOL bCheckMass = TRUE) {}
	virtual void AddForceAtPos(const FVector& Force, const FVector& Pos) {}
	virtual void AddForce(const FVector& Force) {}
	virtual void AddTorque(const FVector& Torque) {}
	virtual void SetConstantForce(const FVector& Force) {}
	virtual void SetConstantTorque(const FVector& Torque) {}
	virtual FVector GetConstantForce() const { return FVector(0.f, 0.f, 0.f); }
	virtual FVector GetConstantTorque() const  { return FVector(0.f, 0.f, 0.f); }
	virtual void SetGravity(const FVector& NewGrav) {}
	inline FVector GetGravity() const
	{
		return rbGravity;
	}
	virtual void SetConstVelocity(const FVector& NewVel) {}
	virtual void SetMass(FLOAT NewMass) {}
	virtual void SetLimits(FLOAT MaxAngVel, FLOAT MaxLinVel) {}
	virtual void SetDampening(FLOAT AngVelDamp, FLOAT LinVelDamp) {}
	virtual void SetStayUpright(UBOOL bEnable) {}

	// Change scene
	virtual void ChangeScene(PX_SceneBase* NewScene);

	// Query for callback events (return TRUE if actor got destroyed and should halt process now).
	virtual UBOOL ProcessCallbacks(FActorRBPhysicsBase* Obj) { return FALSE; }

	// Collision grouping
	virtual void DisableCollision(DWORD ThisID, DWORD ColFlags) {}
	virtual void SetCollisionFlags(DWORD Group, DWORD Flags) {}

	// Debug
	virtual void DrawDebug() {}
private:
	PX_PhysicsObject() {}
};

struct ENGINE_API PX_ArticulationList : public PX_PhysicsObject
{
	friend struct PX_ArticulationLink;

private:
	PX_ArticulationLink* ArtList;

protected:
	PX_ArticulationList(AActor* A, PX_SceneBase* S);

public:
	~PX_ArticulationList() noexcept(false);

	inline PX_ArticulationLink* GetList() const
	{
		return ArtList;
	}

	virtual PX_ArticulationLink* CreateArticulationLink(const FArticulationProperties& Parms, const FVector& Pos, const FRotator& Rot) _VF_BASE_RET(NULL);
	virtual void FinishArticulation() _VF_BASE; // Finished initializing the link and is now ready to be added to the scene.
};
struct ENGINE_API PX_ArticulationLink : public PX_PhysicsObject
{
	friend struct PX_ArticulationList;

private:
	PX_ArticulationList* ArtOwner;
	PX_ArticulationLink* ArtNext;
	PX_ArticulationLink* LinkParent;

protected:
	PX_ArticulationLink(PX_ArticulationList* Art, PX_ArticulationLink* ArtParent, AActor* A, PX_SceneBase* S);

public:
	~PX_ArticulationLink() noexcept(false);

	inline PX_ArticulationList* GetArtOwner() const
	{
		return ArtOwner;
	}
	inline PX_ArticulationLink* GetListNext() const
	{
		return ArtNext;
	}
	inline PX_ArticulationLink* GetArtParent() const
	{
		return LinkParent;
	}

	// Joints...
	virtual void SetJointCoords(const FCoords& A, const FCoords& B) {}
};

struct ENGINE_API PX_MeshShape
{
	enum ERBMeshType : DWORD
	{
		RBSHAPE_Tris,
		RBSHAPE_Convex,
		RBSHAPE_MultiConvex,
	};
private:
	ERBMeshType Type;

public:
	PX_MeshShape(ERBMeshType T)
		: Type(T)
	{}
	virtual ~PX_MeshShape() {}

	inline ERBMeshType GetType() const
	{
		return Type;
	}

private:
	PX_MeshShape() {}
};

struct ENGINE_API FActorRBPhysicsBase : public PX_TickableBase
{
	friend class FSceneObjectIterator;
	friend struct PX_SceneBase;

	enum EPhysicsObjType : DWORD
	{
		OBJ_Unknown,
		OBJ_RigidBody,
		OBJ_Articulation,
		OBJ_Static,
		OBJ_Dynamic,
		OBJ_Hollow,
	};

protected:
	class AActor* Owner;

public:
	FActorRBPhysicsBase(AActor* A, PX_SceneBase* Scene, UBOOL bStatic = FALSE);
	virtual ~FActorRBPhysicsBase();

	inline AActor* GetActor() const
	{
		return Owner;
	}

	virtual void ActorLevelChange(class ULevel* OldLevel, class ULevel* NewLevel) {}
	virtual void ActorMoved() {}
	virtual void PostInitJoints() {}
	virtual void OnZoneChange(class AZoneInfo* NewZone);
	virtual UBOOL RanInto(class AActor* Other) { return FALSE; }
	virtual UBOOL HandleNetworkMove(FVector* NewPos, FRotator* NewRot) { return FALSE; }
	virtual void NetworkVelocity(const FVector& NewVel) {}
	virtual PX_PhysicsObject* GetRbObject() const { return nullptr; }
	virtual class UPX_PhysicsDataBase* GetPxData() const { return nullptr; }
	virtual void PhysicsImpact(PX_PhysicsObject* ThisBody, AActor* OtherBody, FLOAT Force, const FVector& HitLocation, const FVector& HitNormal) {}
	virtual DWORD GetType() const
	{
		return OBJ_Unknown;
	}

	// Rigidbody controls.
	virtual void WakeUp() {}
	virtual void PutToSleep() {}
	virtual UBOOL IsASleep() { return TRUE; }
	virtual void SetAngularVelocity(const FVector& NewVel) {}
	virtual FVector GetAngularVelocity() { return FVector(0, 0, 0); }
	virtual void SetGravity(const FVector& NewGravity) {}
	virtual void SetConstVelocity(const FVector& NewVel) {}
	virtual void SetMass(FLOAT NewMass) {}

	// Set collision groups.
	virtual void SetCollisionFlags(DWORD ColGroups, DWORD ColFlag);

	// Debug
	virtual void DrawDebug() {}
};

struct FShapeProperties
{
	PX_PhysicsObject* RbObject;
	FLOAT Restitution;
	FLOAT Friction;
	const FCoords* Coords;
	UBOOL bUseDefault, bContactReport, bModifyContact;

	FShapeProperties() = delete;

	FShapeProperties(PX_PhysicsObject* O)
		: RbObject(O), Coords(NULL), bUseDefault(TRUE), bContactReport(FALSE), bModifyContact(FALSE)
	{}
	FShapeProperties(PX_PhysicsObject* O, class AZoneInfo* Zone);
	FShapeProperties(PX_PhysicsObject* O, const FCoords& C)
		: RbObject(O), Coords(&C), bUseDefault(TRUE), bContactReport(FALSE), bModifyContact(FALSE)
	{}
	FShapeProperties(PX_PhysicsObject* O, FLOAT Rest, FLOAT Frict)
		: RbObject(O), Restitution(Rest), Friction(Frict), Coords(NULL), bUseDefault(FALSE), bContactReport(FALSE), bModifyContact(FALSE)
	{}
	FShapeProperties(PX_PhysicsObject* O, FLOAT Rest, FLOAT Frict, const FCoords& C, UBOOL bContactRep = FALSE, UBOOL bModContact = FALSE)
		: RbObject(O), Restitution(Rest), Friction(Frict), Coords(&C), bUseDefault(FALSE), bContactReport(bContactRep), bModifyContact(bModContact)
	{}
};

struct ENGINE_API PX_JointBase
{
	PX_JointBase() = delete;
protected:
	PX_JointObject* Owner;

public:
	PX_JointBase(PX_JointObject* Obj);
	virtual ~PX_JointBase() noexcept(false);
	virtual void UpdateCoords(const FCoords& A, const FCoords& B) {}
	virtual void OnChangeScene() {}

	PX_JointObject* GetOwnerObject() const;
	class AActor* GetActorOwner() const;
	PX_PhysicsObject* GetRbOwner() const;
	class UPXJ_BaseJoint* GetJointOwner() const;
};

struct ENGINE_API PX_JointObject
{
	friend struct PX_JointBase;
	friend struct PX_PhysicsObject;

private:
	PX_JointObject* NextJoint; // Next on ActorA linked list for joints.

protected:
	PX_PhysicsObject* ActorA;
	PX_PhysicsObject* ActorB;
	AActor* OtherSideActor;
	class UPXJ_BaseJoint* Joint;
	PX_JointBase* PhysXJoint;

public:

	PX_JointObject(PX_PhysicsObject* A, PX_PhysicsObject* B, UPXJ_BaseJoint* J);
	virtual ~PX_JointObject() noexcept(false);

	inline PX_PhysicsObject* GetActorA() const
	{
		return ActorA;
	}
	inline PX_PhysicsObject* GetActorB() const
	{
		return ActorB;
	}
	inline UPXJ_BaseJoint* GetJointOwner() const
	{
		return Joint;
	}
	inline AActor* GetOtherSideActor() const
	{
		return OtherSideActor;
	}
	inline PX_JointObject* GetNextJoint() const
	{
		return NextJoint;
	}
	virtual UBOOL IsContactJoint() const
	{
		return FALSE;
	}

	virtual void UpdateCoords(const FCoords& A, const FCoords& B)
	{
		if (PhysXJoint)
			PhysXJoint->UpdateCoords(A, B);
	}

	// Called when ActorA scene was changed.
	virtual void OnChangeScene()
	{
		if (PhysXJoint)
			PhysXJoint->OnChangeScene();
	}

	// Callbacks from other side actor.
	virtual void OtherSideDestroyed();
	virtual void OtherSidePhysics(UBOOL bExit);

	// Owner actor was destroyed.
	virtual void OnActorDestroyed();

	// WARNING: This code may not be executed on main thread!
	virtual void OnJointBreak()
	{}
};

inline PX_JointObject* PX_JointBase::GetOwnerObject() const
{
	return Owner;
}
inline AActor* PX_JointBase::GetActorOwner() const
{
	return Owner->GetActorA() ? Owner->GetActorA()->GetActor() : nullptr;
}
inline PX_PhysicsObject* PX_JointBase::GetRbOwner() const
{
	return Owner->GetActorA();
}
inline UPXJ_BaseJoint* PX_JointBase::GetJointOwner() const
{
	return Owner->GetJointOwner();
}

struct ENGINE_API PX_ContactConstraint : public PX_JointObject
{
protected:
	UBOOL WheelMode;

	PX_ContactConstraint(PX_PhysicsObject* inOwner);
public:
	inline UBOOL IsWheelMode() const
	{
		return WheelMode;
	}
	UBOOL IsContactJoint() const
	{
		return TRUE;
	}

	virtual void DisableContact() = 0;
	virtual void SetContactPos(const FVector& Pos) = 0;
	virtual void SetContactNormal(const FVector& Norm) = 0;
	virtual void SetPenetration(FLOAT newPenetration) = 0;
	virtual void SetContact(const FVector& Pos, const FVector& Norm, FLOAT Penetration) = 0;
	virtual void SetWheelMode(UBOOL bEnable) = 0;
	virtual void SetWheelParms(const FVector& Dir, FLOAT ContactVel, FLOAT LatSlip, FLOAT LongSlip) = 0;
};

struct FJointBaseProps
{
	PX_JointObject* Owner;
	FLOAT MaxForce, MaxTorque;
	const FCoords* AxisA, * AxisB;
	PX_PhysicsObject* ArticularOwner;

	FJointBaseProps(PX_JointObject* O, FLOAT MaxF, FLOAT MaxT, const FCoords* A, const FCoords* B)
		: Owner(O), MaxForce(MaxF), MaxTorque(MaxT), AxisA(A), AxisB(B), ArticularOwner(nullptr)
	{}
};

#define DECLARE_JOINT_PARMS(classname, parentclass) \
	classname(PX_JointObject* O, FLOAT MaxF, FLOAT MaxT, const FCoords* A, const FCoords* B) \
	: parentclass(O, MaxF, MaxT, A, B) \
	{}

struct FJointHingeProps : public FJointBaseProps
{
	FLOAT LimitStiffness, LimitDamping, LimitRangeLow, LimitRangeHigh, MotorSpeed, MotorMaxSpeed;
	BITFIELD bLimitMovement : 1;
	BITFIELD bHardLimit : 1;
	BITFIELD bMotor : 1;
	BITFIELD bMotorFreeDrive : 1;

	DECLARE_JOINT_PARMS(FJointHingeProps, FJointBaseProps);
};

struct FJointSocketProps : public FJointBaseProps
{
	FVector2D MovementLimit;
	BITFIELD bLimitMovement : 1;

	DECLARE_JOINT_PARMS(FJointSocketProps, FJointBaseProps);
};

struct FJointConstProps : public FJointBaseProps
{
	struct FCC_LimitType* XAxis, * YAxis, * ZAxis;
	FCC_LimitType* ConeLimit;
	FCC_LimitType* TwistLimit;
	struct FCC_DistanceType* DistanceLimit;

	// How strong handle is.
	FLOAT LinearDamping, LinearStiffness;
	FVector* LinearStiffnessScale3D, * LinearDampingScale3D;
	FLOAT AngularDamping, AngularStiffness;

	DECLARE_JOINT_PARMS(FJointConstProps, FJointBaseProps);
};
#undef DECLARE_JOINT_PARMS
