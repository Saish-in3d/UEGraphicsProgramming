// Fill out your copyright notice in the Description page of Project Settings.


#include "S2_ExternalDataBasicShaders.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Engine/TextureRenderTarget2D.h"


IMPLEMENT_GLOBAL_SHADER(FS2L1_SingleThreadRingCS, "/Plugins/UEGraphicsProgramming/FS2L1_SingleThreadRingCS.usf",
						"FS2L1_SingleThreadRingCS", SF_Compute);



void FS2L1_SingleThreadRingCS::FS2L1_SingleThreadRingCSRun(UTextureRenderTarget2D* OutputRenderTarget, const float inDeltatime)
{
	
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[OutputRenderTarget,inDeltatime](FRHICommandListImmediate& RHICmdList)
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
			FS2L1_Parameters->Deltatime = inDeltatime;
			//Dispatching shader
			//Initialising compute shader object
			TShaderMapRef<FS2L1_SingleThreadRingCS> FS2L1_ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			//Setting up Group Count
			FIntVector GroupCount = FIntVector(1,1,1);
			
			//Actually dispatching the shader
			
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS2L1"),
				FS2L1_ComputeShader,
				FS2L1_Parameters,
				GroupCount
				);
			
			GraphBuilder.Execute();
			
		}
	);
	
	
	
}

TArray<FCircleData> FS2L2_CustomStructBufferCS::InitCircleDataBuffer(int32 inTotalCircleCount, int RenderTargetSize)
{
	TArray<FCircleData> TempCircleDataArray;
	TempCircleDataArray.SetNum(inTotalCircleCount);
	
	float Speed = 100.f; //Pixel distance travelled by circle in a unit second
	float Halfspeed = Speed/2;
	float MinRadius = 10.f;
	float MaxRadius = 30.f;
	float RadiusRange = MaxRadius - MinRadius;
	
	for (FCircleData TempCircleData : TempCircleDataArray)
	{
		//Origin means start location of circle at the start of simulation
		TempCircleData.Origin.X = FMath::RandRange(0, RenderTargetSize);
		TempCircleData.Origin.Y = FMath::RandRange(0, RenderTargetSize);
		
		TempCircleData.Velocity.X = FMath::RandRange(0.f, Speed) - Halfspeed;
		TempCircleData.Velocity.Y = FMath::RandRange(0.f, Speed) - Halfspeed;
		
		TempCircleData.Radius = FMath::RandRange(0.f, RadiusRange) + MinRadius;

	}
	
	return TempCircleDataArray;
}



IMPLEMENT_GLOBAL_SHADER(FS2L2_CustomStructBufferCS, "/Plugins/UEGraphicsProgramming/FS2L2_CustomStructBufferCS.usf",
						"FS2L2_CustomStructBufferCS", SF_Compute);

void FS2L2_CustomStructBufferCS::FS2L2_CustomStructBufferCSRun(UTextureRenderTarget2D* OutputRenderTarget,
	const float inDeltatime, TArray<FCircleData> inCircleDatBuffer)
{
	
	
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[OutputRenderTarget,inDeltatime,inCircleDatBuffer](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering render target in RDG 
			FRDGTextureRef FS2L2_OutputTextureRDGRef = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(OutputRenderTarget->GetRenderTargetResource()->GetTextureRHI(),
					TEXT("FS2L2_RenderTarget"))) ;
			
			
			//Creating a SRV filled with CircleDataBuffer
			
			//1. Create and upload data to a structured buffer on GPU
			FRDGBufferRef CircleDataGPUBuffer = CreateStructuredBuffer(GraphBuilder,
				TEXT("CircleDataGPUBuffer"),
				sizeof(FCircleData),
				inCircleDatBuffer.Num(),
				inCircleDatBuffer.GetData(),
				sizeof(FCircleData)*inCircleDatBuffer.Num());
			
			
			//2.cREATING srv 
			FRDGBufferSRVRef CircleDataGPUBufferSRV = GraphBuilder.CreateSRV(CircleDataGPUBuffer);
			
			//ShaderParameterSetup
			FS2L2_CustomStructBufferCS::FParameters* FS2L2_Parameters = GraphBuilder.AllocParameters<
				FS2L2_CustomStructBufferCS::FParameters>();
			
			FS2L2_Parameters->FS2L2_Output =  GraphBuilder.CreateUAV(FS2L2_OutputTextureRDGRef); //Compute Shader directly writes on this RT
			FS2L2_Parameters->TextureResolution = OutputRenderTarget->SizeX;
			FS2L2_Parameters->CircleDataBuffer = CircleDataGPUBufferSRV; //Setting SRV as buffer
			FS2L2_Parameters->Deltatime = inDeltatime;
			//Dispatching shader
			//Initialising compute shader object
			TShaderMapRef<FS2L2_CustomStructBufferCS> FS2L2_ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			//Setting up Group Count
			FIntVector GroupCount = FIntVector(1,1,1);
			
			//Actually dispatching the shader
			
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS2L2"),
				FS2L2_ComputeShader,
				FS2L2_Parameters,
				GroupCount
				);
			
			GraphBuilder.Execute();
			
		}
	);
	
	
	
	
}
//S}
