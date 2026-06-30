#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class SPARTAPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ASpawnVolume();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomItem();

	// 웨이브가 끝났을 때 남아 있는 아이템을 제거합니다.
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void DestroySpawnedItems();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	int32 CurrentSpawnCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 MaxSpawnCount = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<AActor*> SpawnedItems;

	FVector GetRandomPointInVolume() const;
	FItemSpawnRow* GetRandomItem() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);
};