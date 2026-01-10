// Fill out your copyright notice in the Description page of Project Settings.


#include "SectionOneLessonOnePlane.h"
#include "Section_1/S1L1_FirstComputeShader.h"
#include "Section_1/S1L2-L5_BasicComputeShaders.h"

ASectionOneLessonOnePlane::ASectionOneLessonOnePlane() 
{
	
	
}

void ASectionOneLessonOnePlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!TempOutputRenderTarget){return;}
	switch (LessonIndex)
	{
		default: break;
		case 0: break;
		case 1: FS1L1_FirstComputeShaderCS::FS1L1_FirstCSRun(TempOutputRenderTarget); break;
		case 2:FS1L2_ClearColorCS::FS1L2_ClearColorCSRun(TempOutputRenderTarget);break;
		case 3:FS1L3_SplitScreenCS::FS1L3_SplitScreenCSRun(TempOutputRenderTarget);break;
	}
	
	
	
}

void ASectionOneLessonOnePlane::BeginPlay()
{
	Super::BeginPlay();
	
	
	
	
	
}
