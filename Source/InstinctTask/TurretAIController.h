// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TurretAIController.generated.h"

/**
 * 
 */
UCLASS()
class INSTINCTTASK_API ATurretAIController : public AAIController
{
	GENERATED_BODY()

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
};
