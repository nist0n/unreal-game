#include "Components/TeleportStateComponent.h"

#include "Components/ShipHealthComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

UTeleportStateComponent::UTeleportStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ReturnKey = EKeys::R;
	ReturnHintText = NSLOCTEXT("Teleport", "ReturnHint", "Press R to return to base");
	OnScreenHintDuration = 0.0f;
	bIsAwayFromBase = false;
	bBaseTransformCached = false;
	OnScreenHintKey = 88001;
	bResetShipOnReturnToBase = true;
	ShipSearchTag = FName(TEXT("PlayerShip"));
}

void UTeleportStateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsAwayFromBase)
	{
		PollReturnInput();
	}
}

void UTeleportStateComponent::CacheBaseTransform()
{
	if (const AActor* Owner = GetOwner())
	{
		CachedBaseTransform = Owner->GetActorTransform();
		bBaseTransformCached = true;
	}
}

void UTeleportStateComponent::TeleportToLocation(const FTransform& Destination, bool bMarkAsAwayFromBase)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	if (!bBaseTransformCached)
	{
		CacheBaseTransform();
	}

	Owner->SetActorTransform(Destination, false, nullptr, ETeleportType::TeleportPhysics);

	if (bMarkAsAwayFromBase && !bIsAwayFromBase)
	{
		bIsAwayFromBase = true;
		OnShelterStateChanged.Broadcast(true);
		ShowReturnHint();
	}
}

void UTeleportStateComponent::ReturnToBase()
{
	if (!bIsAwayFromBase || !bBaseTransformCached)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Сначала открепляем персонажа от корабля, если прикреплен
	if (Owner->GetAttachParentActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is attached to %s, detaching..."), 
			*Owner->GetAttachParentActor()->GetName());
		Owner->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	// Телепортируем персонажа на базу
	Owner->SetActorTransform(CachedBaseTransform, false, nullptr, ETeleportType::TeleportPhysics);

	// Сбрасываем корабли
	if (bResetShipOnReturnToBase)
	{
		ResetLinkedShips();
	}

	bIsAwayFromBase = false;
	OnShelterStateChanged.Broadcast(false);
	HideReturnHint();
}

void UTeleportStateComponent::ResetLinkedShips() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("ResetLinkedShips: World is null"));
		return;
	}

	TArray<AActor*> ShipsToReset;

	if (IsValid(LinkedShip))
	{
		ShipsToReset.Add(LinkedShip);
		UE_LOG(LogTemp, Warning, TEXT("ResetLinkedShips: Using direct reference to %s"), *LinkedShip->GetName());
	}
	else if (!ShipSearchTag.IsNone())
	{
		TArray<AActor*> TaggedShips;
		UGameplayStatics::GetAllActorsWithTag(World, ShipSearchTag, TaggedShips);
		ShipsToReset.Append(TaggedShips);
        
		UE_LOG(LogTemp, Warning, TEXT("ResetLinkedShips: Found %d ships with tag '%s'"), 
			TaggedShips.Num(), *ShipSearchTag.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ResetLinkedShips: No LinkedShip and ShipSearchTag is None!"));
		return;
	}

	if (ShipsToReset.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ResetLinkedShips: No ships to reset! Check if ship exists and has correct tag."));
		return;
	}

	for (AActor* Ship : ShipsToReset)
	{
		if (!IsValid(Ship))
		{
			UE_LOG(LogTemp, Warning, TEXT("ResetLinkedShips: Ship is invalid, skipping"));
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("ResetLinkedShips: Found ship %s, checking for UShipHealthComponent"), *Ship->GetName());

		if (UShipHealthComponent* ShipHealth = Ship->FindComponentByClass<UShipHealthComponent>())
		{
			ShipHealth->ResetToSpawnPosition();
			UE_LOG(LogTemp, Warning, TEXT("ResetLinkedShips: Called ResetToSpawnPosition on %s"), *Ship->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ResetLinkedShips: Ship %s doesn't have UShipHealthComponent!"), *Ship->GetName());
		}
	}
}

void UTeleportStateComponent::ShowReturnHint() const
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(OnScreenHintKey, OnScreenHintDuration, FColor::Cyan,
			ReturnHintText.ToString(), true, FVector2D(1.2f, 1.2f));
	}
}

void UTeleportStateComponent::HideReturnHint() const
{
	if (GEngine)
	{
		GEngine->RemoveOnScreenDebugMessage(OnScreenHintKey);
	}
}

void UTeleportStateComponent::PollReturnInput()
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!IsValid(OwnerPawn))
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!IsValid(PC))
	{
		return;
	}

	if (PC->WasInputKeyJustPressed(ReturnKey))
	{
		ReturnToBase();
	}
}
