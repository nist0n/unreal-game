#include "Quest/QuestMarkerComponent.h"

#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Quest/QuestMarkerTarget.h"
#include "QuestSystem/Components/QuestComponent.h"
#include "QuestSystem/Gameplay/QuestSystem/Interfaces/QuestInterface.h"
#include "QuestSystem/Gameplay/QuestSystem/Quest.h"

UQuestMarkerComponent::UQuestMarkerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bDrawGuideLines = true;
	GuideLineThickness = 8.0f;
}

void UQuestMarkerComponent::BeginPlay()
{
	Super::BeginPlay();

	DiscoverMarkerTargets();

	if (AActor* Owner = GetOwner())
	{
		GuideLineMesh = NewObject<UStaticMeshComponent>(Owner, TEXT("QuestGuideLine"));
		if (GuideLineMesh)
		{
			GuideLineMesh->RegisterComponent();
			GuideLineMesh->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			GuideLineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GuideLineMesh->SetCastShadow(false);
			GuideLineMesh->SetHiddenInGame(true);

			if (UStaticMesh* Cylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder")))
			{
				GuideLineMesh->SetStaticMesh(Cylinder);
			}
		}
	}

	if (BindToQuestComponent())
	{
		RefreshMarkers();
	}
}

void UQuestMarkerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateGuideLine();
}

void UQuestMarkerComponent::DiscoverMarkerTargets()
{
	MarkerTargets.Reset();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(World, AQuestMarkerTarget::StaticClass(), Found);

	for (AActor* Actor : Found)
	{
		if (AQuestMarkerTarget* Target = Cast<AQuestMarkerTarget>(Actor))
		{
			MarkerTargets.Add(Target);
		}
	}
}

bool UQuestMarkerComponent::BindToQuestComponent()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return false;
	}

	UQuestComponent* QuestComp = nullptr;

	if (Owner->GetClass()->ImplementsInterface(UQuestInterface::StaticClass()))
	{
		QuestComp = IQuestInterface::Execute_GetQuestComponent(Owner);
	}

	if (!IsValid(QuestComp))
	{
		if (APawn* OwnerPawn = Cast<APawn>(Owner))
		{
			if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
			{
				if (PC->GetClass()->ImplementsInterface(UQuestInterface::StaticClass()))
				{
					QuestComp = IQuestInterface::Execute_GetQuestComponent(PC);
				}
			}
		}
	}

	if (!IsValid(QuestComp))
	{
		QuestComp = Owner->FindComponentByClass<UQuestComponent>();
	}

	if (!IsValid(QuestComp))
	{
		return false;
	}

	UnbindFromQuestComponent();
	BoundQuestComponent = QuestComp;
	BoundQuestComponent->OnQuestAdded.AddDynamic(this, &UQuestMarkerComponent::OnQuestAdded);
	BoundQuestComponent->OnQuestStatusChanged.AddDynamic(this, &UQuestMarkerComponent::OnQuestStatusChanged);
	return true;
}

void UQuestMarkerComponent::UnbindFromQuestComponent()
{
	if (IsValid(BoundQuestComponent))
	{
		BoundQuestComponent->OnQuestAdded.RemoveDynamic(this, &UQuestMarkerComponent::OnQuestAdded);
		BoundQuestComponent->OnQuestStatusChanged.RemoveDynamic(this, &UQuestMarkerComponent::OnQuestStatusChanged);
		BoundQuestComponent = nullptr;
	}
}

void UQuestMarkerComponent::OnQuestAdded(UQuest* Quest)
{
	UpdateMarkerForQuest(Quest);
}

void UQuestMarkerComponent::OnQuestStatusChanged(UQuest* Quest, EQuestStatus NewStatus)
{
	UpdateMarkerForQuest(Quest);
}

void UQuestMarkerComponent::UpdateMarkerForQuest(UQuest* Quest)
{
	if (!IsValid(Quest))
	{
		return;
	}

	for (AQuestMarkerTarget* Target : MarkerTargets)
	{
		if (!IsValid(Target))
		{
			continue;
		}

		if (QuestMatchesTarget(Quest, Target))
		{
			Target->SetMarkerVisible(ShouldShowMarkerForQuest(Quest));
		}
	}

	UpdateGuideLine();
}

