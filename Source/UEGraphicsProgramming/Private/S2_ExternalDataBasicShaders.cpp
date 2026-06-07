// Fill out your copyright notice in the Description page of Project Settings.


#include "S2_ExternalDataBasicShaders.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Chaos/AABBTree.h"
#include "Chaos/AABBTree.h"
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

void FS2L3_CustomStructBufferReadBackCS::PollReadBackandDownloadtoGameThread(TArray<FTransform>& StarTransformsArray, const int inStarCount, TQueue<TUniquePtr<FRHIGPUBufferReadback>, EQueueMode::
	                                                                             Spsc>&
                                                                             inFRHIGPUBufferReadbackQueue)
{

	
	ENQUEUE_RENDER_COMMAND(PollReadback)(
		[ inStarCount,&inFRHIGPUBufferReadbackQueue,&StarTransformsArray](FRHICommandListImmediate& RHICmdList)
		{
			
			TUniquePtr<FRHIGPUBufferReadback> tempFRHIGPUBufferReadback;
			
			if(!inFRHIGPUBufferReadbackQueue.Dequeue(tempFRHIGPUBufferReadback)){return;}
			
			if (!tempFRHIGPUBufferReadback){return;}
	
			FRHIGPUBufferReadback* FRHIGPUBufferReadbackPointer = tempFRHIGPUBufferReadback.Get();
			
			
			//Continously polling inFRHIGPUBufferReadback (stagging readback buffer queue) on render thread
			if (!FRHIGPUBufferReadbackPointer->IsReady())
			{
				inFRHIGPUBufferReadbackQueue.Enqueue(MoveTemp(tempFRHIGPUBufferReadback));
				return;
			}
		

			//Locking Gpu buffer for readback.
			const size_t NumBytes = sizeof(FVector3f) * inStarCount;
			void* Data = FRHIGPUBufferReadbackPointer->Lock(NumBytes);

			//Init render thread staging CPU data array
			TArray<FVector3f> CPULocations_RenderThread;
			CPULocations_RenderThread.SetNumUninitialized(inStarCount);
		
			//Copying the data to render thread staging array
			FMemory::Memcpy(CPULocations_RenderThread.GetData(), Data, NumBytes);

			//Now our copy is done we will unloack and release GPU stagging buffer
			FRHIGPUBufferReadbackPointer->Unlock();
			
			TArray<FTransform> CPUTransforms_RenderThread;
			
			for (int i = 0; i < inStarCount; i++)
			{
				FTransform& trmpTransformRef = CPUTransforms_RenderThread.AddDefaulted_GetRef();
				trmpTransformRef.SetRotation(FRotator::ZeroRotator.Quaternion());
				trmpTransformRef.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
				trmpTransformRef.SetTranslation(FVector(CPULocations_RenderThread[i].X,
					CPULocations_RenderThread[i].Y,
					CPULocations_RenderThread[i].Z)*100.f);
				
				
			}
			

			// Copying Render thread data to Game thread for further application
			AsyncTask(ENamedThreads::GameThread, [ CPUTransforms = MoveTemp(CPUTransforms_RenderThread),
				&StarTransformsArray]()
			{
				StarTransformsArray = CPUTransforms;
			});
		});
	
	
}



void FS2L3_CustomStructBufferReadBackCS::FS2L3_CustomStructBufferReadBackCSRun(const float inDeltatime,
                                                                               const int inStarCount,
                                                                               TQueue<TUniquePtr<FRHIGPUBufferReadback>, EQueueMode::Spsc>& inFRHIGPUBufferReadbackQueue)
{
	TUniquePtr<FRHIGPUBufferReadback> tempFRHIGPUBufferReadback = MakeUnique <FRHIGPUBufferReadback>(
		TEXT("FS2L3_CustomStructBuffer"));
	
	
	if (!tempFRHIGPUBufferReadback.IsValid()){return;}
	FRHIGPUBufferReadback* FRHIGPUBufferReadbackPointer = tempFRHIGPUBufferReadback.Get();
	float Time_RT = inDeltatime;
	ENQUEUE_RENDER_COMMAND(FS2L3_CustomStructBufferReadBackCSRun)(
		[Time_RT,inStarCount,FRHIGPUBufferReadbackPointer]
		(FRHICommandListImmediate& RHICmdList)
		{
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			
			//Creating GPU RDG buffer desc
			
			FRDGBufferDesc StarTransformsGPUBufferDesc = FRDGBufferDesc::CreateByteAddressDesc(
																			sizeof(FVector3f) * inStarCount);

			
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
			FS2L3_Parameters->Deltatime = Time_RT;
			
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
			// preparing StarTransformsGPUBuffer for cpu readback by adding it to 
			// FRHIGPUBufferReadbackPointer (stagging readback buffer queue) this buffer is visible to cpu memory
			//Even tho gpu memory is visible to cpu but copying it before GPU finish its task will create desync issues
			//Large size data can take more than one 1 frame time for actual readback to cpu
			//Below func creates a copy of the buffer and creates a sync fence which we can poll (check) on render thread
			//to make sure that the GPU operation on that buffer is complete
			AddEnqueueCopyPass(GraphBuilder, FRHIGPUBufferReadbackPointer, StarTransformsGPUBuffer, /*OffsetBytes=*/0);
			GraphBuilder.Execute();

		});
	
	inFRHIGPUBufferReadbackQueue.Enqueue(MoveTemp(tempFRHIGPUBufferReadback));
}

