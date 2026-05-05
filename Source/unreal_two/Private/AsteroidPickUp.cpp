// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp/AsteroidPickUp.h"

// Sets default values
AAsteroidPickUp::AAsteroidPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAsteroidPickUp::BeginPlay()
{
	Super::BeginPlay();

	PickUpMeshComponent ->SetSimulatePhysics(true);
}

// Called every frame
void AAsteroidPickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

