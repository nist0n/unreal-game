#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUp/PickUpbase.h"
#include "Components/BoxComponent.h"
#include "AsteroidSpawner.generated.h"

class AAsteroidPickUp;

UCLASS()
class UNREAL_TWO_API AAsteroidSpawner : public AActor
{
	GENERATED_BODY()

public:
	AAsteroidSpawner();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawner")
	TObjectPtr<UBoxComponent> SpawnArea;

	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<APickUpbase> ActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float MinSpawnDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float MaxSpawnDelay;

	/** World-space direction asteroids travel (normalized automatically). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner|Launch")
	FVector LaunchDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner|Launch", meta = (ClampMin = "0.0"))
	float LaunchSpeed;

	/** Destroy spawned asteroids after this many seconds (0 = never). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner|Launch", meta = (ClampMin = "0.0"))
	float AsteroidLifetime;

	FTimerHandle SpawnTimer;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Spawner")
	FVector GetRandomSpawnPoint() const;

	virtual void Tick(float DeltaTime) override;

private:
	void SpawnActors();
	void StartSpawnTimer();
	void ConfigureSpawnedAsteroid(AAsteroidPickUp* Asteroid) const;

	float RandomSpawnDelay;
};
