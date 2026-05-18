#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestMarkerTarget.generated.h"

class UQuest;
class UStaticMeshComponent;
class UMaterialInterface;

UCLASS(Blueprintable)
class UNREAL_TWO_API AQuestMarkerTarget : public AActor
{
	GENERATED_BODY()

public:
	AQuestMarkerTarget();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Quest|Marker")
	void SetMarkerVisible(bool bVisible);

	UFUNCTION(BlueprintPure, Category = "Quest|Marker")
	bool IsMarkerVisible() const { return bMarkerVisible; }

	UFUNCTION(BlueprintPure, Category = "Quest|Marker")
	TSubclassOf<UQuest> GetLinkedQuestClass() const { return LinkedQuestClass; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Marker")
	TSubclassOf<UQuest> LinkedQuestClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Marker")
	bool bShowGuideLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Marker")
	float MarkerScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Marker")
	UMaterialInterface* MarkerMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Marker")
	TObjectPtr<UStaticMeshComponent> MarkerMesh;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Quest|Marker")
	bool bMarkerVisible;

	void UpdateBillboardRotation();
};