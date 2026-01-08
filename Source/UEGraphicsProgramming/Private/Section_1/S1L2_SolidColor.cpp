// Fill out your copyright notice in the Description page of Project Settings.


#include "Section_1/S1L2_SolidColor.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Engine/TextureRenderTarget2D.h"

IMPLEMENT_GLOBAL_SHADER(FS1L2_ClearColorCS, "/Plugins/UEGraphicsProgramming/FS1L2_ClearColorCS.usf",
						"FS1L2_ClearColorCS", SF_Compute);


void FS1L2_ClearColorCS::FS1L2_ClearColorShaderRun(UTextureRenderTarget2D* OutputRenderTarget)
{
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[OutputRenderTarget](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering render target in RDG 
			FRDGTextureRef FS1L1_OutputTextureRDGRef = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(OutputRenderTarget->GetRenderTargetResource()->GetTextureRHI(),
					TEXT("FS1L1_RenderTarget"))) ;
			
			//ShaderParameterSetup
			FS1L2_ClearColorCS::FParameters* FS1L2_Parameters = GraphBuilder.AllocParameters<
				FS1L2_ClearColorCS::FParameters>();
			
			FS1L2_Parameters->FS1L2_Output =  GraphBuilder.CreateUAV(FS1L1_OutputTextureRDGRef)  ; //Compute Shader directly writes on this RT
			
			//Dispatching shader
			//Initialising compute shader object
			TShaderMapRef<FS1L2_ClearColorCS> FS1L1_ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			//Setting up Group Count
			FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(OutputRenderTarget->SizeX,
				OutputRenderTarget->SizeY), FComputeShaderUtils::kGolden2DGroupSize);
			
			//Actually dispatching the shader
			
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS1L1"),
				FS1L1_ComputeShader,
				FS1L2_Parameters,
				GroupCount
				);
			
			GraphBuilder.Execute();
			
		}
	);
	
	
}
