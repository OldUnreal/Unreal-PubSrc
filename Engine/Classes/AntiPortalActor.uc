//=============================================================================
// AntiPortalActor.
//=============================================================================
class AntiPortalActor extends Volume
	native
	nativereplication;

var() const bool bAntiPortalHidden;
var bool BACKUP_Hidden;

replication
{
	unreliable if ( Role==ROLE_Authority )
		bAntiPortalHidden;
}

native final function SetAntiPortalHidden( bool bHide );

function BeginPlay()
{
	BACKUP_Hidden = bAntiPortalHidden;
}

function Reset()
{
	SetAntiPortalHidden(BACKUP_Hidden);
}

//
//	TriggerControl
//
state() TriggerControl
{
	function Trigger(Actor Other,Pawn EventInstigator)
	{
		SetAntiPortalHidden(true);
		RemoteRole = ROLE_SimulatedProxy; // Enable replication channel.
	}
	function UnTrigger(Actor Other,Pawn EventInstigator)
	{
		SetAntiPortalHidden(false);
	}
}

//
//	TriggerToggle
//
state() TriggerToggle
{
	function Trigger(Actor Other,Pawn EventInstigator)
	{
		SetAntiPortalHidden(!bAntiPortalHidden);
		RemoteRole = ROLE_SimulatedProxy;
	}
}

defaultproperties
{
	bHidden=false
	bCollideActors=False
	BrushColor=(R=140,G=0,B=38)
	bAlwaysRelevant=true
	NetUpdateFrequency=0.5
}