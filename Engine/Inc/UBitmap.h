	// Constructor.
	UBitmap();
	
	// Static.
	static class UClient* __Client;

	// UObject interface.
	void onPackageChanged(UObject* OldPackage);

	// UBitmap interface.
	virtual FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev) { return NULL; }

	virtual INT GetNumMips() { return 0; }
	virtual FMipmapBase* GetMip(INT i) { return NULL; }
	