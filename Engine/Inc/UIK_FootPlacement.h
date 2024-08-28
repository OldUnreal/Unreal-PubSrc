	UIK_FootPlacement() {}

	void Reset();
	void UpdateInstance();
	void Tick(FLOAT DeltaTime);
	void DrawPreview(struct FSceneNode* Frame);
	void Disable();
	void Destroy();
	UBOOL TraceFoot(FLOAT DeltaTime);