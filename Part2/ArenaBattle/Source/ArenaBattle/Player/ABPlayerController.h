// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AABPlayerController();
	
protected:
	virtual void BeginPlay() override;

	// HUD Section
protected:
	// 위젯 생성 시 클래스 정보를 넘겨야 함 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HUD)
	TSubclassOf<class UABHUDWidget> ABHUDWidgetClass;

	// 생성할 위젯을 담는 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=HUD)
	TObjectPtr<class UABHUDWidget> ABHUDWidget;
};
