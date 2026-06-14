// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FUEGraphicsProgrammingModule : public IModuleInterface
{
public:
	void BoundRendererDelegate();
	void UnboundRendererHandle();
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	FPostOpaqueRenderDelegate D;
	
	//IRendererModule& RendererModule;
	
	FDelegateHandle Handle;
};
