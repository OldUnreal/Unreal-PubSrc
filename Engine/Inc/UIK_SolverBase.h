	UIK_SolverBase(){}
	
	void Init(struct FSkelMeshInstance* Owner);
	virtual void Reset() {}
	virtual void UpdateInstance() {}
	virtual void Tick(FLOAT DeltaTime);
	virtual void DrawPreview(struct FSceneNode* Frame){}
	virtual void Disable() {}
	void PostEditChange();

	class AActor* GetOwner() const;
	class USkeletalMesh* GetMesh() const;
