	UCanvas() {}
	
public:
	// UCanvas interface.
	virtual void Init( UViewport* InViewport );
	virtual void Update( FSceneNode* Frame, BYTE bNoReset=0 );
	virtual void DrawTile( UTexture* Texture, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, class FSpanBuffer* SpanBuffer, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags );
	virtual void DrawTile( FTextureInfo& Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, class FSpanBuffer* SpanBuffer, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags );
	virtual void DrawTileStretched( INT X1, INT X2, INT Y1, INT Y2, class UTexture* Tex );
	virtual void DrawIcon( UTexture* Texture, FLOAT ScreenX, FLOAT ScreenY, FLOAT XSize, FLOAT YSize, class FSpanBuffer* SpanBuffer, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags );
	virtual void DrawPattern( UTexture* Texture, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT Scale, FLOAT OrgX, FLOAT OrgY, class FSpanBuffer* SpanBuffer, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags );
	virtual void WrappedStrLenf( UFont* Font, INT& XL, INT& YL, const TCHAR* Fmt, ... );
	virtual void VARARGS WrappedPrintf( UFont* Font, UBOOL Center, const TCHAR* Fmt, ... );
	virtual void SetClip( INT X, INT Y, INT XL, INT YL );
	virtual void DrawTris(UTexture* Tex, FCanvasPoly* Polys, INT NumPolies, DWORD PolyFlags, UBOOL bNeedsScaling=0);
	void DrawRotatedTile(UTexture* Tex, FLOAT X, FLOAT Y, FLOAT Roll, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, const FPlane& Color, FLOAT PivotX = 0.5f, FLOAT PivotY = 0.5f, UBOOL bRotateUV = 0, DWORD PolyFlags = PF_TwoSided, UBOOL bNeedsScaling = 0);
	void PushCanvasScale(FLOAT Scale, UBOOL bAbsolute = 0);
	void PopCanvasScale();
	void eventReset(UBOOL bResetScale=0);
	
	void WrappedPrint(DWORD PolyFlags, INT& XL, INT& YL, UFont* Font, UBOOL Center, UBOOL bClip, const TCHAR* Text);