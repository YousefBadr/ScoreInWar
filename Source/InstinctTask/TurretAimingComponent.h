// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TurretAimingComponent.generated.h"

class AProjectile;
class UGameInstanceCPP;

//Enum for aiming state
UENUM()
enum class EFiringState : uint8 //uint8 can hold up to 256 condition
{
	Reloading,
	Aiming,
	Patrol,
	OutOfReach,
	Locked,
	OutOfAmmo
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INSTINCTTASK_API UTurretAimingComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	//Used to Aim at player. called by AI controller.
	void AimAt(FVector HitLocation);

	//Fire called when locked on player
	UFUNCTION(BlueprintCallable, Category = "Firing")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Initialise(UStaticMeshComponent* BarrelToSet, UStaticMeshComponent* TurretToSet, TSubclassOf <AProjectile> ProjectileToSet);

	//gets the above firing state
	EFiringState GetFiringState() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EFiringState FiringState = EFiringState::Reloading;

private:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsBarrelMoving();

	//Player is not in Turret's radius
	bool IsOutOfReach();

	// Sets default values for this component's properties
	UTurretAimingComponent();

	//when player in radius. we rotate barrel and Dome towrds the player.
	void MoveBarrelAndTurretTowards(FVector AimDirection);

	APawn* PlayerBall = nullptr;
	UGameInstanceCPP* CurrentGameInstance = nullptr;
	UStaticMeshComponent* Barrel = nullptr;
	UStaticMeshComponent* Turret = nullptr;

	//last time we fired at player.
	double LastFireTime = 0;

	//where we are aiming right now.
	FVector AimDirection;

	//the projectls used to be launched and hit player.
	TSubclassOf <AProjectile> ProjectileBluePrint;

	//THIS IS CRITICAL. setting it too low "SuggestProjectileVelocity" won't find a solution to hit player and fail if player is far.
	float LaunchSpeed = 800;


	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float ReloadTimeInSeconds = 1;


public:
	// -1 is max downward speed and 1 is max up speed
	void Elevate(float RelativeSpeed);

	//-1 is max downward speed and 1 is max up speed
	void Rotate(float RelativeSpeed);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float ElevationMaxDegreesPerSecond = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float MaxElevationDegrees = 60;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float MinElevationDegrees = -60;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float TurretRotationMaxDegreesPerSecond = 60;
		
};