IMPLEMENT_GLOBAL_SHADER(FS2L4_BasicNoiseCS, "/Plugins/UEGraphicsProgramming/FS2L4_BasicNoiseCS.usf",
						"FS2L4_BasicNoiseCS", SF_Compute);

void FS2L4_BasicNoiseCS::FS2L4_BasicNoiseCSRun(UTextureRenderTarget2D* OutputRenderTarget, const float inDeltatime)
{
	
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[OutputRenderTarget, inDeltatime](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering render target in RDG 
			FRDGTextureRef FS2L4_OutputTextureRDGRef = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(OutputRenderTarget->GetRenderTargetResource()->GetTextureRHI(),
					TEXT("FS2L4_RenderTarget"))) ;
			
			//ShaderParameterSetup
			FS2L4_BasicNoiseCS::FParameters* FS2L4_Parameters = GraphBuilder.AllocParameters<
				FS2L4_BasicNoiseCS::FParameters>();
			
			FS2L4_Parameters->FS2L4_Output =  GraphBuilder.CreateUAV(FS2L4_OutputTextureRDGRef)  ; //Compute Shader directly writes on this RT
			FS2L4_Parameters->Deltatime = inDeltatime;
			FS2L4_Parameters->TextureResolution = OutputRenderTarget->SizeX;
			
			//Dispatching shader
			//Initialising compute shader object
			TShaderMapRef<FS2L4_BasicNoiseCS> FS2L4_BasicNoiseCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			//Setting up Group Count
			FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(OutputRenderTarget->SizeX,
				OutputRenderTarget->SizeY), FComputeShaderUtils::kGolden2DGroupSize);
			
			//Actually dispatching the shader
			
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS2L4"),
				FS2L4_BasicNoiseCS,
				FS2L4_Parameters,
				GroupCount
				);
			
			GraphBuilder.Execute();
			
		}
	);
	
	

	
}


IMPLEMENT_GLOBAL_SHADER(FS2L5_ProceduralWoodCS, "/Plugins/UEGraphicsProgramming/FS2L5_ProceduralWoodCS.usf",
						"FS2L5_ProceduralWoodCS", SF_Compute);

void FS2L5_ProceduralWoodCS::FS2L5_ProceduralWoodCSRun(UTextureRenderTarget2D* OutputRenderTarget,
	const FProceduralWoodData inProceduralWoodData)
{
	
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[OutputRenderTarget, inProceduralWoodData](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering render target in RDG 
			FRDGTextureRef FS2L5_OutputTextureRDGRef = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(OutputRenderTarget->GetRenderTargetResource()->GetTextureRHI(),
					TEXT("FS2L5_RenderTarget"))) ;
			
			
			FRDGBufferRef ProceduralWoodInputData = CreateStructuredBuffer(GraphBuilder,
				TEXT("FS2L5_ProceduralWoodInputData"),
				sizeof(FProceduralWoodData),
				1,
				&inProceduralWoodData,
				sizeof(FProceduralWoodData));
			
			FRDGBufferSRVRef SRV_ProceduralWoodInputData = GraphBuilder.CreateSRV(ProceduralWoodInputData);
			
			//ShaderParameterSetup
			FS2L5_ProceduralWoodCS::FParameters* FS2L5_Parameters = GraphBuilder.AllocParameters<
				FS2L5_ProceduralWoodCS::FParameters>();
			
			FS2L5_Parameters->FS2L5_Output =  GraphBuilder.CreateUAV(FS2L5_OutputTextureRDGRef)  ; //Compute Shader directly writes on this RT
			FS2L5_Parameters->ProceduralWoodData = SRV_ProceduralWoodInputData;
			FS2L5_Parameters->TextureResolution = OutputRenderTarget->SizeX;
			
			//Dispatching shader
			//Initialising compute shader object
			TShaderMapRef<FS2L5_ProceduralWoodCS> FS2L5_ProceduralWoodCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			//Setting up Group Count
			FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(OutputRenderTarget->SizeX,
				OutputRenderTarget->SizeY), FComputeShaderUtils::kGolden2DGroupSize);
			
			//Actually dispatching the shader
			
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS2L5"),
				FS2L5_ProceduralWoodCS,
				FS2L5_Parameters,
				GroupCount
				);
			
			GraphBuilder.Execute();
			
		}
	);
	
	
}


