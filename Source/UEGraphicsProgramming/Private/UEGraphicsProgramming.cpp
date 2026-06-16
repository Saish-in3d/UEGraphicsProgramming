// Copyright Epic Games, Inc. All Rights Reserved.

#include "UEGraphicsProgramming.h"

#include "EngineModule.h"
#include "Modules/ModuleInterface.h"
#include "Interfaces/IPluginManager.h"
#include "RendererInterface.h"


#define LOCTEXT_NAMESPACE "FUEGraphicsProgrammingModule"

void FUEGraphicsProgrammingModule::BoundRendererDelegate(FDelegateHandle& inHandle,FPostOpaqueRenderDelegate& inPostOpaqueRenderDelegate)
{

	
	inHandle = GetRendererModule().RegisterPostOpaqueRenderDelegate(inPostOpaqueRenderDelegate);
}

void FUEGraphicsProgrammingModule::UnboundRendererHandle(FDelegateHandle& inHandle,FPostOpaqueRenderDelegate& inPostOpaqueRenderDelegate)
{
	GetRendererModule().RemovePostOpaqueRenderDelegate(inHandle);
	inHandle.Reset();
}

void FUEGraphicsProgrammingModule::StartupModule()
{
	/// Set up the Shader Directories
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("UEGraphicsProgramming"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugins/UEGraphicsProgramming"), PluginShaderDir);
	
	//RendererModule = ;
	
	//BoundRendererDelegate();
	
	//UnboundRendererHandle();
	
}

void FUEGraphicsProgrammingModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUEGraphicsProgrammingModule, UEGraphicsProgramming)