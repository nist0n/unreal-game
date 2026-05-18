
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpbase.generated.h"

UCLASS()
class UNREAL_TWO_API APickUpbase : public AActor
{
	GENERATED_BODY()
	
public:	
	APickUpbase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Mesh")
	UStaticMeshComponent* PickUpMeshComponent;

	UFUNCTION(BlueprintPure, Category = "PickUpbase")
	bool IsPickUpActive() const;

	UFUNCTION(BlueprintCallable, Category = "PickUpbase")
	void SetPickUpActive(bool state);

protected:
	virtual void BeginPlay() override;

	bool bIsActive;

public:	
	virtual void Tick(float DeltaTime) override;

};
