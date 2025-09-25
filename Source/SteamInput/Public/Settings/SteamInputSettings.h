// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SteamInputTypes.h"
#include "UObject/Object.h"
#include "SteamInputSettings.generated.h"

UENUM()
enum class EKeyType : uint8
{
	Button,
	Analog,
	Joystick,
	MouseInput
};

USTRUCT()
struct FSteamInputAction
{
	GENERATED_BODY()

	UPROPERTY(Config, EditAnywhere, Category = "Steam Input")
	FName ActionName;
	
	UPROPERTY(Config, EditAnywhere, Category = "Steam Input")
	EKeyType KeyType = EKeyType::Button;
	
	bool bHandleValid = false;

	ControllerActionHandle_t CachedHandle = 0;

	FSteamInputAction() = default;
	FSteamInputAction(const FName& KeyName, const EKeyType KeyType) : ActionName(KeyName), KeyType(KeyType)
	{
		GenerateHandle();
	}

	bool GenerateHandle();

	void GenerateKey(bool RefreshHandle = false);

#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
	{
		GenerateHandle();
		GenerateKey();
	}
#endif
};

/**
 * 
 */
UCLASS(config = Input, DefaultConfig)
class STEAMINPUT_API USteamInputSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Actions", meta = (ForceInlineRow = true))
	TArray<FSteamInputAction> Keys;
	
	static const FName MenuCategory;
	
	static FName GetXAxisName(const FName Name);

	static FName GetYAxisName(const FName Name);

	void RefreshHandles();

	UPROPERTY(VisibleAnywhere)
	int AppID;
private:
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;

	UFUNCTION()
	void SteamInputInitialized();
	
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};
