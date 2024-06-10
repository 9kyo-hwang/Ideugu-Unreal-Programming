// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABWeaponItemData.h"

UABWeaponItemData::UABWeaponItemData()
{
	// 모든 아이템은 생성자에서 타입을 설정해야 함
	Type = EItemType::Weapon;
}
