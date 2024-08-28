/*=============================================================================
	UnEngineNative.h: Native function lookup table for static libraries.
	Copyright 2000 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Brandon Reinhart
=============================================================================*/

#ifndef UNENGINENATIVE_H
#define UNENGINENATIVE_H

DECLARE_NATIVE_TYPE(Engine,AActor);
DECLARE_NATIVE_TYPE(Engine,APawn);
DECLARE_NATIVE_TYPE(Engine,APlayerPawn);
DECLARE_NATIVE_TYPE(Engine,ADecal);
DECLARE_NATIVE_TYPE(Engine,ATimeDemo);
DECLARE_NATIVE_TYPE(Engine,AStatLog);
DECLARE_NATIVE_TYPE(Engine,AStatLogFile);
DECLARE_NATIVE_TYPE(Engine,AZoneInfo);
DECLARE_NATIVE_TYPE(Engine,AWarpZoneInfo);
DECLARE_NATIVE_TYPE(Engine,ALevelInfo);
DECLARE_NATIVE_TYPE(Engine,AGameInfo);
DECLARE_NATIVE_TYPE(Engine,ANavigationPoint);
DECLARE_NATIVE_TYPE(Engine,UCanvas);
DECLARE_NATIVE_TYPE(Engine,UConsole);
DECLARE_NATIVE_TYPE(Engine,UShadowBitMap);
DECLARE_NATIVE_TYPE(Engine,UIK_SolverBase);
DECLARE_NATIVE_TYPE(Engine,UIK_LipSync);
DECLARE_NATIVE_TYPE(Engine,UScriptedTexture);
DECLARE_NATIVE_TYPE(Engine,AProjector);
DECLARE_NATIVE_TYPE(Engine,UPX_PhysicsDataBase);
DECLARE_NATIVE_TYPE(Engine,UPX_VehicleBase);
DECLARE_NATIVE_TYPE(Engine,UPX_VehicleWheeled);
DECLARE_NATIVE_TYPE(Engine,UPXJ_BaseJoint);
DECLARE_NATIVE_TYPE(Engine,UPhysicsAnimation);
DECLARE_NATIVE_TYPE(Engine,URenderIterator);
DECLARE_NATIVE_TYPE(Engine,AVolume);
DECLARE_NATIVE_TYPE(Engine,AInventory);
DECLARE_NATIVE_TYPE(Engine,AProjectile);
DECLARE_NATIVE_TYPE(Engine,UEngine);
DECLARE_NATIVE_TYPE(Engine,UClientPreloginScene);
DECLARE_NATIVE_TYPE(Engine,UServerPreloginScene);
DECLARE_NATIVE_TYPE(Engine,UAnimationNotify);
#define AUTO_INITIALIZE_REGISTRANTS_ENGINE		\
	ULodMesh::StaticClass();					\
	UStaticMesh::StaticClass();					\
	AActor::StaticClass();						\
	ALight::StaticClass();						\
	AWeapon::StaticClass();						\
	ALevelInfo::StaticClass();					\
	AGameInfo::StaticClass();					\
	ACamera::StaticClass();						\
	AZoneInfo::StaticClass();					\
	ASkyZoneInfo::StaticClass();				\
	APathNode::StaticClass();					\
	ANavigationPoint::StaticClass();			\
	AScout::StaticClass();						\
	AInterpolationPoint::StaticClass();			\
	ADecoration::StaticClass();					\
	AProjectile::StaticClass();					\
	AWarpZoneInfo::StaticClass();				\
	ATeleporter::StaticClass();					\
	APlayerStart::StaticClass();				\
	AKeypoint::StaticClass();					\
	AInventory::StaticClass();					\
	AInventorySpot::StaticClass();				\
	ATriggers::StaticClass();					\
	ATrigger::StaticClass();					\
	ATriggerMarker::StaticClass();				\
	AButtonMarker::StaticClass();				\
	AWarpZoneMarker::StaticClass();				\
	AHUD::StaticClass();						\
	AMenu::StaticClass();						\
	ASavedMove::StaticClass();					\
	ACarcass::StaticClass();					\
	AInfo::StaticClass();						\
	AReplicationInfo::StaticClass();			\
	APlayerReplicationInfo::StaticClass();		\
	AInternetInfo::StaticClass();				\
	ATimeDemo::StaticClass();					\
	AStatLog::StaticClass();					\
	AStatLogFile::StaticClass();				\
	ULevelSummary::StaticClass();				\
	ADecal::StaticClass();						\
	USound::StaticClass();						\
	UMusic::StaticClass();						\
	UAudioSubsystem::StaticClass();				\
	UClient::StaticClass();						\
	UViewport::StaticClass();					\
	UCanvas::StaticClass();						\
	UConsole::StaticClass();					\
	UNetConnection::StaticClass();				\
	UDemoPlayPendingLevel::StaticClass();		\
	UDemoRecConnection::StaticClass();			\
	UDemoRecDriver::StaticClass();				\
	ABrush::StaticClass();						\
	UEngine::StaticClass();						\
	URenderBase::StaticClass();					\
	URenderDevice::StaticClass();				\
	URenderIterator::StaticClass();				\
	UServerCommandlet::StaticClass();			\
	UPolys::StaticClass();						\
	UFont::StaticClass();						\
	UGameEngine::StaticClass();					\
	UInput::StaticClass();						\
	ULevelBase::StaticClass();					\
	ULevel::StaticClass();						\
	UMesh::StaticClass();						\
	UBspNodes::StaticClass();					\
	UBspSurfs::StaticClass();					\
	UVectors::StaticClass();					\
	UVerts::StaticClass();						\
	UModel::StaticClass();						\
	AMover::StaticClass();						\
	UPackageMapLevel::StaticClass();			\
	UNetDriver::StaticClass();					\
	APawn::StaticClass();						\
	APlayerPawn::StaticClass();					\
	UPendingLevel::StaticClass();				\
	UNetPendingLevel::StaticClass();			\
	UPlayer::StaticClass();						\
	UPrimitive::StaticClass();					\
	UScriptedTexture::StaticClass();			\
	UBitmap::StaticClass();						\
	UTexture::StaticClass();					\
	UPalette::StaticClass();					\
	UAnimation::StaticClass();					\
	UDownload::StaticClass();					\
	UChannelDownload::StaticClass();			\
	UPhysicsEngine::StaticClass();				\
	UPX_PhysicsDataBase::StaticClass();			\
	UPX_RigidBodyData::StaticClass();			\
	UPX_Repulsor::StaticClass();				\
	UPXC_CollisionShape::StaticClass();			\
	UPXC_BoxCollision::StaticClass();			\
	UPXC_SphereCollision::StaticClass();		\
	UPXC_MeshCollision::StaticClass();			\
	UPXC_MultiCollision::StaticClass();			\
	UPXC_CapsuleCollision::StaticClass();		\
	UPXC_BrushCollision::StaticClass();			\
	UPXJ_BaseJoint::StaticClass();				\
	UPXJ_FixedJoint::StaticClass();				\
	UPXJ_HingeJoint::StaticClass();				\
	UPXJ_SocketJoint::StaticClass();			\
	UPXJ_ConstraintJoint::StaticClass();		\
	UPXJ_ShadowParm::StaticClass();				\
	AProjector::StaticClass();					\
	UProjectorPrimitive::StaticClass();			\
	ACollisionPlane::StaticClass();				\
	UPlaneBoundsPrim::StaticClass();			\
	AVolume::StaticClass();						\
	ABlockingVolume::StaticClass();				\
	ADynamicZoneInfo::StaticClass();			\
	UStaticLightData::StaticClass();			\
	UPX_SkeletalBody::StaticClass();			\
	UPX_RagdollData::StaticClass();				\
	UUnrealCursor::StaticClass();				\
	USkeletalMesh::StaticClass();				\
	UChannelDownload::StaticClass();			\
	UZoneCurtain::StaticClass();				\
	UShadowBitMap::StaticClass();				\
	UPlayerInteraction::StaticClass();			\
	UPhysicsAnimation::StaticClass();			\
	USecurityData::StaticClass();				\
	UFontColored::StaticClass();				\
	ASunlightCorona::StaticClass();				\
	ADynamicCorona::StaticClass();				\
	AClipMarker::StaticClass();					\
	AVisibilityNotify::StaticClass();			\
	AInventoryAttachment::StaticClass();		\
	AWeaponAttachment::StaticClass();			\
	AWeaponMuzzleFlash::StaticClass();			\
	AHUDOverlay::StaticClass();					\
	AGameReplicationInfo::StaticClass();		\
	ASpawnNotify::StaticClass();				\
	AFluidInfo::StaticClass();					\
	UPortalModifier::StaticClass();				\
	UAnimationNotify::StaticClass();			\
	UIK_SolverBase::StaticClass();				\
	UIK_HeadTurn::StaticClass();				\
	UIK_FootPlacement::StaticClass();			\
	UIK_Limb::StaticClass();					\
	UIK_LipSync::StaticClass();					\
	ABlockingActor::StaticClass();				\
	AStaticMeshActor::StaticClass();			\
	USHAUpdateCommandlet::StaticClass();		\
	ULinkerUpdateCommandlet::StaticClass();     \
	UClientPreloginScene::StaticClass();        \
	UServerPreloginScene::StaticClass();        \
	AZoneUnion::StaticClass();					\
	UTerrainInfoRender::StaticClass();			\
	UPX_VehicleBase::StaticClass();				\
	UPX_VehicleWheeled::StaticClass();			\
    /* UScriptViewport::StaticClass(); */

#endif
