// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABStageGimmick.generated.h"

DECLARE_DELEGATE(FOnStageChangedDelegate);
USTRUCT(BlueprintType)
struct FOnStageChangedDelegateWrapper
{
	GENERATED_BODY()

	FOnStageChangedDelegateWrapper() {}
	FOnStageChangedDelegateWrapper(const FOnStageChangedDelegate& InDelegate) : StageDelegate(InDelegate) {}
	FOnStageChangedDelegate StageDelegate;
};

UENUM(BlueprintType)
enum class EStageState : uint8
{
	Ready = 0,
	Fight,
	Reward,
	Next
};

UCLASS()
class ARENABATTLE_API AABStageGimmick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABStageGimmick();

	FORCEINLINE int32 GetStageNum() const { return CurrentStageNum; }
	FORCEINLINE void SetStageNum(int32 NewStageNum) { CurrentStageNum = NewStageNum; }

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	// Stage Section
protected:
	UPROPERTY(VisibleAnywhere, Category=Stage, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UStaticMeshComponent> Stage;  // root component.

	UPROPERTY(VisibleAnywhere, Category=Stage, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UBoxComponent> StageTrigger;  // player가 들어왔을 때 감지할 수 있는 트리거. 스테이지 중앙에 생성. 

	UFUNCTION()  // 플레이어와 스테이지 트리거의 오버랩 이벤트 발동 시 받아줄 함수.
	void OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Gate Section
protected:
	UPROPERTY(VisibleAnywhere, Category=Gate, meta=(AllowPrivateAccess="true"))
	TMap<FName, TObjectPtr<class UStaticMeshComponent>> Gates;  // Gate는 4개가 필요하므로, 고유 Key를 전달해 구분

	UPROPERTY(VisibleAnywhere, Category=Gate, meta=(AllowPrivateAccess="true"))
	TArray<TObjectPtr<class UBoxComponent>> GateTriggers;  // 각 문마다 트리거 배치.

	UFUNCTION()  // 플레이어와 스테이지 트리거의 오버랩 이벤트 발동 시 받아줄 함수.
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OpenAllGates();
	void CloseAllGates();
	
	// Stage Section
protected:
	UPROPERTY(EditAnywhere, Category=Stage, meta=(AllowPrivateAccess="true"))
	EStageState CurrentStage;
	void SetState(EStageState NewState);  // 상태 변경은 반드시 Setter를 통해서만 이루어지도록.

	UPROPERTY()
	TMap<EStageState, FOnStageChangedDelegateWrapper> StateChangeActions;

	void SetReady();
	void SetFight();
	void SetChooseReward();
	void SetChooseNext();

	// Fight Section
protected:
	UPROPERTY(EditAnywhere, Category=Fight, meta=(AllowPrivateAccess="true"))
	TSubclassOf<class AABCharacterNonPlayer> OpponentClass;  // 대전할 NPC 생성을 위해 Class 지정. 단, AABCharacterNonPlayer의 자식 클래스만 표시
	UPROPERTY(EditAnywhere, Category=Fight, meta=(AllowPrivateAccess="true"))
	float OpponentSpawnTime;  // 스폰에 딜레이를 주기 위해

	UFUNCTION()
	void OnOpponentDestroyed(AActor* DestroyedActor);  // NPC 사망 시 보상 단계로 진행하기 위한 함수

	FTimerHandle OpponentTimerHandle;
	void OnOpponentSpawn();  // NPC 스폰 시 위 함수들을 연동하기 위한 함수

	// Reward Section
protected:
	UPROPERTY(VisibleAnywhere, Category=Reward, meta=(AllowPrivateAccess="true"))
	TSubclassOf<class AABItemBox> RewardBoxClass;  // 박스 클래스
	UPROPERTY(VisibleAnywhere, Category=Reward, meta=(AllowPrivateAccess="true"))
	TArray<TWeakObjectPtr<class AABItemBox>> RewardBoxes;  // 스테이지 기믹 액터와는 무관하게 스스로 소멸할 수 있으므로, GC가 없애버릴 수 있도록 약참조로 선언
	
	TMap<FName, FVector> RewardBoxLocations;  // 박스 이름으로 스폰 위치 저장

	UFUNCTION()  // 오버랩 시 발동하는 함수
	void OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 스폰 함수
	void SpawnRewardBoxes();

	// Stage Stat
protected:
	UPROPERTY(VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	int32 CurrentStageNum;
};
