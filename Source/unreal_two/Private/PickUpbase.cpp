// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp/PickUpbase.h"

// Sets default values
APickUpbase::APickUpbase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickUpMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pick Up Mesh"));

	RootComponent = PickUpMeshComponent;
}

bool APickUpbase::IsPickUpActive() const
{
	return bIsActive;
}

void APickUpbase::SetPickUpActive(bool state)
{
	bIsActive = state;
}

// Called when the game starts or when spawned
void APickUpbase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickUpbase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

