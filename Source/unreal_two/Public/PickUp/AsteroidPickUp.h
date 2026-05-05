// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpbase.h"
#include "GameFramework/Actor.h"
#include "AsteroidPickUp.generated.h"

UCLASS()
class UNREAL_TWO_API AAsteroidPickUp : public APickUpbase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroidPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
