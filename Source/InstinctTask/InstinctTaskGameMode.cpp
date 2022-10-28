// Copyright Epic Games, Inc. All Rights Reserved.

#include "InstinctTaskGameMode.h"
#include "InstinctTaskBall.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"

AInstinctTaskGameMode::AInstinctTaskGameMode()
{
	// set default pawn class to our ball
	DefaultPawnClass = AInstinctTaskBall::StaticClass();

	//Tick is enabled so we can spawn coins periodically
	PrimaryActorTick.bCanEverTick = true;

}



void AInstinctTaskGameMode::BeginPlay()
{

	Super::BeginPlay();

	FString FileName = "GameData.txt";
	//FString FilePath = FPaths::Combine(FPaths::ProjectContentDir(), "GameData", FileName);
	FString FilePath = FPaths::ProjectContentDir() + "GameData" + '/' + FileName;
	UE_LOG(LogTemp, Warning, TEXT("FilePath = %s"), *FilePath);

	int32 ArrSize = FileContentStringArray.Num();

	if (LoadStringArrayFromFile(FileContentStringArray, ArrSize, FilePath, true))
	{
		for (int i = 0; i < FileContentStringArray.Num(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("FileContentStringArray[%d]: %s"), i, *FileContentStringArray[i]);
		}
	}

	//Set Important Variables From File
	SetVariablesFromFile();

	//SpawnGridPlanes
	SpawnAllTiles();

	//Spawn Enemy Turrets
	SpawnEnemyTurrets();
	
	//Call Initialize Game in Blueprint to remove loading widget and show in-game widget.
	InitializeGame();
	IsGameInitialized = true;

	//sets player ball in the middle of the grid
	SetPlayerPawnLocationInTheMiddleOfGrid();
}

void AInstinctTaskGameMode::SetPlayerPawnLocationInTheMiddleOfGrid()
{
	//Set Pawn Location
	FVector PawnSetLocation = FVector(((GridSizeX * 1000) / 2) + 700, ((GridSizeY * 1000) / 2) + 700, 500);
	auto PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn(); //be careful in multiplayer.
	if (!PlayerPawn)return;
	PlayerPawn->SetActorLocation(PawnSetLocation);
}

void AInstinctTaskGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/*An equivalent method for a delay node in Blueprints.
	"CoinSpawningInterval" is blueprint editable.*/
	if (FPlatformTime::Seconds() - LastTimeSpawningCoins >= CoinSpawningInterval)
	{
		if (!(CurrentNumberOfCoins >= MaximumNumberOfCoins))
		{
			SpawnCoins();
			LastTimeSpawningCoins = FPlatformTime::Seconds();
			UE_LOG(LogTemp, Warning, TEXT("LastTimeSpawningCoins = %f"), LastTimeSpawningCoins);
		}
	}

}


