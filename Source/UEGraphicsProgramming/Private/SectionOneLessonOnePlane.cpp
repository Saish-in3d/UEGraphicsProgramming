// Fill out your copyright notice in the Description page of Project Settings.


#include "SectionOneLessonOnePlane.h"
#include "Section_1/S1L1_FirstComputeShader.h"
#include "Section_1/S1L2_SolidColor.h"

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
		case 0: FS1L1_FirstComputeShaderCS::FS1L1_FirstComputeShaderRun(TempOutputRenderTarget); break;
		case 1:FS1L2_ClearColorCS::FS1L2_ClearColorShaderRun(TempOutputRenderTarget);break;
	}
	
	
	
}

void ASectionOneLessonOnePlane::BeginPlay()
{
	Super::BeginPlay();
	
	
	
	
	
}
