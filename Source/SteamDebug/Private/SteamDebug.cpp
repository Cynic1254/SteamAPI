#include "SteamDebug.h"

#include "ToolMenus.h"
#include "Windows/InputMonitor.h"

#define LOCTEXT_NAMESPACE "FSteamDebugModule"

DEFINE_LOG_CATEGORY_STATIC(SteamDebug, Log, All)

void FSteamDebugModule::StartupModule()
{
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSteamDebugModule::RegisterWindows));
	
    OpenSteamWindowCommand = IConsoleManager::Get().RegisterConsoleCommand(
    	TEXT("Steam.OpenDebug"),
    	TEXT("Opens a Steam debug window.\n")
		TEXT("Usage: Steam.OpenDebug <WindowName>\n")
		TEXT("Call without arguments to list available windows."),
    	FConsoleCommandWithArgsDelegate::CreateRaw(this, &FSteamDebugModule::OpenSteamWindow)
    	);
}

void FSteamDebugModule::ShutdownModule()
{
	if (OpenSteamWindowCommand)
	{
		IConsoleManager::Get().UnregisterConsoleObject(OpenSteamWindowCommand);
		OpenSteamWindowCommand = nullptr;
	}

#if WITH_EDITOR
	for (const auto& SteamWindow : SteamWindows)
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SteamWindow.Value.WindowId);
	}
#endif
	
	// Close all standalone windows
	for (TSharedPtr<SWindow>& Window : OpenWindows)
	{
		if (Window.IsValid())
		{
			Window->RequestDestroyWindow();
		}
	}
	OpenWindows.Empty();
	SteamWindows.Empty();
}

void FSteamDebugModule::RegisterWindows()
{
	FToolMenuOwnerScoped OwnerScoped(this);
    
	// Get the main menu bar
	UToolMenu* MainMenuBar = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
    
	// Add a new menu entry to the menu bar
	FToolMenuSection& Section = MainMenuBar->AddSection("Steam", LOCTEXT("SteamSection", "Steam"));

	Section.AddSubMenu(
		"SteamMenu",
		LOCTEXT("SteamMenu", "Steam"),
		LOCTEXT("SteamMenuTooltip", "Steam Debugging Tools"),
		FNewToolMenuDelegate::CreateLambda([](UToolMenu* Submenu){}),
		false,
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Visibility")
		);

	RegisterDebugWindow<SInputMonitor>("Steam.Input.KeyMonitor", FText::FromString("Key Monitor"), FText::FromString("Monitor for steam keys"));
}

void FSteamDebugModule::RegisterDebugWindow(const FDebugWindowInfo& WindowInfo)
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		WindowInfo.WindowId,
		FOnSpawnTab::CreateLambda([WindowInfo](const FSpawnTabArgs& Args)
		{
			return SNew(SDockTab)
				.TabRole(NomadTab)
				[
					WindowInfo.WidgetFactory()
				];
		})
		)
	.SetDisplayName(WindowInfo.DisplayName)
	.SetTooltipText(WindowInfo.TooltipText)
	.SetMenuType(ETabSpawnerMenuType::Hidden)
	.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Visibility"));
    
	SteamWindows.Add(WindowInfo.WindowId.ToString(), WindowInfo);

	FToolMenuSection& Section = GetSteamMenuSection(WindowInfo.WindowId);
	Section.AddMenuEntry(WindowInfo.WindowId, WindowInfo.DisplayName, WindowInfo.TooltipText, FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Visibility"),
		FUIAction(FExecuteAction::CreateLambda([WindowInfo]()
		{
			FGlobalTabmanager::Get()->TryInvokeTab(WindowInfo.WindowId);
		})));
}

void FSteamDebugModule::OpenSteamWindow(const TArray<FString>& Args)
{
	if (Args.IsEmpty())
	{
		UE_LOG(SteamDebug, Log, TEXT("Available steam debug windows:"));

		for (const auto& Element : SteamWindows)
		{
			UE_LOG(SteamDebug, Log, TEXT("	- %s"), *Element.Key);
		}
		return;
	}
	
	if (!SteamWindows.Contains(Args[0]))
	{
		UE_LOG(SteamDebug, Warning, TEXT("Unknown window: '%s'"), *Args[0]);
		UE_LOG(SteamDebug, Log, TEXT("Available windows:"));
		for (const auto& [Name, Info] : SteamWindows)
		{
			UE_LOG(SteamDebug, Log, TEXT("  - %s"), *Name);
		}
		return;
	}

#if WITH_EDITOR
	FGlobalTabmanager::Get()->TryInvokeTab(SteamWindows.Find(Args[0])->WindowId);
#else
	const FDebugWindowInfo& Info = SteamWindows[Args[0]];
	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(FText::FromString(Args[0]))
		.ClientSize(FVector2D(800, 600))
		.SupportsMaximize(true)
		.SupportsMinimize(true)
		[
			Info.WidgetFactory()
		];

	FSlateApplication::Get().AddWindow(Window);
	OpenWindows.Add(Window);
#endif
}

