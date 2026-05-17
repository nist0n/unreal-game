#pragma once

#include "CoreMinimal.h"
#include "PickUpbase.h"
#include "AsteroidPickUp.generated.h"

UCLASS()
class UNREAL_TWO_API AAsteroidPickUp : public APickUpbase
{
	GENERATED_BODY()

public:
	AAsteroidPickUp();

	UFUNCTION(BlueprintCallable, Category = "Asteroid")
	void LaunchInDirection(FVector Direction, float Speed);

	UFUNCTION(BlueprintCallable, Category = "Asteroid")
	void SetLifetimeSeconds(float Seconds);

protected:
	virtual void BeginPlay() override;

	void ConfigureMeteorOverlapCollision();

	UFUNCTION()
	void OnAsteroidOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Asteroid")
	bool bSimulatePhysicsOnSpawn;

	UPROPERTY(EditAnywhere, Category = "Asteroid")
	bool bDestroyOnShipHit;

	UPROPERTY(EditAnywhere, Category = "Asteroid")
	int32 ShipDamageAmount;

	UPROPERTY(EditAnywhere, Category = "Asteroid")
	float HitCooldownSeconds;

	float LastHitTimeSeconds;
};
