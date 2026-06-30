#include "SpartaGameState.h"
#include "SpartaCharacter.h"
#include "SpartaGameInstance.h"
#include "SpartaGameMode.h"
#include "SpartaPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

ASpartaGameState::ASpartaGameState()
	: Score(0),
	  SpawnedCoinCount(0),
	  CollectedCoinCount(0),
	  CurrentLevelIndex(0),
	  CurrentWaveIndex(0),
	  CurrentWaveTimeLimit(0.0f),
	  WaveEndTime(0.0f)
{
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetMapName().Contains(TEXT("MenuLevel")))
	{
		return;
	}

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController =
			Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (USpartaGameInstance* SpartaGameInstance = GetGameInstance<USpartaGameInstance>())
	{
		SpartaGameInstance->AddToScore(Amount);
	}
}

void ASpartaGameState::InitializeProgress(int32 LevelIndex)
{
	CurrentLevelIndex = LevelIndex;
	CurrentWaveIndex = 0;
	CurrentWaveTimeLimit = 0.0f;
	WaveEndTime = 0.0f;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
}

void ASpartaGameState::BeginWaveState(int32 WaveIndex, float TimeLimit)
{
	CurrentWaveIndex = WaveIndex;
	CurrentWaveTimeLimit = TimeLimit;
	WaveEndTime = GetServerWorldTimeSeconds() + TimeLimit;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	UpdateHUD();
}

void ASpartaGameState::SetSpawnedCoinCount(int32 CoinCount)
{
	SpawnedCoinCount = CoinCount;
}

void ASpartaGameState::OnCoinCollected()
{
	++CollectedCoinCount;
	
	if (ASpartaGameMode* SpartaGameMode = GetWorld()->GetAuthGameMode<ASpartaGameMode>())
	{
		SpartaGameMode->HandleCoinCollected();
	}
}

float ASpartaGameState::GetWaveTimeRemaining() const
{
	return FMath::Max(0.0f, WaveEndTime - GetServerWorldTimeSeconds());
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController =
			Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	ASpartaPlayerController* SpartaPlayerController =
		Cast<ASpartaPlayerController>(PlayerController);
	if (!SpartaPlayerController)
	{
		return;
	}

	UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget();
	if (!HUDWidget)
	{
		return;
	}

	if (UTextBlock* TimeText =
		Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
	{
		TimeText->SetText(FText::FromString(
			FString::Printf(TEXT("Time: %.1f"), GetWaveTimeRemaining())
		));
	}

	if (UTextBlock* ScoreText =
		Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
	{
		if (USpartaGameInstance* SpartaGameInstance =
			GetGameInstance<USpartaGameInstance>())
		{
			ScoreText->SetText(FText::FromString(
				FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)
			));
		}
	}

	if (UTextBlock* LevelText =
		Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
	{
		LevelText->SetText(FText::FromString(
			FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)
		));
	}
	
	if (UTextBlock* WaveText =
		Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
	{
		WaveText->SetText(FText::FromString(
			FString::Printf(TEXT("Wave: %d"), CurrentWaveIndex)
		));
	}

	if (UUserWidget* HPWidget =
		Cast<UUserWidget>(HUDWidget->GetWidgetFromName(TEXT("WBP_HP"))))
	{
		if (UTextBlock* HPText =
			Cast<UTextBlock>(HPWidget->GetWidgetFromName(TEXT("OverHeadHP"))))
		{
			if (ASpartaCharacter* Character =
				Cast<ASpartaCharacter>(SpartaPlayerController->GetPawn()))
			{
				HPText->SetText(FText::FromString(
					FString::Printf(
						TEXT("%.0f / %.0f"),
						static_cast<float>(Character->GetHealth()),
						Character->GetMaxHealth()
					)
				));
			}
		}
	}
}