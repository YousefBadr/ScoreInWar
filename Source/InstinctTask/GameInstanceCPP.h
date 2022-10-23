// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/StaticMesh.h"
#include "GameInstanceCPP.generated.h"

/**
 * 
 */

class ATurretAIController;
USTRUCT()
struct FTurretLocation
{
	GENERATED_USTRUCT_BODY()
	double X;
	double Y;
};

UCLASS()
class INSTINCTTASK_API UGameInstanceCPP : public UGameInstance
{
	GENERATED_BODY()

public:
	bool LoadStringArrayFromFile(TArray<FString>& StringArray, int32& ArraySize, FString FullFilePath, bool ExcludeEmptyLines);

	virtual void Init();

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	double ProjectileSpeed = 8000.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Pawn Location")
	FVector PawnSetLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Pawn Location")
	bool AllPlanesAndTureetsAreSet = false;

	UPROPERTY(BlueprintReadWrite, Category = "Begin Play")
	bool RemoveWelcomeWidget = false;

	UPROPERTY(BlueprintReadWrite, Category = "Coins")
	int32 CurrentNumberOfCoins = 0;

	UFUNCTION(BlueprintImplementableEvent)
	void AllIsDoneAndSet();
private:



	UPROPERTY(EditDefaultsOnly, Category = "Enemy Turrets")
	double CoinSpawningInterval = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Turrets")
		double MaximumNumberOfCoins = 50;

	double LastTimeSpawningCoins = 0;

	bool MyTick(float DeltaSeconds);

	FDelegateHandle TickDelegateHandle;
	void SpawnEnemyTurrets();

	void SpawnCoins();

	void SetVariablesFromFile();

	bool SpawnPlaneAtLocation(FVector Location, FRotator Rotation,FVector Scale);

	TArray<FString> FileContentStringArray;

	UPROPERTY(EditDefaultsOnly, Category = "Land")
	UStaticMesh* UnitPlane;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Turrets")
	TSubclassOf <AActor> TurretBluePrint;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Turrets")
	TSubclassOf <AActor> CoinBluePrint;

	void SpawnAllTiles();
	bool alltilesspawned = false;

	double LastPlaneSpawnTime = 0;
	uint64 NumberOfTurrets = 0;

	TArray<FTurretLocation> TurretsLocation;

	//Grid Size of the land
	uint64 GridSizeX = 0;
	uint64 GridSizeY = 0;
	uint64 TotalLandSize = 0;
};
