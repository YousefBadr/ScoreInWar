// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretAIController.h"

#include "Gameframework/Actor.h"
#include "Engine/World.h"
#include "AIController.h"
#include "TurretAimingComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"


void ATurretAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ATurretAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    auto TankAimingComponent = GetPawn()->FindComponentByClass<UTurretAimingComponent>();
    if (!ensure(TankAimingComponent))return;
    auto PossessdTurret = GetPawn();
    auto PlayerBall = GetWorld()->GetFirstPlayerController()->GetPawn();

    if (!(PlayerBall && PossessdTurret))return;

    //Aim towards player
    TankAimingComponent->AimAt(PlayerBall->GetActorLocation());

    //shoot the player if we are locked on him.
    if (TankAimingComponent->GetFiringState() == EFiringState::Locked)
    {
        TankAimingComponent->Fire(); 
    }


}