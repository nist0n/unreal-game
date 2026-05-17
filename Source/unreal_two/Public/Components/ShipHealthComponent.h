#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShipHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShipHealthChanged, int32, CurrentHealth, int32, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShipDamageFlash);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShipDestroyed);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREAL_TWO_API UShipHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShipHealthComponent();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Ship")
	FTransform InitialWorldTransform;

	UFUNCTION(BlueprintCallable, Category = "Ship|Health")
	void ApplyAsteroidHit(int32 DamageAmount = 1);

	UFUNCTION(BlueprintCallable, Category = "Ship|Health")
	void ResetHealth();
	
	UFUNCTION(BlueprintCallable, Category = "Ship|Health")
	void ResetToSpawnPosition();

	UFUNCTION(BlueprintPure, Category = "Ship|Health")
	int32 GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Ship|Health")
	int32 GetMaxHealth() const { return MaxHealth; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Health")
	int32 MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Health|FX")
	bool bUseCameraFlashOnDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Health|FX")
	float DamageFlashPeakAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Health|FX")
	float DamageFlashFadeInTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Health|FX")
	float DamageFlashFadeOutTime;

	UPROPERTY(BlueprintAssignable, Category = "Ship|Health")
	FOnShipHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Ship|Health")
	FOnShipDamageFlash OnDamageFlash;

	UPROPERTY(BlueprintAssignable, Category = "Ship|Health")
	FOnShipDestroyed OnShipDestroyed;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Ship|Health")
	int32 CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Ship|Health")
	FTransform InitialTransform;

	UPROPERTY(BlueprintReadOnly, Category = "Ship|Health")
	bool bInitialTransformCached;

	void HandleDeath();

	void ConfigureShipOverlapCollision();
	void PlayDamageFlash();
	void EndDamageFlash();

	FTimerHandle DamageFlashTimerHandle;

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> ShipMeshComponent;
    
	FTransform InitialMeshTransform;
};