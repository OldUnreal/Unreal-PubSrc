// Convert a modulated texture into a texture with alpha masking instead.
Class ModulateToAlpha extends Texture
	native;

var() Texture SourceTexture;
var private transient Texture OldTexture;
var private transient Palette OldPalette;
var private transient bool bSourceValid;

defaultproperties
{
	bParametric=true
	bFractical=true
	MaxInitResolution=-1
}