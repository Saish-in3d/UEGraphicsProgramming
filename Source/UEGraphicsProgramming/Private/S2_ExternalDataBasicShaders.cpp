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
	//TempCircleDataArray.Init(FCircleData(),inTotalCircleCount);
	
	float Speed = 100.f; //Pixel distance travelled by circle in a unit second
	float Halfspeed = Speed/2;
	float MinRadius = 10.f;
	float MaxRadius = 30.f;
	float RadiusRange = MaxRadius - MinRadius;
	
	for (int i = 0; i < inTotalCircleCount; i++)
	{
		
		FCircleData& TempCircleData = TempCircleDataArray.AddDefaulted_GetRef();
		
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
                                        const float inDeltatime, TArray<FCircleData> inCircleDatBuffer, int CircleCount)
{
	
	
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[OutputRenderTarget,inDeltatime,inCircleDatBuffer,CircleCount](FRHICommandListImmediate& RHICmdList)
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
				(sizeof(FCircleData))*(inCircleDatBuffer.Num()));
			
			
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
			FIntVector GroupCount = FIntVector(FMath::DivideAndRoundUp(CircleCount, ThreadsX),
																										1,1);
			
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


IMPLEMENT_GLOBAL_SHADER(FS2L3_CustomStructBufferReadBackCS, "/Plugins/UEGraphicsProgramming/FS2L3_CustomStructBufferReadBackCS.usf",
						"FS2L3_CustomStructBufferReadBackCS", SF_Compute);

void FS2L3_CustomStructBufferReadBackCS::PollReadBackandDownloadtoGameThread(TArray<FTransform>& StarTransformsArray, const int inStarCount, FRHIGPUBufferReadback* FRHIGPUBufferReadbackPointer)
{
	ENQUEUE_RENDER_COMMAND(PollReadback)(
		[ inStarCount,FRHIGPUBufferReadbackPointer,&StarTransformsArray](FRHICommandListImmediate& RHICmdList)
		{
			//Continously polling inFRHIGPUBufferReadback (stagging readback buffer queue) on render thread
			if (!FRHIGPUBufferReadbackPointer->IsReady()){return;}
		

			//Locking Gpu buffer for readback.
			const size_t NumBytes = sizeof(FTransform) * inStarCount;
			void* Data = FRHIGPUBufferReadbackPointer->Lock(NumBytes);

			//Init render thread staging CPU data array
			TArray<FTransform> CPUTransforms_RenderThread;
			CPUTransforms_RenderThread.SetNumUninitialized(inStarCount);
		
			//Copying the data to render thread staging array
			FMemory::Memcpy(CPUTransforms_RenderThread.GetData(), Data, NumBytes);

			//Now our copy is done we will unloack and release GPU stagging buffer
			FRHIGPUBufferReadbackPointer->Unlock();

			// Copying Render thread data to Game thread for further application
			AsyncTask(ENamedThreads::GameThread, [ CPUTransforms = MoveTemp(CPUTransforms_RenderThread),&StarTransformsArray]()
			{
				StarTransformsArray = CPUTransforms;
			});
		});
}

