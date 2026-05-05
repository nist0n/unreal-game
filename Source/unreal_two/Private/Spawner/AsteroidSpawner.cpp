// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner/AsteroidSpawner.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AAsteroidSpawner::AAsteroidSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Area"));

	MinSpawnDelay = 0.5f;
	MaxSpawnDelay = 1.5f;
}

// Called when the game starts or when spawned
void AAsteroidSpawner::BeginPlay()
{
	Super::BeginPlay();

	StartSpawnTimer();
}

FVector AAsteroidSpawner::GetRandomSpawnPoint()
{
	const FVector SpawnOrigin = SpawnArea->Bounds.Origin;
	const FVector SpawnLimits = SpawnArea->Bounds.BoxExtent;

	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnLimits);
}

void AAsteroidSpawner::SpawnActors()
{
	if (!ActorToSpawn || !GetWorld())
	{
		return;
	}

	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = this;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APickUpbase* SpawnedActor = GetWorld()->SpawnActor<APickUpbase>(ActorToSpawn, GetRandomSpawnPoint(),
		UKismetMathLibrary::RandomRotator(), spawnParameters);

	StartSpawnTimer();
}

void AAsteroidSpawner::StartSpawnTimer()
{
	RandomSpawnDelay = FMath::RandRange(MinSpawnDelay, MaxSpawnDelay);

	GetWorld()->GetTimerManager().SetTimer(SpawnTimer,this,
		&AAsteroidSpawner::SpawnActors,RandomSpawnDelay,false);
}


// Called every frame
void AAsteroidSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

