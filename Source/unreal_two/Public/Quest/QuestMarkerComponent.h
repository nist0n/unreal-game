#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestSystem/DataTypes/Enums.h"
#include "QuestMarkerComponent.generated.h"

class UQuest;
class UQuestComponent;
class AQuestMarkerTarget;
class UStaticMeshComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREAL_TWO_API UQuestMarkerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestMarkerComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Quest|Marker")
	void RefreshMarkers();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Marker")
	bool bDrawGuideLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Marker")
	float GuideLineThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Marker")
	UMaterialInterface* GuideLineMaterial;

protected:
	UPROPERTY()
	TArray<TObjectPtr<AQuestMarkerTarget>> MarkerTargets;

	UPROPERTY()
	TObjectPtr<UQuestComponent> BoundQuestComponent;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> GuideLineMesh;

	void DiscoverMarkerTargets();
	bool BindToQuestComponent();
	void UnbindFromQuestComponent();

	UFUNCTION()
	void OnQuestAdded(UQuest* Quest);

	UFUNCTION()
	void OnQuestStatusChanged(UQuest* Quest, EQuestStatus NewStatus);

	void UpdateMarkerForQuest(UQuest* Quest);
	void UpdateAllMarkers();
	bool ShouldShowMarkerForQuest(const UQuest* Quest) const;
	bool QuestMatchesTarget(const UQuest* Quest, const AQuestMarkerTarget* Target) const;

	void UpdateGuideLine();
};