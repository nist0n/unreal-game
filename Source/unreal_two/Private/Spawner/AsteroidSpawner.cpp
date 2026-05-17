#include "Spawner/AsteroidSpawner.h"

#include "PickUp/AsteroidPickUp.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

AAsteroidSpawner::AAsteroidSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Area"));
	SetRootComponent(SpawnArea);

	MinSpawnDelay = 0.5f;
	MaxSpawnDelay = 1.5f;
	LaunchDirection = FVector(-1.0f, 0.0f, 0.0f);
	LaunchSpeed = 2500.0f;
	AsteroidLifetime = 8.0f;
}

void AAsteroidSpawner::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnTimer();
}

FVector AAsteroidSpawner::GetRandomSpawnPoint() const
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

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APickUpbase* SpawnedActor = GetWorld()->SpawnActor<APickUpbase>(
		ActorToSpawn,
		GetRandomSpawnPoint(),
		UKismetMathLibrary::RandomRotator(),
		SpawnParameters);

	if (AAsteroidPickUp* Asteroid = Cast<AAsteroidPickUp>(SpawnedActor))
	{
		ConfigureSpawnedAsteroid(Asteroid);
	}

	StartSpawnTimer();
}

void AAsteroidSpawner::ConfigureSpawnedAsteroid(AAsteroidPickUp* Asteroid) const
{
	if (!IsValid(Asteroid))
	{
		return;
	}

	Asteroid->LaunchInDirection(LaunchDirection, LaunchSpeed);
	Asteroid->SetLifetimeSeconds(AsteroidLifetime);
}

void AAsteroidSpawner::StartSpawnTimer()
{
	RandomSpawnDelay = FMath::RandRange(MinSpawnDelay, MaxSpawnDelay);

	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimer,
		this,
		&AAsteroidSpawner::SpawnActors,
		RandomSpawnDelay,
		false);
}

void AAsteroidSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
