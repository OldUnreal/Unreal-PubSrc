	UTexModifier(){}
	FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev);
	void Clear( DWORD ClearFlags )
	{
		Material = NULL;
	}
	void Init( INT InUSize, INT InVSize );
	void Tick( FLOAT DeltaSeconds ) {}
	void ConstantTimeTick() {}
	void Update() {}
	void Prime() {}
	void InitToMaterial();
	void UpdateFromPtrList();
	virtual void UpdatePtrList() {}
	virtual void GetCoords(FCoords2D& C) {}