FToolMenuSection& FSteamDebugModule::GetSteamMenuSection(const FName TabId)
{
	TArray<FString> Parts;
	TabId.ToString().ParseIntoArray(Parts, TEXT("."));

	if (Parts.Num() > 0 && Parts[0] == "Steam")
	{
		Parts.RemoveAt(0);
	}
	
	// Edge case: empty or single part path
	if (Parts.Num() <= 1)
	{
		UToolMenu* SteamMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.SteamMenu");
		return SteamMenu->FindOrAddSection("Default");
	}

	// Start at the Steam menu
	FString CurrentMenuPath = "LevelEditor.MainMenu.SteamMenu";
	FString CurrentSectionName = "Default";

	for (int32 i = 0; i < Parts.Num() - 1; ++i)
	{
		const FString& Part = Parts[i];
		
		FString MenuName;
		FString SectionName;

		// Check if this part defines a section: "Input,Advanced"
		if (!Part.Split(TEXT(","), &MenuName, &SectionName))
		{
			// No section: entire part is menu name
			MenuName = Part;
			SectionName = "Default";
		}

		FString SubMenuPath = CurrentMenuPath + TEXT(".") + MenuName;
		FString SubMenuKey = CurrentMenuPath + TEXT(":") + MenuName + TEXT(":") + CurrentSectionName;

		// Create submenu if it doesn't exist yet
		if (!CreatedSubMenus.Contains(SubMenuKey))
		{
			UToolMenu* ParentMenu = UToolMenus::Get()->ExtendMenu(*CurrentMenuPath);
			FToolMenuSection& ParentSection = ParentMenu->FindOrAddSection(*CurrentSectionName);
            
			// Create the submenu
			ParentSection.AddSubMenu(
				*MenuName,
				FText::FromString(MenuName),
				FText::FromString(FString::Printf(TEXT("%s submenu"), *MenuName)),
				FNewToolMenuDelegate::CreateLambda([SectionName](UToolMenu* SubMenu)
				{
					// Pre-create the section when submenu is first opened
					SubMenu->FindOrAddSection(*SectionName);
				}),
				false,
				FSlateIcon()
			);
            
			CreatedSubMenus.Add(SubMenuKey);
		}

		CurrentMenuPath = SubMenuPath;
		CurrentSectionName = SectionName;
	}

	UToolMenu* FinalMenu = UToolMenus::Get()->ExtendMenu(*CurrentMenuPath);
	return FinalMenu->FindOrAddSection(*CurrentSectionName);
}

bool FuzzyMatch(const FString& Name, const FString& Query)
{
	if (Query.IsEmpty())
		return true;
        
	FString LowerName = Name.ToLower();
	FString LowerQuery = Query.ToLower();
    
	int32 NameIndex = 0;
	int32 QueryIndex = 0;
    
	while (NameIndex < LowerName.Len() && QueryIndex < LowerQuery.Len())
	{
		if (LowerName[NameIndex] == LowerQuery[QueryIndex])
		{
			QueryIndex++;
		}
		NameIndex++;
	}
    
	return QueryIndex == LowerQuery.Len();
}

void FSteamDebugModule::GetWindowNameSuggestions(const TArray<FString>& Args, TArray<FString>& Suggestions)
{
	const FString PartialName = Args.Num() > 0 ? Args[0] : TEXT("");
    
	if (PartialName.IsEmpty())
	{
		for (const auto& [WindowName, Info] : SteamWindows)
		{
			Suggestions.Add(WindowName);
		}
		return;
	}
    
	for (const auto& [WindowName, Info] : SteamWindows)
	{
		if (FuzzyMatch(WindowName, PartialName))
		{
			Suggestions.Add(WindowName);
		}
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSteamDebugModule, SteamDebug)