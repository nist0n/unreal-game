#include "Components/TeleportStateComponent.h"

#include "Components/ShipHealthComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UTeleportStateComponent::UTeleportStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ReturnKey = EKeys::R;
	bIsAwayFromBase = false;
	bBaseTransformCached = false;
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

	if (Owner->GetAttachParentActor())
	{
		Owner->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	Owner->SetActorTransform(CachedBaseTransform, false, nullptr, ETeleportType::TeleportPhysics);

	if (bResetShipOnReturnToBase)
	{
		ResetLinkedShips();
	}

	bIsAwayFromBase = false;
	OnShelterStateChanged.Broadcast(false);
}

void UTeleportStateComponent::ResetLinkedShips() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<AActor*> ShipsToReset;

	if (IsValid(LinkedShip))
	{
		ShipsToReset.Add(LinkedShip);
	}
	else if (!ShipSearchTag.IsNone())
	{
		TArray<AActor*> TaggedShips;
		UGameplayStatics::GetAllActorsWithTag(World, ShipSearchTag, TaggedShips);
		ShipsToReset.Append(TaggedShips);
	}
	else
	{
		return;
	}

	if (ShipsToReset.Num() == 0)
	{
		return;
	}

	for (AActor* Ship : ShipsToReset)
	{
		if (!IsValid(Ship))
		{
			continue;
		}

		if (UShipHealthComponent* ShipHealth = Ship->FindComponentByClass<UShipHealthComponent>())
		{
			ShipHealth->ResetToSpawnPosition();
		}
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
