#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportGateActor.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class UNREAL_TWO_API ATeleportGateActor : public AActor
{
	GENERATED_BODY()

public:
	ATeleportGateActor();

	UFUNCTION(BlueprintCallable, Category = "Teleport")
	void HandleInteract(AActor* Interactor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	AActor* DestinationActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	FTransform DestinationTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	bool bTeleportToDestination;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	bool bEnableReturnToBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Teleport")
	TObjectPtr<UBoxComponent> InteractionVolume;

protected:
	FTransform ResolveDestination() const;
};
