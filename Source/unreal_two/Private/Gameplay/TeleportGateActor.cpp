#include "Gameplay/TeleportGateActor.h"

#include "Components/BoxComponent.h"
#include "Components/TeleportStateComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ATeleportGateActor::ATeleportGateActor()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	SetRootComponent(InteractionVolume);
	InteractionVolume->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	bTeleportToDestination = true;
	bEnableReturnToBase = true;
}

void ATeleportGateActor::HandleInteract(AActor* Interactor)
{
	APawn* InteractorPawn = nullptr;

	if (IsValid(Interactor))
	{
		InteractorPawn = Cast<APawn>(Interactor);
		if (!IsValid(InteractorPawn))
		{
			InteractorPawn = Cast<APawn>(Interactor->GetOwner());
		}
	}

	if (!IsValid(InteractorPawn))
	{
		if (UWorld* World = GetWorld())
		{
			InteractorPawn = UGameplayStatics::GetPlayerPawn(World, 0);
		}
	}

	if (!IsValid(InteractorPawn))
	{
		return;
	}

	UTeleportStateComponent* TeleportState = InteractorPawn->FindComponentByClass<UTeleportStateComponent>();
	if (!IsValid(TeleportState))
	{
		return;
	}

	if (bTeleportToDestination)
	{
		TeleportState->TeleportToLocation(ResolveDestination(), bEnableReturnToBase);
	}
	else
	{
		TeleportState->ReturnToBase();
	}
}

FTransform ATeleportGateActor::ResolveDestination() const
{
	if (IsValid(DestinationActor))
	{
		return DestinationActor->GetActorTransform();
	}

	return DestinationTransform;
}
