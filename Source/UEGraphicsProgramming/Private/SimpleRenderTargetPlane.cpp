// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRenderTargetPlane.h"
#include "Section_1/S1L1_FirstComputeShader.h"
#include "Section_1/S1L2-L5_BasicComputeShaders.h"
#include "S2_ExternalDataBasicShaders.h"

//namespace  CircleDataNamespace
//{
ASimpleRenderTargetPlane::ASimpleRenderTargetPlane() 
{
	
	//CircleDataBuffer.Init(FCircleData(),32);
	if (!S2_L2_BufferCreated)
	{
		CircleDataBuffer = FS2L2_CustomStructBufferCS::InitCircleDataBuffer(32,256);
		S2_L2_BufferCreated= true;
	}
	
}

void ASimpleRenderTargetPlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!TempOutputRenderTarget){return;}
	
	if (SectionIndex == 1)
	{
		switch (LessonIndex)
		{
		default:break;
		case 0:break;
		case 1:FS1L1_FirstComputeShaderCS::FS1L1_FirstCSRun(TempOutputRenderTarget); break;
		case 2:FS1L2_ClearColorCS::FS1L2_ClearColorCSRun(TempOutputRenderTarget);break;
		case 3:FS1L3_SplitScreenCS::FS1L3_SplitScreenCSRun(TempOutputRenderTarget);break;
		case 4:FS1L4_SolidCircleCS::FS1L4_SolidCircleCSRun(TempOutputRenderTarget);break;
		}
	}
	else if (SectionIndex == 2)
	{
		switch (LessonIndex)
		{
		default:break;
		case 0:break;
		case 1:
			//Clearing previous output by setting clear color first on RT
			FS1L2_ClearColorCS::FS1L2_ClearColorCSRun(TempOutputRenderTarget);
			FS2L1_SingleThreadRingCS::FS2L1_SingleThreadRingCSRun(TempOutputRenderTarget, this->GetGameTimeSinceCreation()); break;
		case 2:
			
			
			if (CircleDataBuffer.IsEmpty()){return;}
			FS1L2_ClearColorCS::FS1L2_ClearColorCSRun(TempOutputRenderTarget);
			FS2L2_CustomStructBufferCS::FS2L2_CustomStructBufferCSRun(TempOutputRenderTarget, this->GetGameTimeSinceCreation(),CircleDataBuffer, 32); break;
		case 3:break;
		case 4:FS2L4_BasicNoiseCS::FS2L4_BasicNoiseCSRun(TempOutputRenderTarget,this->GetGameTimeSinceCreation());break;
		case 5:FS2L5_ProceduralWoodCS::FS2L5_ProceduralWoodCSRun(TempOutputRenderTarget,ProceduralWoodData);break;
		}
	}
	
	
	
}

void ASimpleRenderTargetPlane::BeginPlay()
{
	Super::BeginPlay();
	
	
	
	
	
}
//}
