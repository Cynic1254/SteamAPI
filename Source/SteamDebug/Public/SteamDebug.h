// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

struct FToolMenuSection;
class SWidget;
class SWindow;
struct IConsoleCommand;

class FSteamDebugModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    struct FDebugWindowInfo
    {
        FName WindowId;
        FText DisplayName;
        FText TooltipText;
        TFunction<TSharedRef<SWidget>()> WidgetFactory;
    };
    
    void RegisterWindows();
    
    template<typename WidgetType>
    void RegisterDebugWindow(FName TabId, FText DisplayName, FText TooltipText);
    void RegisterDebugWindow(const FDebugWindowInfo& WindowInfo);

    void OpenSteamWindow(const TArray<FString>& Args);
private:
    FToolMenuSection& GetSteamMenuSection(FName TabId);
    
    TMap<FString, FDebugWindowInfo> SteamWindows;
    TArray<TSharedPtr<SWindow>> OpenWindows;

    void GetWindowNameSuggestions(const TArray<FString>& Args, TArray<FString>& Suggestions);
    IConsoleCommand* OpenSteamWindowCommand = nullptr;
    
    TSet<FString> CreatedSubMenus;
};

template <typename WidgetType>
void FSteamDebugModule::RegisterDebugWindow(const FName TabId, const FText DisplayName, const FText TooltipText)
{
    FDebugWindowInfo Info;
    Info.WindowId = TabId;
    Info.DisplayName = DisplayName;
    Info.TooltipText = TooltipText;
    Info.WidgetFactory = []() -> TSharedRef<SWidget>
    {
        // Bypass the SNew macro entirely - direct construction
        TSharedRef<WidgetType> Widget = MakeShared<WidgetType>();
        
        // Call Construct with default arguments
        typename WidgetType::FArguments Args;
        Widget->Construct(Args);
        
        return Widget;
    };

    RegisterDebugWindow(Info);
}
