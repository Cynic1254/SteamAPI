#pragma once

#include "CoreMinimal.h"

struct FToolMenuSection;

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
    Info.WidgetFactory = []()
    {
        return SNew(WidgetType);
    };

    RegisterDebugWindow(Info);
}
