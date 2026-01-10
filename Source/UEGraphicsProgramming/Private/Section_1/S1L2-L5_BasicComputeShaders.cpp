// Fill out your copyright notice in the Description page of Project Settings.


#include "Section_1/S1L2-L5_BasicComputeShaders.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Engine/TextureRenderTarget2D.h"

IMPLEMENT_GLOBAL_SHADER(FS1L2_ClearColorCS, "/Plugins/UEGraphicsProgramming/FS1L2_ClearColorCS.usf",
						"FS1L2_ClearColorCS", SF_Compute);



void FS1L2_ClearColorCS::FS1L2_ClearColorCSRun(UTextureRenderTarget2D* OutputRenderTarget)
{
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[OutputRenderTarget](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering render target in RDG 
			FRDGTextureRef FS1L2_OutputTextureRDGRef = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(OutputRenderTarget->GetRenderTargetResource()->GetTextureRHI(),
					TEXT("FS1L1_RenderTarget"))) ;
			
			//ShaderParameterSetup
			FS1L2_ClearColorCS::FParameters* FS1L2_Parameters = GraphBuilder.AllocParameters<
				FS1L2_ClearColorCS::FParameters>();
			
			FS1L2_Parameters->FS1L2_Output =  GraphBuilder.CreateUAV(FS1L2_OutputTextureRDGRef)  ; //Compute Shader directly writes on this RT
			
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


IMPLEMENT_GLOBAL_SHADER(FS1L3_SplitScreenCS, "/Plugins/UEGraphicsProgramming/FS1L3_SplitScreenCS.usf",
						"FS1L3_SplitScreenCS", SF_Compute);

void FS1L3_SplitScreenCS::FS1L3_SplitScreenCSRun(UTextureRenderTarget2D* OutputRenderTarget)
{
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[OutputRenderTarget](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering render target in RDG 
			FRDGTextureRef FS1L3_OutputTextureRDGRef = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(OutputRenderTarget->GetRenderTargetResource()->GetTextureRHI(),
					TEXT("FS1L3_RenderTarget"))) ;
			
			//ShaderParameterSetup
			FS1L3_SplitScreenCS::FParameters* FS1L3_Parameters = GraphBuilder.AllocParameters<
				FS1L3_SplitScreenCS::FParameters>();
			
			FS1L3_Parameters->FS1L3_Output =  GraphBuilder.CreateUAV(FS1L3_OutputTextureRDGRef)  ; //Compute Shader directly writes on this RT
			FS1L3_Parameters->TextureResolution = OutputRenderTarget->SizeX; //Assuming that RT is a squaree
			//Dispatching shader
			//Initialising compute shader object
			TShaderMapRef<FS1L3_SplitScreenCS> FS1L3_ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			//Setting up Group Count
			FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(OutputRenderTarget->SizeX,
				OutputRenderTarget->SizeY), FComputeShaderUtils::kGolden2DGroupSize);
			
			//Actually dispatching the shader
			
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS1L3"),
				FS1L3_ComputeShader,
				FS1L3_Parameters,
				GroupCount
				);
			
			GraphBuilder.Execute();
			
		}
	);
	
	
	
	
}
