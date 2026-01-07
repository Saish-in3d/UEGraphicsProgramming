// Fill out your copyright notice in the Description page of Project Settings.


#include "SectionOneLessonOnePlane.h"
#include "Section_1/S1L1_FirstComputeShader.h"

ASectionOneLessonOnePlane::ASectionOneLessonOnePlane() 
{
	
	
}

void ASectionOneLessonOnePlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!TempOutputRenderTarget){return;}
	FS1L1_FirstComputeShaderCS::FS1L1_FirstComputeShaderRun(TempOutputRenderTarget);
	
	
}

void ASectionOneLessonOnePlane::BeginPlay()
{
	Super::BeginPlay();
	
	
	
	
	
}