void FS2L3_CustomStructBufferReadBackCS::FS2L3_CustomStructBufferReadBackCSRun(const float inDeltatime,
                                                                               TArray<FTransform>& StarTransformsArray, 
                                                                               const int inStarCount, 
                                                                               TUniquePtr<FRHIGPUBufferReadback>&
                                                                               inFRHIGPUBufferReadback)
{
	if (!inFRHIGPUBufferReadback.IsValid()){return;}
	FRHIGPUBufferReadback* FRHIGPUBufferReadbackPointer = inFRHIGPUBufferReadback.Get();
	
	/*FRDGBufferRef StarTransformsGPUBuffer;*/
	
	ENQUEUE_RENDER_COMMAND(FS2L3_CustomStructBufferReadBackCSRun)(
		[inDeltatime,inStarCount,FRHIGPUBufferReadbackPointer]
		(FRHICommandListImmediate& RHICmdList)
		{
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			
			//Creating GPU RDG buffer desc
			
			FRDGBufferDesc StarTransformsGPUBufferDesc = FRDGBufferDesc::CreateByteAddressDesc(
	sizeof(FTransform) * inStarCount);

			
			//Creating GPU RDG buffer Ref
			FRDGBufferRef StarTransformsGPUBuffer = GraphBuilder.CreateBuffer(StarTransformsGPUBufferDesc,
																				TEXT("StarTransformsGPUBuffer"));				
			
			//Making GPU RDG buffer Ref as UAV
			const FRDGBufferUAVRef StarTransformsGPUBufferUAV = GraphBuilder.CreateUAV(StarTransformsGPUBuffer);
			AddClearUAVPass(GraphBuilder, StarTransformsGPUBufferUAV, 0);
			
			//ShaderParameterSetup
			FS2L3_CustomStructBufferReadBackCS::FParameters* FS2L3_Parameters = GraphBuilder.AllocParameters<
				FS2L3_CustomStructBufferReadBackCS::FParameters>();
			
			//Setting actual parameters
			FS2L3_Parameters->FS2L3StructDataBufferOutPut =  StarTransformsGPUBufferUAV;
			FS2L3_Parameters->Deltatime = inDeltatime;
			
			//Initialising compute shader object
			TShaderMapRef<FS2L3_CustomStructBufferReadBackCS> FS2L3_ComputeShader(GetGlobalShaderMap(
																								GMaxRHIFeatureLevel));
			
			int GroupCountX = (inStarCount + ThreadsX - 1)/ThreadsX;
			
			//Setting up Group Count
			FIntVector GroupCount = FIntVector(GroupCountX,1,1);
			
			//Actually dispatching the shader (at the end of execution output of shader will be stored in UAV)
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS2L3"),
				FS2L3_ComputeShader,
				FS2L3_Parameters,
				GroupCount
				);
			
			if (!StarTransformsGPUBuffer){return;}
			AddEnqueueCopyPass(GraphBuilder, FRHIGPUBufferReadbackPointer, StarTransformsGPUBuffer, /*OffsetBytes=*/0);
			
			// preparing StarTransformsGPUBuffer for cpu readback by adding it to 
			// inFRHIGPUBufferReadback (stagging readback buffer queue) this buffer is visible to cpu memory
			//Even tho gpu memory is visible to cpu but copying it before GPU finish its task will create desync issues
			//Large size data can take more than one 1 frame time for actual readback to cpu
			//Below code creates a copy of the buffer and creates a sync fence which we can poll (check) on render thread
			//to make sure that the GPU operation on that buffer is complete
			/*GraphBuilder.AddPass(
			RDG_EVENT_NAME("FS2L3_Readback"),
			ERDGPassFlags::None,
			[ FRHIGPUBufferReadbackPointer,StarTransformsGPUBuffer](FRHICommandListImmediate& RHICmdList)
			{
				if (!FRHIGPUBufferReadbackPointer) return;
				
				if (!StarTransformsGPUBuffer) return;
				
				FRHIGPUBufferReadbackPointer->EnqueueCopy(
					RHICmdList,
					StarTransformsGPUBuffer->GetRHI()
				);
			});*/

				GraphBuilder.Execute();
			
			
		});
	
	//EnqueueGPUReadback(inFRHIGPUBufferReadback,StarTransformsGPUBuffer);
	
	//return;
	//
	//PollReadBackandDownloadtoGameThread(StarTransformsArray, inStarCount, FRHIGPUBufferReadbackPointer);
	
	
	
	
	
	
	
	
}

bool FS2L3_CustomStructBufferReadBackCS::IsPendingReadback(TUniquePtr<FRHIGPUBufferReadback>& inFRHIGPUBufferReadback) const
{
	
	return !inFRHIGPUBufferReadback.IsValid();
	
}

void FS2L3_CustomStructBufferReadBackCS::EnqueueGPUReadback(TUniquePtr<FRHIGPUBufferReadback>& inFRHIGPUBufferReadback, FRDGBufferRef& inFRDGBufferRef)
{
	ensure(inFRHIGPUBufferReadback.IsValid());
	
	static const FName ReadbackName = TEXT("TransformRT_Buffer");
	
	ENQUEUE_RENDER_COMMAND(EnqueueGPUReadback)(
		[&inFRHIGPUBufferReadback,inFRDGBufferRef](FRHICommandListImmediate& RHICmdList)
		{
			inFRHIGPUBufferReadback.Reset(new FRHIGPUBufferReadback(ReadbackName));
			if (inFRHIGPUBufferReadback.IsValid() && inFRDGBufferRef->GetSize())
			{
				inFRHIGPUBufferReadback->EnqueueCopy(RHICmdList, inFRDGBufferRef->GetRHI());
				
			}
			
		}
	);
	
	
	
	
}

bool FS2L3_CustomStructBufferReadBackCS::IsGPUReadbackFinished(TUniquePtr<FRHIGPUBufferReadback>& inFRHIGPUBufferReadback) const
{
	
	return !IsPendingReadback(inFRHIGPUBufferReadback) && inFRHIGPUBufferReadback->IsReady();
	
}




