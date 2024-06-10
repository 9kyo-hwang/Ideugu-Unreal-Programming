// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABWidgetComponent.h"
#include "ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	Super::InitWidget();

	// 위젯에 대한 인스턴스가 생성된 직후. -> 액터 관련 정보를 얻어 OwningActor에 할당
	// 위젯 관련 UI 요소 준비 완료 시 NativeConstruct 함수 호출

	if(auto ABUserWidget = Cast<UABUserWidget>(GetWidget()))
	{
		ABUserWidget->SetOwningActor(GetOwner());  // 컴포넌트이므로 GetOwner를 통해 자신을 소유하고 있는 액터 정보를 얻을 수 있음
	}
}
