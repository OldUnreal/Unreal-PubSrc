// Helper to make skeletal model head focus at a position.
Class IK_HeadTurn extends IK_SolverBase
	native;

var(IK) vector ViewPosition; // World location to face at.
var(IK) name HeadBone; // The head bone in question.
var(IK) name TopSpineBone; // Optional: Spine bone to rotate along with head.
var(IK) byte NumSpineBones; // Optional: Number of spine bones to create a smoother rotation.
var(IK) int MaxAngYaw[2],MaxAngPitch[2]; // Limited view angles of head.
var(IK) int MaxSpineYaw[2],MaxSpinePitch[2]; // Limited angle of spine rotation.
var(IK) float RotationRate; // Rotation speed of head.
var(IK) rotator HeadBoneDir; // Enable bPreviewAxis and try to get Green arrow to face forward and blue arrow upwards from head.
var(IK) ERotationAxis RotationAxis; // Rotation axis to apply to head bone.
var(IK) ERotationAxis SpineRotationAxis; // Rotation axis to apply to spine bone.
var(IK) bool bOutofRangeForward; // Make the head face forward when view target is out of range (behind).
var(IK) bool bFlipYaw,bFlipPitch; // Flip rotation axis if it's inverted.
var(IK) bool bFlipSpineYaw,bFlipSpinePitch; // Flip rotation axis if it's inverted.

var transient int iHeadBone,iSpineBone;
var transient float RotationAlpha,OldAlpha;
var transient rotator CurrentTurn;

defaultproperties
{
	MaxAngYaw(0)=-16384
	MaxAngYaw(1)=16384
	MaxAngPitch(0)=-8000
	MaxAngPitch(1)=8000
	MaxSpineYaw(0)=-6000
	MaxSpineYaw(1)=6000
	MaxSpinePitch(0)=-4000
	MaxSpinePitch(1)=4000
	ViewPosition=(X=800)
	bFlipYaw=true
	bFlipSpineYaw=true
	RotationAxis=ROT_RollYawPitch
	SpineRotationAxis=ROT_RollYawPitch
	RotationRate=32000
	NumSpineBones=1
}