void AInstinctTaskGameMode::SpawnAllTiles()
{
	TotalLandSize = (GridSizeX * 1000.0f) + (GridSizeX * 1000.0f);

	//Spawn Grid Tiles
	if (UnitPlane)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnitPlane Is Not Null!"));

		FVector Location(1000.0f, 1000.0f, 0.0f);

		//Next ground plane location.
		int NextLocationAddition = 1000;

		//Next wall plane location.
		int NextLocationForTheWalls = 500;

		for (int i = 0; i < GridSizeX; i++)
		{
			//Spawn Horizontal Planes
			for (int j = 0; j < GridSizeY; j++)
			{
				//Check if at the first row or last row to apply WALLS. so the player doesn't fall off. (only falls by powerful hit)
				if (i == 0 || i == GridSizeX - 1)
				{
					int temp = NextLocationForTheWalls;
					//spawn walls special cases. if we reach the end.
					if (i == GridSizeX - 1 && GridSizeX % 2 != 0)
						temp = -1 * NextLocationForTheWalls;
					SpawnPlaneAtLocation(FVector(Location.X - temp, Location.Y, Location.Z), FRotator(90.0f, 180.0f, 180.0f), FVector(3.0f, 10.0f, 3.0f));
					// if the size of GridX is 1 only, then we spawn another last Wall at the top.
					if (GridSizeX == 1)
					{
						SpawnPlaneAtLocation(FVector(Location.X + temp, Location.Y, Location.Z), FRotator(90.0f, 180.0f, 180.0f), FVector(3.0f, 10.0f, 3.0f));
					}
				}
				//Spawning Ground Planes.
				UE_LOG(LogTemp, Warning, TEXT("Location = %s"), *Location.ToString());
				SpawnPlaneAtLocation(Location, FRotator(0.0f, 0.0f, 0.0f), FVector(10.0f, 10.0f, 10.0f));

				//Check if at the first column or last column to apply WALLS. so the player doesn't fall off. (only falls by powerful hit)
				if (j == GridSizeY - 1 || j == 0)
				{
					int temp = NextLocationForTheWalls;
					//If at the first column spwan walls on the left.
					if (j == 0)
						temp = -1 * NextLocationForTheWalls;
					SpawnPlaneAtLocation(FVector(Location.X, Location.Y + temp, Location.Z), FRotator(-90.0f, 0.0f, 90.0f), FVector(3.0f, 10.0f, 3.0f));

					// if the size of GridY is 1 only, then we spawn another last Wall at the top.
					if (GridSizeY == 1)
					{
						SpawnPlaneAtLocation(FVector(Location.X, Location.Y - temp, Location.Z), FRotator(-90.0f, 0.0f, 90.0f), FVector(3.0f, 10.0f, 3.0f));
					}
				}
				//GoTo the next Y Tile in order if it's not the last tile.
				if (j != GridSizeY - 1)
				{
					Location.Y = Location.Y + NextLocationAddition;
				}

			}
			//GoTo the next X Tile in order if it's not the last tile.
			Location.X = Location.X + 1000;

			//Switch the walls and planes direction
			NextLocationAddition = -1 * NextLocationAddition;
			NextLocationForTheWalls = -1 * NextLocationForTheWalls;

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UnitPlane Is Null!"));
	}


}
void AInstinctTaskGameMode::SpawnCoins()
{
	if (!CoinBluePrint)return;

	//number of coins to be spawned in this run
	int32 TotalNumberOfCoins = FMath::RandRange(0, GridSizeX * GridSizeY * 3);

	//don't spawn more than the MaximumNumberOfCoins.
	TotalNumberOfCoins = FMath::Min(TotalNumberOfCoins, ((int32)MaximumNumberOfCoins) - CurrentNumberOfCoins - 1);

	for (int i = 0; i <= TotalNumberOfCoins; i++)
	{
		//Just move location by 30 so it doesn't spawn on walls
		int32 PositionX = FMath::RandRange(530, (GridSizeX * 1000) - 30);
		int32 PositionY = FMath::RandRange(530, (GridSizeY * 1000) - 30);

		FActorSpawnParameters MyActorSpawnParameters;
		MyActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		FVector NewLocation(PositionX, PositionY, 60.0f);
		FRotator NewRotation(0.0f, 0.0f, 0.0f);
		auto CheckSpawn = GetWorld()->SpawnActor(CoinBluePrint, &NewLocation, &NewRotation, MyActorSpawnParameters);
		if (CheckSpawn)CurrentNumberOfCoins += 1;
	}

}


void AInstinctTaskGameMode::SpawnEnemyTurrets()
{
	for (int i = 0; i < TurretsLocation.Num(); i++)
	{
		FVector NewLocation(TurretsLocation[i].X, TurretsLocation[i].Y, 10.0f);
		FRotator NewRotation(0.0f, 0.0f, 0.0f);

		UE_LOG(LogTemp, Warning, TEXT("TurretSpawnLocation[%d]: %s"), i, *NewLocation.ToString());

		FActorSpawnParameters MyActorSpawnParameters;
		MyActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

		auto Turret = GetWorld()->SpawnActor(TurretBluePrint, &NewLocation, &NewRotation, MyActorSpawnParameters);
	}

}

