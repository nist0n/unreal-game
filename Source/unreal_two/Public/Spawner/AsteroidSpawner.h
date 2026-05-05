// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUp/PickUpbase.h"
#include "Components/BoxComponent.h"
#include "AsteroidSpawner.generated.h"

UCLASS()
class UNREAL_TWO_API AAsteroidSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAsteroidSpawner();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawner")
	UBoxComponent* SpawnArea;

	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<APickUpbase> ActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float MinSpawnDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float MaxSpawnDelay;

	FTimerHandle SpawnTimer;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Spawner")
	FVector GetRandomSpawnPoint();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void SpawnActors();

	void StartSpawnTimer();

	float RandomSpawnDelay;
};



