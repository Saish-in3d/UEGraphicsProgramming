// Fill out your copyright notice in the Description page of Project Settings.


#include "S2_ExternalDataBasicShaders.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Engine/TextureRenderTarget2D.h"


IMPLEMENT_GLOBAL_SHADER(FS2L1_SingleThreadRingCS, "/Plugins/UEGraphicsProgramming/FS2L1_SingleThreadRingCS.usf",
						"FS2L1_SingleThreadRingCS", SF_Compute);



void FS2L1_SingleThreadRingCS::FS2L1_SingleThreadRingCSRun(UTextureRenderTarget2D* OutputRenderTarget)
{
	
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[OutputRenderTarget](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering render target in RDG 
			FRDGTextureRef FS2L1_OutputTextureRDGRef = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(OutputRenderTarget->GetRenderTargetResource()->GetTextureRHI(),
					TEXT("FS2L1_RenderTarget"))) ;
			
			//ShaderParameterSetup
			FS2L1_SingleThreadRingCS::FParameters* FS2L1_Parameters = GraphBuilder.AllocParameters<
				FS2L1_SingleThreadRingCS::FParameters>();
			
			FS2L1_Parameters->FS2L1_Output =  GraphBuilder.CreateUAV(FS2L1_OutputTextureRDGRef)  ; //Compute Shader directly writes on this RT
			FS2L1_Parameters->Centre = OutputRenderTarget->SizeX/2;
			FS2L1_Parameters->CircleRadius = OutputRenderTarget->SizeX/3; //Assuming that RT is a squaree
			//Dispatching shader
			//Initialising compute shader object
			TShaderMapRef<FS2L1_SingleThreadRingCS> FS2L1_ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			//Setting up Group Count
			FIntVector GroupCount = FIntVector(1,1,1);
			
			//Actually dispatching the shader
			
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS1L3"),
				FS2L1_ComputeShader,
				FS2L1_Parameters,
				GroupCount
				);
			
			GraphBuilder.Execute();
			
		}
	);
	
	
	
}
