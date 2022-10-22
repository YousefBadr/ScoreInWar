// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawningCoinsHandler.h"

// Sets default values
ASpawningCoinsHandler::ASpawningCoinsHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawningCoinsHandler::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASpawningCoinsHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}