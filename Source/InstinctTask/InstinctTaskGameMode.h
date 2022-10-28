// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InstinctTaskGameMode.generated.h"

class ATurretAIController;
USTRUCT()
struct FTurretLocation
{
	GENERATED_USTRUCT_BODY()
	double X;
	double Y;
};


UCLASS()
class AInstinctTaskGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AInstinctTaskGameMode();

	bool LoadStringArrayFromFile(TArray<FString>& StringArray, int32& ArraySize, FString FullFilePath, bool ExcludeEmptyLines);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		double ProjectileSpeed = 8000.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Begin Play")
		bool RemoveWelcomeWidget = false;

	UPROPERTY(BlueprintReadWrite, Category = "Coins")
		int32 CurrentNumberOfCoins = 0;

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeGame();

	UPROPERTY(BlueprintReadOnly, Category = "Pawn Location")
	bool IsGameInitialized = false;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Turrets")
	double CoinSpawningInterval = 5;

private:

	void SetPlayerPawnLocationInTheMiddleOfGrid();


	UPROPERTY(EditDefaultsOnly, Category = "Enemy Turrets")
		double MaximumNumberOfCoins = 50;

	double LastTimeSpawningCoins = 0;

	FDelegateHandle TickDelegateHandle;
	void SpawnEnemyTurrets();

	void SpawnCoins();

	void SetVariablesFromFile();

	bool SpawnPlaneAtLocation(FVector Location, FRotator Rotation, FVector Scale);

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



