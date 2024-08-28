// Collision Plane Primitive base.
// Written by .:..:
#pragma once

class ENGINE_API UPlaneBoundsPrim : public UPrimitive
{
	DECLARE_CLASS(UPlaneBoundsPrim,UPrimitive,CLASS_Transient,Engine)

	UPlaneBoundsPrim() {}

	// UPrimitive collision interface.
	UBOOL PointCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		FVector			Location,
		FVector			Extent,
		DWORD           ExtraNodeFlags
	);
	UBOOL LineCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		FVector			End,
		FVector			Start,
		FVector			Extent,
		DWORD           ExtraNodeFlags
	);
	FBox GetCollisionBoundingBox( const AActor* Owner );
	void Destroy();
};