void UQuestMarkerComponent::UpdateAllMarkers()
{
	if (!IsValid(BoundQuestComponent))
	{
		for (AQuestMarkerTarget* Target : MarkerTargets)
		{
			if (IsValid(Target))
			{
				Target->SetMarkerVisible(false);
			}
		}
		return;
	}

	for (AQuestMarkerTarget* Target : MarkerTargets)
	{
		if (!IsValid(Target) || !Target->GetLinkedQuestClass())
		{
			continue;
		}

		const FGuid TargetId = Target->GetLinkedQuestClass().GetDefaultObject()->GetId();
		const EQuestStatus Status = BoundQuestComponent->GetQuestStatus(TargetId);
		Target->SetMarkerVisible(Status == EQuestStatus::Active);
	}

	UpdateGuideLine();
}

void UQuestMarkerComponent::RefreshMarkers()
{
	DiscoverMarkerTargets();
	if (!BindToQuestComponent())
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestMarkerComponent: QuestComponent not found on owner/controller."));
	}
	UpdateAllMarkers();
}

bool UQuestMarkerComponent::ShouldShowMarkerForQuest(const UQuest* Quest) const
{
	return IsValid(Quest) && Quest->GetStatus() == EQuestStatus::Active;
}

bool UQuestMarkerComponent::QuestMatchesTarget(const UQuest* Quest, const AQuestMarkerTarget* Target) const
{
	if (!IsValid(Quest) || !IsValid(Target) || !Target->GetLinkedQuestClass())
	{
		return false;
	}

	return Quest->GetClass() == Target->GetLinkedQuestClass();
}

void UQuestMarkerComponent::UpdateGuideLine()
{
	if (!bDrawGuideLines || !GuideLineMesh)
	{
		return;
	}

	AActor* Owner = GetOwner();
	APawn* PlayerPawn = nullptr;

	if (const APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		PlayerPawn = const_cast<APawn*>(OwnerPawn);
	}
	else if (APlayerController* PC = Cast<APlayerController>(Owner))
	{
		PlayerPawn = PC->GetPawn();
	}

	if (!IsValid(PlayerPawn))
	{
		GuideLineMesh->SetHiddenInGame(true);
		return;
	}

	AQuestMarkerTarget* ActiveTarget = nullptr;
	float ClosestDistSq = TNumericLimits<float>::Max();

	for (AQuestMarkerTarget* Target : MarkerTargets)
	{
		if (!IsValid(Target) || !Target->IsMarkerVisible() || !Target->bShowGuideLine)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(PlayerPawn->GetActorLocation(), Target->GetActorLocation());
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			ActiveTarget = Target;
		}
	}

	if (!ActiveTarget)
	{
		GuideLineMesh->SetHiddenInGame(true);
		return;
	}

	const FVector Start = PlayerPawn->GetActorLocation();
	const FVector End = ActiveTarget->GetActorLocation();
	const FVector Segment = End - Start;
	const float Length = Segment.Size();

	if (Length < KINDA_SMALL_NUMBER)
	{
		GuideLineMesh->SetHiddenInGame(true);
		return;
	}

	GuideLineMesh->SetHiddenInGame(false);

	if (GuideLineMaterial)
	{
		GuideLineMesh->SetMaterial(0, GuideLineMaterial);
	}

	const FVector Midpoint = (Start + End) * 0.5f;
	const FRotator Rotation = FRotationMatrix::MakeFromZ(Segment.GetSafeNormal()).Rotator();

	GuideLineMesh->SetWorldLocation(Midpoint);
	GuideLineMesh->SetWorldRotation(Rotation);
	GuideLineMesh->SetWorldScale3D(FVector(GuideLineThickness / 100.0f, GuideLineThickness / 100.0f, Length / 100.0f));
}