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

	UPROPERTY(EditAnywhere, Category = "Steam Input")
	EKeyType KeyType = EKeyType::Button;

	UPROPERTY(VisibleAnywhere, Category = "Steam Input", meta = (DisplayName = "Handle"))
	ControllerActionHandle_t CachedHandle = 0;

	UPROPERTY(VisibleAnywhere, Category = "Steam Input")
	bool bHandleValid = false;

	FSteamInputAction() = default;
	FSteamInputAction(EKeyType InKeyType) : KeyType(InKeyType) {}

	bool Update(FName KeyName);
};

/**
 * 
 */
UCLASS(config = Input, DefaultConfig)
class STEAMINPUT_API USteamInputSettings : public UObject
{
	GENERATED_BODY()

public:
	USteamInputSettings();

	UPROPERTY(Config, EditAnywhere, Category = "Actions", meta = (ForceInlineRow = true))
	TMap<FName, FSteamInputAction> Keys;

	UPROPERTY(Config, EditAnywhere, Category = "Actions")
	FName MenuCategory = "SteamBindings";

	static void RegenerateKeys();

	UFUNCTION(CallInEditor = true, Category = "Actions")
	void RefreshHandles();
	
	static FName GetXAxisName(const FName Name);

	static FName GetYAxisName(const FName Name);

	UPROPERTY(Config, EditAnywhere, Category = "Slate Input")
	TMap<FKey, EUINavigation> KeyEventRules;

	UPROPERTY(Config, EditAnywhere, Category = "Slate Input")
	TMap<FKey, EUINavigationAction> KeyActionRules;

	static void ApplySlateConfig();

	UFUNCTION()
	static TArray<FName> GetFSteamKeysOptions();

private:
	virtual void PostInitProperties() override;
	void GenerateKey(FName ActionName, EKeyType KeyType) const;

	void UpdateAllHandles();
	
#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
