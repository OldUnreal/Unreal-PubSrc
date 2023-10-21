//=============================================================================
// UPakExplosion2.
//=============================================================================
class UPakExplosion2 expands SpriteBallExplosion;

function MakeSound();

simulated function PostBeginPlay()
{
	PlaySound( sound'Explo5', Slot_Misc, 250, true, 10000 );
}

defaultproperties
{
     SpriteAnim(0)=Texture'e8_a00'
     SpriteAnim(1)=Texture'e8_a01'
     SpriteAnim(2)=Texture'e8_a02'
     SpriteAnim(3)=Texture'e8_a03'
     SpriteAnim(4)=Texture'e8_a04'
     SpriteAnim(5)=Texture'e8_a05'
     SpriteAnim(6)=Texture'e8_a06'
     SpriteAnim(7)=Texture'e8_a07'
     NumFrames=16
     EffectSound1=Sound'Explo5'
     LifeSpan=0.900000
     DrawScale=1.000000
}
