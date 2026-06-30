#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SpartaGameMode.generated.h"

USTRUCT(BlueprintType)
struct FWaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 WaveIndex = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float TimeLimit = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 ItemSpawnCount = 15;

	FWaveInfo() = default;

	FWaveInfo(int32 InWaveIndex, float InTimeLimit, int32 InItemSpawnCount)
		: WaveIndex(InWaveIndex),
		  TimeLimit(InTimeLimit),
		  ItemSpawnCount(InItemSpawnCount)
	{
	}
};

UCLASS()
class SPARTAPROJECT_API ASpartaGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASpartaGameMode();

	virtual void BeginPlay() override;
	
	void HandleCoinCollected();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave")
	TArray<FWaveInfo> WaveInfos;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

private:
	int32 CurrentWaveArrayIndex;
	bool bWaveInProgress;
	FTimerHandle WaveTimerHandle;

	void EnsureDefaultConfiguration();
	void StartWave();
	void EndCurrentWave();
	void HandleWaveTimeUp();
	void AdvanceToNextLevel();
	void SpawnWaveItems(const FWaveInfo& WaveInfo);
	void ClearSpawnedItems();
};