void AInstinctTaskGameMode::SetVariablesFromFile()
{

	for (int i = 0; i < FileContentStringArray.Num(); i++)
	{
		if (FileContentStringArray[i].Equals("Grid Size"))
		{
			i++;
			FString StringGridX, StringGridY;
			FileContentStringArray[i].Split(TEXT(" "), &StringGridX, &StringGridY);
			GridSizeX = FCString::Strtoui64(*StringGridX, NULL, 10);
			GridSizeY = FCString::Strtoui64(*StringGridY, NULL, 10);
			//if (GridSizeX == 3)FGenericPlatformMisc::RequestExit(false);
			UE_LOG(LogTemp, Warning, TEXT("GridSizeX = %d,GridSizeY = %d"), GridSizeX, GridSizeY);
		}
		else if (FileContentStringArray[i].Equals("Number Of Turrets"))
		{
			i++;
			NumberOfTurrets = FCString::Strtoui64(*FileContentStringArray[i], NULL, 10);
			UE_LOG(LogTemp, Warning, TEXT("NumberOfTurrets = %d"), NumberOfTurrets);
		}
		else if (FileContentStringArray[i].Equals("Turrets Location"))
		{
			for (int j = 0; j < NumberOfTurrets; j++)
			{
				i++;
				FString StringLocationX, StringLocationY;
				FTurretLocation temp;
				FileContentStringArray[i].Split(TEXT(" "), &StringLocationX, &StringLocationY);

				temp.X = FCString::Atod(*StringLocationX) * 1000;
				temp.Y = FCString::Atod(*StringLocationY) * 1000;

				//Prevent Code from spawning Turrets outside of the map.
				if (temp.X > (GridSizeX * 1000) || temp.X < 1) continue;
				if (temp.Y > (GridSizeY * 1000) || temp.Y < 1) continue;

				TurretsLocation.Add(temp);
				UE_LOG(LogTemp, Warning, TEXT("TurretsLocation[%d].X = %f,TurretsLocation[%d].Y = %f"), j, TurretsLocation[j].X, j, TurretsLocation[j].Y);
			}
		}
		else if (FileContentStringArray[i].Equals("Projectile Speed"))
		{
			//remmember don't set it too low or the turret won't shoot far.
			i++;
			NumberOfTurrets = FCString::Strtoui64(*FileContentStringArray[i], NULL, 10);
			ProjectileSpeed = FCString::Atod(*FileContentStringArray[i]);
			UE_LOG(LogTemp, Warning, TEXT("ProjectileSpeed = %f"), ProjectileSpeed);
		}
	}
}


bool AInstinctTaskGameMode::SpawnPlaneAtLocation(FVector Location, FRotator Rotation, FVector Scale)
{
	FActorSpawnParameters SpawnInfo;
	AStaticMeshActor* MyNewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	MyNewActor->SetMobility(EComponentMobility::Movable);
	MyNewActor->SetActorLocation(Location);
	MyNewActor->SetActorRotation(Rotation);
	MyNewActor->SetActorScale3D(Scale);
	UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(UnitPlane);
	}
	return true;
}

bool AInstinctTaskGameMode::LoadStringArrayFromFile(TArray<FString>& StringArray, int32& ArraySize, FString FullFilePath, bool ExcludeEmptyLines)
{
	ArraySize = 0;

	if (FullFilePath == "" || FullFilePath == " ") return false;

	//Empty any previous contents!
	if (StringArray.Num() != 0)
		StringArray.Empty();

	TArray<FString> FileArray;

	if (!FFileHelper::LoadANSITextFileToStrings(*FullFilePath, NULL, FileArray))
	{
		return false;
	}

	if (ExcludeEmptyLines)
	{
		for (const FString& Each : FileArray)
		{
			if (Each == "") continue;

			//check for any non whitespace
			bool FoundNonWhiteSpace = false;
			for (int32 v = 0; v < Each.Len(); v++)
			{
				if (Each[v] != ' ' && Each[v] != '\n')
				{
					FoundNonWhiteSpace = true;
					break;
				}
				//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			}

			if (FoundNonWhiteSpace)
			{
				StringArray.Add(Each);
			}
		}
	}
	else
	{
		StringArray.Append(FileArray);
	}

	ArraySize = StringArray.Num();
	return true;
}

