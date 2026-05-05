// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpbase.generated.h"

UCLASS()
class UNREAL_TWO_API APickUpbase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpbase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Mesh")
	UStaticMeshComponent* PickUpMeshComponent;

	UFUNCTION(BlueprintPure, Category = "PickUpbase")
	bool IsPickUpActive() const;

	UFUNCTION(BlueprintCallable, Category = "PickUpbase")
	void SetPickUpActive(bool state);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsActive;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
