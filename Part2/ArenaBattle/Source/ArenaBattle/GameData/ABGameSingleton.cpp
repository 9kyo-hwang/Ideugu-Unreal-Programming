// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/ABGameSingleton.h"

DEFINE_LOG_CATEGORY(LogABGameSingleton)

UABGameSingleton::UABGameSingleton()
{
	// TODO: 엑셀 테이블에서 만든 에셋에 접근해 TArray로 변환해 저장

	// 기본적으로 데이터 테이블은 (Key, Value) 쌍으로 구성됨. FName값을 가지고 접근할 수 있음
	// 레벨 정보는 순차적으로 바로 저장이 되기 때문에 Key를 갖지 않고 바로 TArray로 저장
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Script/Engine.DataTable'/Game/ArenaBattle/GameData/ABCharacterStatTable.ABCharacterStatTable'"));
	if(DataTableRef.Object)
	{
		const UDataTable* DataTable = DataTableRef.Object;
		check(DataTable->GetRowMap().Num() > 0);

		TArray<uint8*> ValueArray;
		DataTable->GetRowMap().GenerateValueArray(ValueArray);
		// Value 값만 가져와서 저장함
		Algo::Transform(ValueArray, CharacterStatTable,
			[](uint8* Value)
			{
				return *reinterpret_cast<FABCharacterStat*>(Value);
			});
	}

	CharacterMaxLevel = CharacterStatTable.Num();
	ensure(CharacterMaxLevel > 0);
}

UABGameSingleton& UABGameSingleton::Get()
{
	// 싱글톤은 엔진이 초기화될 때 활성화되는 것이 보장됨. CastChecked로 검사
	// 이를 받아오는 것은 GEngine이라는 전역 변수를 통해 접근 가능
	if(UABGameSingleton* Singleton = CastChecked<UABGameSingleton>(GEngine->GameSingleton))
	{
		return *Singleton;
	}

	// 여기에 접근하는 건 잘못된 것 -> 로그 출력
	UE_LOG(LogABGameSingleton, Error, TEXT("Invalid Game Singleton"));
	return *NewObject<UABGameSingleton>();
}