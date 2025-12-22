// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SteamInputTypes.h"
#include "UObject/Object.h"
#include "Types/SlateEnums.h"
#include "SteamInputSettings.generated.h"

UENUM()
enum class EKeyType : uint8
{
	Button,
	Analog,
	Joystick,
	MouseInput
};

UENUM()
enum class EUINavigationOptions : uint8
{
	/** Four cardinal directions*/
	Left,
	Right,
	Up,
	Down,

	/** Conceptual next and previous*/
	Next,
	Previous,
	
	/** Accept, normally acts like a mouse click. Corresponds to Virtual_Accept key */
	Accept,

	/** Back/Cancel, normally acts like escape. Corresponds to Virtual_Back key */
	Back,

	/** Invalid action */
	Invalid,
};

class UTexture2D;

// Helper struct to handle conversions
struct FNavigationOptionHelper
{
    static EUINavigationOptions FromNavigation(EUINavigation Nav)
    {
        switch (Nav)
        {
            case EUINavigation::Left: return EUINavigationOptions::Left;
            case EUINavigation::Right: return EUINavigationOptions::Right;
            case EUINavigation::Up: return EUINavigationOptions::Up;
            case EUINavigation::Down: return EUINavigationOptions::Down;
            case EUINavigation::Next: return EUINavigationOptions::Next;
            case EUINavigation::Previous: return EUINavigationOptions::Previous;
            default: return EUINavigationOptions::Invalid;
        }
    }
    
    static EUINavigationOptions FromAction(EUINavigationAction Action)
    {
        switch (Action)
        {
            case EUINavigationAction::Accept: return EUINavigationOptions::Accept;
            case EUINavigationAction::Back: return EUINavigationOptions::Back;
            default: return EUINavigationOptions::Invalid;
        }
    }
    
    static bool IsNavigation(EUINavigationOptions Option)
    {
        return Option >= EUINavigationOptions::Left && 
               Option <= EUINavigationOptions::Previous;
    }
    
    static bool IsAction(EUINavigationOptions Option)
    {
        return Option == EUINavigationOptions::Accept || 
               Option == EUINavigationOptions::Back;
    }
    
    static EUINavigation ToNavigation(EUINavigationOptions Option)
    {
        if (!IsNavigation(Option)) return EUINavigation::Invalid;
        
        switch (Option)
        {
            case EUINavigationOptions::Left: return EUINavigation::Left;
            case EUINavigationOptions::Right: return EUINavigation::Right;
            case EUINavigationOptions::Up: return EUINavigation::Up;
            case EUINavigationOptions::Down: return EUINavigation::Down;
            case EUINavigationOptions::Next: return EUINavigation::Next;
            case EUINavigationOptions::Previous: return EUINavigation::Previous;
            default: return EUINavigation::Invalid;
        }
    }
    
    static EUINavigationAction ToAction(EUINavigationOptions Option)
    {
        if (!IsAction(Option)) return EUINavigationAction::Invalid;
        
        switch (Option)
        {
            case EUINavigationOptions::Accept: return EUINavigationAction::Accept;
            case EUINavigationOptions::Back: return EUINavigationAction::Back;
            default: return EUINavigationAction::Invalid;
        }
    }
};

USTRUCT()
struct FSlateNavigationBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Steam|Input|Navigation", meta=(GetOptions = "SteamInput.SteamInputSettings.GetKeyList"))
	FName SteamActionName;

	UPROPERTY(EditAnywhere, Category = "Steam|Input|Navigation")
	EUINavigationOptions NavigationType = EUINavigationOptions::Invalid;

	FSlateNavigationBinding() = default;
	FSlateNavigationBinding(const FName InActionName, const EUINavigationOptions InNavType) 
		: SteamActionName(InActionName), NavigationType(InNavType) {}
	FSlateNavigationBinding(const FName InActionName, const EUINavigation InNavType) 
		: SteamActionName(InActionName), NavigationType(FNavigationOptionHelper::FromNavigation(InNavType)) {}
	FSlateNavigationBinding(const FName InActionName, const EUINavigationAction InNavType) 
		: SteamActionName(InActionName), NavigationType(FNavigationOptionHelper::FromAction(InNavType)) {}
};

