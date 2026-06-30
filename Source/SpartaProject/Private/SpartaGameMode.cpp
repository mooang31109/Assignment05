#include "SpartaGameMode.h"
#include "CoinItem.h"
#include "SpawnVolume.h"
#include "SpartaCharacter.h"
#include "SpartaGameInstance.h"
#include "SpartaGameState.h"
#include "SpartaPlayerController.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

ASpartaGameMode::ASpartaGameMode()
	: CurrentWaveArrayIndex(0),
	  bWaveInProgress(false)
{
	DefaultPawnClass = ASpartaCharacter::StaticClass();
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
	GameStateClass = ASpartaGameState::StaticClass();
	
	WaveInfos =
	{
		FWaveInfo(1, 30.0f, 15),
		FWaveInfo(2, 25.0f, 25),
		FWaveInfo(3, 20.0f, 35)
	};

	LevelMapNames =
	{
		TEXT("BasicLevel"),
		TEXT("IntermediateLevel"),
		TEXT("AdvancedLevel")
	};
}

void ASpartaGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld()->GetMapName().Contains(TEXT("MenuLevel")))
	{
		return;
	}

	EnsureDefaultConfiguration();

	ASpartaGameState* SpartaGameState = GetGameState<ASpartaGameState>();
	USpartaGameInstance* SpartaGameInstance = GetGameInstance<USpartaGameInstance>();
	if (!SpartaGameState || !SpartaGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Wave system could not find GameState or GameInstance."));
		return;
	}

	const int32 LevelIndex = FMath::Clamp(SpartaGameInstance->CurrentLevelIndex, 0, 2);
	SpartaGameState->InitializeProgress(LevelIndex);
	CurrentWaveArrayIndex = 0;
	StartWave();
}

void ASpartaGameMode::EnsureDefaultConfiguration()
{
	if (WaveInfos.Num() < 3)
	{
		WaveInfos =
		{
			FWaveInfo(1, 30.0f, 15),
			FWaveInfo(2, 25.0f, 25),
			FWaveInfo(3, 20.0f, 35)
		};
	}

	if (LevelMapNames.Num() < 3)
	{
		LevelMapNames =
		{
			TEXT("BasicLevel"),
			TEXT("IntermediateLevel"),
			TEXT("AdvancedLevel")
		};
	}
}

void ASpartaGameMode::StartWave()
{
	if (!WaveInfos.IsValidIndex(CurrentWaveArrayIndex))
	{
		AdvanceToNextLevel();
		return;
	}

	ASpartaGameState* SpartaGameState = GetGameState<ASpartaGameState>();
	if (!SpartaGameState)
	{
		return;
	}

	const FWaveInfo& WaveInfo = WaveInfos[CurrentWaveArrayIndex];
	bWaveInProgress = true;
	SpartaGameState->BeginWaveState(WaveInfo.WaveIndex, WaveInfo.TimeLimit);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Wave %d 시작!"), WaveInfo.WaveIndex)
		);
	}

	SpawnWaveItems(WaveInfo);

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameMode::HandleWaveTimeUp,
		WaveInfo.TimeLimit,
		false
	);
}

void ASpartaGameMode::SpawnWaveItems(const FWaveInfo& WaveInfo)
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No SpawnVolume was found for Wave %d."), WaveInfo.WaveIndex);
		return;
	}

	int32 SpawnedCoinCount = 0;
	for (int32 ItemIndex = 0; ItemIndex < WaveInfo.ItemSpawnCount; ++ItemIndex)
	{
		ASpawnVolume* SpawnVolume =
			Cast<ASpawnVolume>(FoundVolumes[ItemIndex % FoundVolumes.Num()]);

		if (SpawnVolume)
		{
			AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
			if (SpawnedActor && SpawnedActor->IsA<ACoinItem>())
			{
				++SpawnedCoinCount;
			}
		}
	}

	if (ASpartaGameState* SpartaGameState = GetGameState<ASpartaGameState>())
	{
		SpartaGameState->SetSpawnedCoinCount(SpawnedCoinCount);
	}
}

void ASpartaGameMode::HandleCoinCollected()
{
	if (!bWaveInProgress)
	{
		return;
	}

	const ASpartaGameState* SpartaGameState = GetGameState<ASpartaGameState>();
	if (SpartaGameState &&
		SpartaGameState->SpawnedCoinCount > 0 &&
		SpartaGameState->CollectedCoinCount >= SpartaGameState->SpawnedCoinCount)
	{
		EndCurrentWave();
	}
}

void ASpartaGameMode::HandleWaveTimeUp()
{
	EndCurrentWave();
}

void ASpartaGameMode::EndCurrentWave()
{
	if (!bWaveInProgress)
	{
		return;
	}

	bWaveInProgress = false;
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	ClearSpawnedItems();

	++CurrentWaveArrayIndex;
	if (WaveInfos.IsValidIndex(CurrentWaveArrayIndex))
	{
		StartWave();
	}
	else
	{
		AdvanceToNextLevel();
	}
}

void ASpartaGameMode::ClearSpawnedItems()
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	for (AActor* FoundActor : FoundVolumes)
	{
		if (ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundActor))
		{
			SpawnVolume->DestroySpawnedItems();
		}
	}
}

void ASpartaGameMode::AdvanceToNextLevel()
{
	ASpartaGameState* SpartaGameState = GetGameState<ASpartaGameState>();
	USpartaGameInstance* SpartaGameInstance = GetGameInstance<USpartaGameInstance>();
	if (!SpartaGameState || !SpartaGameInstance)
	{
		return;
	}

	const int32 NextLevelIndex = SpartaGameState->CurrentLevelIndex + 1;
	if (NextLevelIndex >= 3)
	{
		SpartaGameState->OnGameOver();
		return;
	}

	if (!LevelMapNames.IsValidIndex(NextLevelIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("LevelMapNames has no entry for index %d."), NextLevelIndex);
		SpartaGameState->OnGameOver();
		return;
	}
	
	SpartaGameInstance->CurrentLevelIndex = NextLevelIndex;
	UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[NextLevelIndex]);
}