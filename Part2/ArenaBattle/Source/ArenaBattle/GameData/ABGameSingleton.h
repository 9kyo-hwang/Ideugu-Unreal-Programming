// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameData/ABCharacterStat.h"
#include "ABGameSingleton.generated.h"

// 게임 내 중요한 데이터의 중요한 요소에 영향을 미치므로 Error
DECLARE_LOG_CATEGORY_EXTERN(LogABGameSingleton, Error, All);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UABGameSingleton();
	// 엑셀 데이터 테이블을 로드하는 기능 추가
	static UABGameSingleton& Get();  // 싱글톤 객체를 전역에서 접근할 수 있도록 Get 함수 정의

	// Character Stat Data Section
	// 들어온 Level에서 1을 빼도록 설정
	FORCEINLINE FABCharacterStat GetCharacterStat(int32 InLevel) const { return CharacterStatTable.IsValidIndex(InLevel - 1) ? CharacterStatTable[InLevel - 1] : FABCharacterStat(); }

	UPROPERTY()
	int32 CharacterMaxLevel;  // 총 몇 개의 레벨이 있는가.
	
private:
	TArray<FABCharacterStat> CharacterStatTable;  // 스탯 테이블
};