USTRUCT()
struct FSteamInputAction
{
	GENERATED_BODY()

	UPROPERTY(Config, EditAnywhere, Category = "Steam|Input")
	FName ActionName;
	
	UPROPERTY(Config, EditAnywhere, Category = "Steam|Input")
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

	// Slate Navigation Configuration
	UPROPERTY(Config, EditAnywhere, Category = "Slate | Navigation", 
			  meta = (ToolTip = "Configure how Steam Input actions control UI navigation"))
	TArray<FSlateNavigationBinding> SlateNavigationBindings;
	
	// Quick setup defaults
	UPROPERTY(Config, EditAnywhere, Category = "Slate | Navigation",
			  meta = (ToolTip = "Automatically configure common navigation bindings"))
	bool bAutoConfigureCommonNavigation = true;
	
	// Default action names for common navigation
	UPROPERTY(Config, EditAnywhere, Category = "Slate | Navigation",
			  meta = (EditCondition = "bAutoConfigureCommonNavigation", GetOptions = "SteamInput.SteamInputSettings.GetKeyList"))
	FName NavigateUpAction = "menu_up";
	
	UPROPERTY(Config, EditAnywhere, Category = "Slate | Navigation", 
			  meta = (EditCondition = "bAutoConfigureCommonNavigation", GetOptions = "SteamInput.SteamInputSettings.GetKeyList"))
	FName NavigateDownAction = "menu_down";
	
	UPROPERTY(Config, EditAnywhere, Category = "Slate | Navigation",
			  meta = (EditCondition = "bAutoConfigureCommonNavigation", GetOptions = "SteamInput.SteamInputSettings.GetKeyList"))  
	FName NavigateLeftAction = "menu_left";
	
	UPROPERTY(Config, EditAnywhere, Category = "Slate | Navigation",
			  meta = (EditCondition = "bAutoConfigureCommonNavigation", GetOptions = "SteamInput.SteamInputSettings.GetKeyList"))
	FName NavigateRightAction = "menu_right";
	
	UPROPERTY(Config, EditAnywhere, Category = "Slate | Navigation",
			  meta = (EditCondition = "bAutoConfigureCommonNavigation", GetOptions = "SteamInput.SteamInputSettings.GetKeyList"))
	FName AcceptAction = "menu_accept";
	
	UPROPERTY(Config, EditAnywhere, Category = "Slate | Navigation",
			  meta = (EditCondition = "bAutoConfigureCommonNavigation", GetOptions = "SteamInput.SteamInputSettings.GetKeyList"))
	FName BackAction = "menu_back";
	
	// Mapping for Steam Input Action Origin to it's texture
	UPROPERTY(Config, EditAnywhere, Category = "Slate | UI")
	TMap<ESteamInputActionOrigin, TSoftObjectPtr<UTexture2D>> ButtonTextureMapping;
	
	static const FName MenuCategory;
	
	static FName GetXAxisName(const FName Name);

	static FName GetYAxisName(const FName Name);

	UFUNCTION()
	static TArray<FName> GetKeyList();
	
	void RefreshHandles();

	void UpdateSlateNavigationConfig();
	void SetupDefaultSlateBindings();
private:
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;

	UFUNCTION()
	void SteamInputInitialized();
	
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;

	UFUNCTION(CallInEditor, Category = "Debug")
	void ValidateSlateIntegration();
	UFUNCTION(CallInEditor, Category = "Debug")
	void LogCurrentSlateConfig();
#endif
	UPROPERTY(VisibleAnywhere, Category = "Debug")
	int AppID;
};
