#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

UCLASS()
class SPARTAPROJECT_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASpartaGameState();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress")
	int32 CurrentLevelIndex;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress")
	int32 CurrentWaveIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress")
	float CurrentWaveTimeLimit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress")
	float WaveEndTime;

	FTimerHandle HUDUpdateTimerHandle;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void OnGameOver();

	void InitializeProgress(int32 LevelIndex);
	void BeginWaveState(int32 WaveIndex, float TimeLimit);
	void SetSpawnedCoinCount(int32 CoinCount);
	void OnCoinCollected();
	void UpdateHUD();

	UFUNCTION(BlueprintPure, Category = "Progress")
	float GetWaveTimeRemaining() const;
};