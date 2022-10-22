// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretAimingComponent.h"
#include "Gameframework/Actor.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"

#include "Projectile.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "GameInstanceCPP.h"

// Sets default values for this component's properties
UTurretAimingComponent::UTurretAimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}
bool UTurretAimingComponent::IsBarrelMoving()
{
	if (!ensure(Barrel)) return false;
	return !(AimDirection.Equals(Barrel->GetForwardVector(), 0.01));
}

bool UTurretAimingComponent::IsOutOfReach()
{
	if (!(PlayerBall && Turret)) return true;
	//UE_LOG(LogTemp, Warning, TEXT("Distance between player and Turret is %f"), (Turret->GetComponentLocation() - PlayerBall->GetActorLocation()).Size());
	return ((Turret->GetComponentLocation() - PlayerBall->GetActorLocation()).Size()) > 1000;
}

void UTurretAimingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (IsOutOfReach())
		FiringState = EFiringState::OutOfReach;
	else if ((FPlatformTime::Seconds() - LastFireTime) < ReloadTimeInSeconds)
		FiringState = EFiringState::Reloading;
	else if (IsBarrelMoving())
		FiringState = EFiringState::Aiming;
	else
		FiringState = EFiringState::Locked;
}

void UTurretAimingComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentGameInstance = Cast<UGameInstanceCPP>(UGameplayStatics::GetGameInstance(GetWorld()));
	//So that first fire is after initial reload
	LastFireTime = FPlatformTime::Seconds();
	PlayerBall = GetWorld()->GetFirstPlayerController()->GetPawn();
}
void UTurretAimingComponent::Fire()
{
	if (!CurrentGameInstance) return;
	if (CurrentGameInstance->RemoveWelcomeWidget == false) return;

	if (FiringState != EFiringState::Reloading)
	{
		if (!ensure(Barrel))return;
		if (!ensure(ProjectileBluePrint))return;
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileBluePrint,
			Barrel->GetSocketLocation(FName("Projectile")),
			Barrel->GetSocketRotation(FName("Projectile")));

		if (!Projectile)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed To Spawn Projectile"));
			return;
		}

		Projectile->LaunchProjectile(LaunchSpeed);
		LastFireTime = FPlatformTime::Seconds();
	}
}

EFiringState UTurretAimingComponent::GetFiringState() const
{
	return FiringState;
}

void UTurretAimingComponent::AimAt(FVector HitLocation)
{
	if (FiringState==EFiringState::OutOfReach)
	{
		return;
	}
	//auto OurTankName=GetOwner()->GetName(); 
	if (!(Barrel && Turret)) { return; }
	auto BarrelLocation = Barrel->GetComponentLocation();
	FVector OutLaunchVelocity;
	FVector StartLocation = Barrel->GetSocketLocation(FName("Projectile"));

	if (!CurrentGameInstance)
	{
		CurrentGameInstance = Cast<UGameInstanceCPP>(UGameplayStatics::GetGameInstance(GetWorld()));
		return;
	}

	LaunchSpeed = CurrentGameInstance->ProjectileSpeed;


	//UE_LOG(LogTemp, Warning, TEXT("LaunchSpeed = %f"), LaunchSpeed);

	//Calculate the launch Velocity
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity(
		this,
		OutLaunchVelocity,
		StartLocation,
		HitLocation,
		LaunchSpeed,
		false,
		0,
		0,
		ESuggestProjVelocityTraceOption::DoNotTrace);
	if (bHaveAimSolution)
	{
		AimDirection = OutLaunchVelocity.GetSafeNormal();
		//auto OurTankName=GetOwner()->GetName(); 
		//auto Time=GetWorld()->GetTimeSeconds();
		//UE_LOG(LogTemp,Warning,TEXT("%s aiming at %s aim solution found"),*OurTankName,*HitLocation.ToString())
		MoveBarrelAndTurretTowards(AimDirection);
		//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, FString::Printf(TEXT("Found Solution Launch = %s"), *OutLaunchVelocity.ToString()));

	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, FString::Printf(TEXT("Can't Find Solution When launch speed is %f"), LaunchSpeed));
	}
}

void UTurretAimingComponent::Initialise(UStaticMeshComponent* BarrelToSet, UStaticMeshComponent* TurretToSet, TSubclassOf <AProjectile> ProjectileToSet)
{
	if (!ensure(BarrelToSet && TurretToSet && ProjectileToSet)) return;
	ProjectileBluePrint = ProjectileToSet;
	Barrel = BarrelToSet;
	Turret = TurretToSet;
}
void UTurretAimingComponent::MoveBarrelAndTurretTowards(FVector TargetAimDirection)
{
	//Work out Differance between current barrel rotation and TargetAimDirection
	if (!ensure(Barrel && Turret)) return;
	auto BarrelRotator = Barrel->GetForwardVector().Rotation();
	auto AimAsRotator = TargetAimDirection.Rotation();
	auto DeltaRotator = AimAsRotator - BarrelRotator;

	// Always yaw the shortest way
	Elevate(DeltaRotator.Pitch);

	if (FMath::Abs(DeltaRotator.Yaw) < 180)
		Rotate(DeltaRotator.Yaw);
	else //Avoid going the long-way round
		Rotate(-DeltaRotator.Yaw);
}


void UTurretAimingComponent::Elevate(float RelativeSpeed)
{
	//Move the barrel the right amount this frame 
	//Given a max elevation speed and frame time
	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1.0, 1.0);

	//Elevation change per frame
	auto ElevationChange = RelativeSpeed * ElevationMaxDegreesPerSecond * GetWorld()->DeltaTimeSeconds;
	auto RawNewElevation = Barrel->GetRelativeRotation().Pitch + ElevationChange;

	// auto RawNewElevation =ElevationChange;
	auto Elevation = FMath::Clamp<float>(RawNewElevation, MinElevationDegrees, MaxElevationDegrees);
	Barrel->SetRelativeRotation(FRotator(Elevation, 0, 0));
}

void UTurretAimingComponent::Rotate(float RelativeSpeed)
{
	//Rotate the Turret the right amount this frame 
	//Given a max rotation speed and frame time
	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1, 1);
	//Rotation change per frame
	auto RotationChange = RelativeSpeed * TurretRotationMaxDegreesPerSecond * GetWorld()->DeltaTimeSeconds;
	auto RawNewRotation = Turret->GetRelativeRotation().Yaw + RotationChange;

	Turret->SetRelativeRotation(FRotator(0, RawNewRotation, 0));
}