IMPLEMENT_GLOBAL_SHADER(FS2L6_ProceduralMarbleCS, "/Plugins/UEGraphicsProgramming/FS2L6_ProceduralMarbleCS.usf",
						"FS2L6_ProceduralMarbleCS", SF_Compute);


void FS2L6_ProceduralMarbleCS::FS2L6_ProceduralMarbleCSRun(UTextureRenderTarget2D* OutputRenderTarget)
{
	
	// Executing logic in render thread while passing OutputRender target to render thread via this lambda function
	ENQUEUE_RENDER_COMMAND(DispatchMyCompute)(
		[OutputRenderTarget](FRHICommandListImmediate& RHICmdList)
		{
			
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			
			//Registering render target in RDG 
			FRDGTextureRef FS2L6_OutputTextureRDGRef = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(OutputRenderTarget->GetRenderTargetResource()->GetTextureRHI(),
					TEXT("FS2L6_RenderTarget"))) ;
			
			//ShaderParameterSetup
			FS2L6_ProceduralMarbleCS::FParameters* FS2L6_Parameters = GraphBuilder.AllocParameters<
				FS2L6_ProceduralMarbleCS::FParameters>();
			
			FS2L6_Parameters->FS2L6_Output =  GraphBuilder.CreateUAV(FS2L6_OutputTextureRDGRef)  ; //Compute Shader directly writes on this RT
			FS2L6_Parameters->Marble = 1;
			FS2L6_Parameters->TextureResolution = OutputRenderTarget->SizeX;
			
			//Dispatching shader
			//Initialising compute shader object
			TShaderMapRef<FS2L6_ProceduralMarbleCS> FS2L6_ProceduralMarbleCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			//Setting up Group Count
			FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(OutputRenderTarget->SizeX,
				OutputRenderTarget->SizeY), FComputeShaderUtils::kGolden2DGroupSize);
			
			//Actually dispatching the shader
			
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS2L6"),
				FS2L6_ProceduralMarbleCS,
				FS2L6_Parameters,
				GroupCount
				);
			
			GraphBuilder.Execute();
			
		}
	);
	
	
}


IMPLEMENT_GLOBAL_SHADER(FS2L7_ReadbackDeformMeshCS, "/Plugins/UEGraphicsProgramming/FS2L7_ReadbackDeformMeshCS.usf",
						"FS2L7_ReadbackDeformMeshCS", SF_Compute);

