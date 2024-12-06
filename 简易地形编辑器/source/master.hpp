class MGMaster: public IAppBase{
	_secr CCamera Camera;
	_secr CMesh3D *pModel;
	_secr CAnimAssy Anim;
	_secr CSkeleton Bones;

	_open ~MGMaster(){
		$m.Finalize();
	}
	_open MGMaster(){
		$m.Initialize();
	}
	//////
	_open $VOID Initialize(){
		//资源

		$m.pModel = mgGetModel(0, "Knight")->lpFirst;

		//骨骼动画

		$m.Anim.Initialize(L".\\model\\actor\\Knight.dat");
		$m.Bones.Initialize(GE3d::lpD3dDevice, L".\\model\\actor\\Knight.dat");

		//相机

		$m.Camera.Reset(MVECTOR{ 0.f, 0.f, -10.f, 0.f });
		$m.Camera.Perspective(TO_RAD(gMyFovAngX), gMyWndW / gMyWndH, 0.1f, 1000.f);

		//屏幕裁剪

		gMyWndEx.rcScissor.left = 0;
	}
	_open $VOID Finalize(){
		$m.Anim.Finalize();
		$m.Bones.Finalize();
	}
	_open $VOID Paint(){
		gMyGfxCmd.BeginFrame();
		gMyRender.SetFrame(gMyGfxCmd.CurFrame);
		gMyRender.SetCamera($m.Camera);
		gMyRender.SetPipelineState(gMyArrPso[ID_PSO_Animal]);
		gMyRender.SetResourceViewHeap(gMyMtlLib.lpTexSrvs);
		gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());
		gMyRender.SetModelAttrs(Mat_Identity());
		gMyRender.SetBones($m.Bones.GetPosture());
		gMyRender.BeginDraw(gMyWndEx);
		gMyRender.ClearWindow(gMyWndEx, *(COLOR128*)dx::g_XMOne.f);
		gMyRender.DrawModels($m.pModel, &gMyMtlLib);
		gMyRender.EndDraw(gMyWndEx);
		gMyGfxCmd.EndFrame();

		GE2d::BeginDraw(gMyWndEx.pD3d11Buff, gMyWndEx.pD2dTarget);
		gMyGuiRoot->OnMessage(WM_PAINT, 0, 0);
		GE2d::EndDraw(gMyWndEx.pD3d11Buff);

		gMyWndEx.PresentBuffer();
	}
	_open $VOID Update(){
		UpdateBones();
		UpdateView();
	}
	_secr $VOID UpdateView(){
		if(gKeysTest[DIK_W]) $m.Camera.Fly(0.1f);
		if(gKeysTest[DIK_S]) $m.Camera.Fly(-0.1f);
		if(gKeysTest[DIK_D]) $m.Camera.Strafe(0.1f);
		if(gKeysTest[DIK_A]) $m.Camera.Strafe(-0.1f);

		if(GEInp::MouseState.lX > 0) $m.Camera.HorizontalYaw(0.01f);
		else if(GEInp::MouseState.lX < 0) $m.Camera.HorizontalYaw(-0.01f);
		if(GEInp::MouseState.lY > 0) $m.Camera.Pitch(0.01f);
		else if(GEInp::MouseState.lY < 0) $m.Camera.Pitch(-0.01f);
		if(GEInp::MouseState.lZ > 0) $m.Camera.Walk(2.f);
		else if(GEInp::MouseState.lZ < 0) $m.Camera.Walk(-2.f);

		$m.Camera.UpdateView(B_True);
	}
	_secr $VOID UpdateBones(){
		static SPFPN tTime = 0.f;
		SPFPN Period = $m.Anim.GetDuration();

		tTime += 0.002f;
		if(tTime > Period) tTime = 0.f;

		$m.Anim.ApplyToBones(tTime, $m.Bones.GetBones());
		$m.Bones.Update();
	}
	_open IBOOL OnMessage(DWORD, WPARAM, LPARAM){
		return B_False;
	}
};