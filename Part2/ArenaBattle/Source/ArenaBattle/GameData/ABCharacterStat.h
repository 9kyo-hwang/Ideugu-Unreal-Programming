#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ABcharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FABCharacterStat : public FTableRowBase  // 상속 필요 -> Engine/DataTable.h 필요
{
	GENERATED_BODY()

	FABCharacterStat() : MaxHp(0), Attack(0), AttackRange(0), AttackSpeed(0), MovementSpeed(0) {}

	// 모든 속성 EditAnywhere & float
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stat)
	float MaxHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stat)
	float Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stat)
	float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stat)
	float AttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stat)
	float MovementSpeed;

	FABCharacterStat operator+(const FABCharacterStat& Other) const
	{
		// 모든 타입이 float으로 통일되어있기 때문에
		const float* const Lhs = reinterpret_cast<const float* const>(this);
		const float* const Rhs = reinterpret_cast<const float* const>(&Other);

		FABCharacterStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);

		// float 타입에 대해 개수만 정의해 더하는 식으로 정의하면
		int32 StatNum = sizeof(FABCharacterStat) / sizeof(float);
		for(int32 i = 0; i < StatNum; ++i)
		{
			ResultPtr[i] = Lhs[i] + Rhs[i];
		}

		// 나중에 멤버 변수가 변경되어도 이 코드는 건드릴 필요 없음
		return Result;
	}
};