void FS2L7_ReadbackDeformMeshCS::FS2L7_ReadbackDeformMeshCSRun(const float inAlpha,
                                                               const int inVerticeCount,
                                                               TArray<FVector3f>& initialVerticesBuffer,
                                                               TQueue<TUniquePtr<FRHIGPUBufferReadback>,
                                                               EQueueMode::Spsc>& inFRHIGPUBufferReadbackQueue)
{
	TUniquePtr<FRHIGPUBufferReadback> tempFRHIGPUBufferReadback = MakeUnique <FRHIGPUBufferReadback>(
		TEXT("FS2L7_VertBuffer"));
	
	
	if (!tempFRHIGPUBufferReadback.IsValid()){return;}
	FRHIGPUBufferReadback* FRHIGPUBufferReadbackPointer = tempFRHIGPUBufferReadback.Get();
	float Alpha_RT = inAlpha;
	ENQUEUE_RENDER_COMMAND(FS2L7_ReadbackDeformMeshCSRun)(
		[Alpha_RT,inVerticeCount,initialVerticesBuffer,FRHIGPUBufferReadbackPointer]
		(FRHICommandListImmediate& RHICmdList)
		{
			//Getting new graph builder each frame
			FRDGBuilder GraphBuilder(RHICmdList);
			

			//Creating GPU RDG buffer ref
			FRDGBufferRef InputRDGVertBuffer = CreateStructuredBuffer(GraphBuilder,TEXT("InputVertBuffer"),
														sizeof(FVector3f),
														inVerticeCount,
														initialVerticesBuffer.GetData(),
														sizeof(FVector3f) * inVerticeCount);
			
			//Create SRV out of it
			const FRDGBufferSRVRef InputRDGRef_SRV = GraphBuilder.CreateSRV(InputRDGVertBuffer);

			
			//Creating Output GPU RDG buffer desc
			
			FRDGBufferDesc VertTransformsGPUBufferDesc = FRDGBufferDesc::CreateByteAddressDesc(
																			sizeof(FVector3f) * inVerticeCount);

			
			//Creating GPU RDG buffer Ref
			FRDGBufferRef VertTransformsGPUBuffer = GraphBuilder.CreateBuffer(VertTransformsGPUBufferDesc,
																				TEXT("VertTransformsGPUBuffer"));				
			
			//Making GPU RDG buffer Ref as UAV
			const FRDGBufferUAVRef VertTransformsGPUBufferUAV = GraphBuilder.CreateUAV(VertTransformsGPUBuffer);
			AddClearUAVPass(GraphBuilder, VertTransformsGPUBufferUAV, 0);
			
			//ShaderParameterSetup
			FS2L7_ReadbackDeformMeshCS::FParameters* FS2L7_Parameters = GraphBuilder.AllocParameters<
				FS2L7_ReadbackDeformMeshCS::FParameters>();
			
			//Setting actual parameters
			FS2L7_Parameters->SphereRadius = 50;
			FS2L7_Parameters->Alpha = Alpha_RT;
			FS2L7_Parameters->FS2L7_InitialMeshVerticeBuffer = InputRDGRef_SRV;
			FS2L7_Parameters->FS2L7VerticesBufferOutput = VertTransformsGPUBufferUAV;
			
			//Initialising compute shader object
			TShaderMapRef<FS2L7_ReadbackDeformMeshCS> FS2L7_ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			
			int GroupCountX = (inVerticeCount + ThreadsX - 1)/ThreadsX;
			
			//Setting up Group Count
			FIntVector GroupCount = FIntVector(GroupCountX,1,1);
			
			//Actually dispatching the shader (at the end of execution output of shader will be stored in UAV)
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FS2L7"),
				FS2L7_ComputeShader,
				FS2L7_Parameters,
				GroupCount
				);
			
			if (!VertTransformsGPUBuffer){return;}
			// preparing StarTransformsGPUBuffer for cpu readback by adding it to 
			// FRHIGPUBufferReadbackPointer (stagging readback buffer queue) this buffer is visible to cpu memory
			//Even tho gpu memory is visible to cpu but copying it before GPU finish its task will create desync issues
			//Large size data can take more than one 1 frame time for actual readback to cpu
			//Below func creates a copy of the buffer and creates a sync fence which we can poll (check) on render thread
			//to make sure that the GPU operation on that buffer is complete
			AddEnqueueCopyPass(GraphBuilder, FRHIGPUBufferReadbackPointer, VertTransformsGPUBuffer, /*OffsetBytes=*/0);
			GraphBuilder.Execute();

		});
	
	inFRHIGPUBufferReadbackQueue.Enqueue(MoveTemp(tempFRHIGPUBufferReadback));
	
	
}

void FS2L7_ReadbackDeformMeshCS::PollReadBackandDownloadtoGameThread(TArray<FVector3f>& VertReadbackArray,
                                                                     const int inVerticeCount,
                                                                     TQueue<TUniquePtr<FRHIGPUBufferReadback>, EQueueMode::Spsc>& inFRHIGPUBufferReadbackQueue)
{
	ENQUEUE_RENDER_COMMAND(PollReadback)(
		[ inVerticeCount,&inFRHIGPUBufferReadbackQueue,&VertReadbackArray](FRHICommandListImmediate& RHICmdList)
		{
			
			TUniquePtr<FRHIGPUBufferReadback> tempFRHIGPUBufferReadback;
			
			if(!inFRHIGPUBufferReadbackQueue.Dequeue(tempFRHIGPUBufferReadback)){return;}
			
			if (!tempFRHIGPUBufferReadback){return;}
	
			FRHIGPUBufferReadback* FRHIGPUBufferReadbackPointer = tempFRHIGPUBufferReadback.Get();
			
			
			//Continously polling inFRHIGPUBufferReadback (stagging readback buffer queue) on render thread
			if (!FRHIGPUBufferReadbackPointer->IsReady())
			{
				inFRHIGPUBufferReadbackQueue.Enqueue(MoveTemp(tempFRHIGPUBufferReadback));
				return;
			}
		

			//Locking Gpu buffer for readback.
			const size_t NumBytes = sizeof(FVector3f) * inVerticeCount;
			void* Data = FRHIGPUBufferReadbackPointer->Lock(NumBytes);

			//Init render thread staging CPU data array
			TArray<FVector3f> CPULocations_RenderThread;
			CPULocations_RenderThread.SetNumUninitialized(inVerticeCount);
		
			//Copying the data to render thread staging array
			FMemory::Memcpy(CPULocations_RenderThread.GetData(), Data, NumBytes);

			//Now our copy is done we will unloack and release GPU stagging buffer
			FRHIGPUBufferReadbackPointer->Unlock();

			// Copying Render thread data to Game thread for further application
			AsyncTask(ENamedThreads::GameThread, [ CPUVerts = MoveTemp(CPULocations_RenderThread),
				&VertReadbackArray]()
			{
				VertReadbackArray = CPUVerts;
			});
		});
	
	
}

