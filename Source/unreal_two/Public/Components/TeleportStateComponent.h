#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeleportStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShelterStateChanged, bool, bIsInShelter);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREAL_TWO_API UTeleportStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTeleportStateComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category = "Teleport")
	void CacheBaseTransform();
	
	UFUNCTION(BlueprintCallable, Category = "Teleport")
	void TeleportToLocation(const FTransform& Destination, bool bMarkAsAwayFromBase = true);
	
	UFUNCTION(BlueprintCallable, Category = "Teleport")
	void ReturnToBase();

	UFUNCTION(BlueprintPure, Category = "Teleport")
	bool IsAwayFromBase() const { return bIsAwayFromBase; }

	UPROPERTY(BlueprintAssignable, Category = "Teleport")
	FOnShelterStateChanged OnShelterStateChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Input")
	FKey ReturnKey;

	UPROPERTY(BlueprintReadOnly, Category = "Teleport")
	FTransform CachedBaseTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Ship")
	TObjectPtr<AActor> LinkedShip;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Ship")
	FName ShipSearchTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Ship")
	bool bResetShipOnReturnToBase;

protected:
	void ResetLinkedShips() const;
	UPROPERTY(BlueprintReadOnly, Category = "Teleport")
	bool bIsAwayFromBase;

	UPROPERTY(BlueprintReadOnly, Category = "Teleport")
	bool bBaseTransformCached;

	void PollReturnInput();
};
