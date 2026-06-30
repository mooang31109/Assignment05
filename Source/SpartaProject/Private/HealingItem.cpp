// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"
#include "SpartaCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20.f;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* Playercharacter = Cast<ASpartaCharacter>(Activator))
		{
			Playercharacter->AddHealth(HealAmount);
		}
		DestroyItem();
	}
}

