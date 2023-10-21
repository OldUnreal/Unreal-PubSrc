// Sunlight actor.
Class SunLight extends DistantLightActor
	native;

cpptext
{
	ASunLight() {}
	UBOOL ShouldSunlightLit( const INT iZoneNumber ) const;
}

#exec Texture Import File="Textures\SunIcon.pcx" Name="SunIcon" GROUP="Icons" Mips=Off Flags=2

var() name SunlightZoneTag; // If this is set, only zoneinfo's with matching Tag will be lit by this sunlight, otherwise current zone only (or entire level if is in SkyZoneInfo).

defaultproperties
{
	NewLightRadius=98304
	bDirectional=True
	LightEffect=LE_Sunlight
	DrawScale=2
	Texture=Texture'SunIcon'
	LightRadius=255
}