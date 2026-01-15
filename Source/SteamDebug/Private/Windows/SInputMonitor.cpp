// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#include "Windows/SInputMonitor.h"

#include "Helper/SteamInputFunctionLibrary.h"
#include "Settings/SteamInputSettings.h"
#include "steam/isteamcontroller.h"
#include "Subsystems/USteamDebugSubsystem.h"
#include "Containers/Ticker.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Styling/CoreStyle.h"

#define LOCTEXT_NAMESPACE "SteamInputMonitor"

class USteamInputSettings;

void SInputMonitor::Construct(const FArguments& InArgs)
{
	SSteamWindowBase::Construct(SSteamWindowBase::FArguments{});

	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &SInputMonitor::Regenerate), 1);
	
	for (int i = 0; i < STEAM_CONTROLLER_MAX_COUNT; ++i)
	{
		ControllerOptions.Add(MakeShared<int32>(i));
	}
}

TSharedRef<SWidget> SInputMonitor::ConstructContent()
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f)
		[
			CreateControllerSelector()
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f, 4.0f)
		[
			SNew(STextBlock)
			.Text_Lambda([this]() -> FText
			{
				return IsControllerConnected(SelectedControllerIndex) ?
				LOCTEXT("Connected", "✓ Controller Connected") :
				LOCTEXT("NotConnected", "✗ Controller Not Connected");
			})
			.ColorAndOpacity_Lambda([this]() -> FSlateColor
			{
				return IsControllerConnected(SelectedControllerIndex) ?
				FSlateColor(FLinearColor::Green) : 
				FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f));
			})
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f)
		[
			CreateActionSetManager()
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(8.0f, 8.0f, 8.0f, 4.0f)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(0.4f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ActionNameHeader", "Action Name"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
			]

			+ SHorizontalBox::Slot()
			.FillWidth(0.2f)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ActiveHeader", "Active"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
				.ToolTipText(LOCTEXT("ActiveTooltip", "Green when action is in current action set"))
			]

			+ SHorizontalBox::Slot()
			.FillWidth(0.4f)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("StateHeader", "Current State"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
			]
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]

		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(8.0f, 0.0f, 8.0f, 8.0f)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SAssignNew(ActionListBox, SVerticalBox)
			]
		];
}

bool SInputMonitor::Regenerate(float InDeltaTime)
{
	if (ActionListBox.IsValid())
	{
		ActionListBox->ClearChildren();

		if (const USteamInputSettings* Settings = GetDefault<USteamInputSettings>())
		{
			for (const FSteamInputAction& Action : Settings->Keys)
			{
				ActionListBox->AddSlot()
				.AutoHeight()
				.Padding(0, 2)
				[
					CreateActionRow(Action)
				];
			}
		}
	}

	return false;
}

TSharedRef<SWidget> SInputMonitor::CreateControllerSelector()
{
	return SNew(SHorizontalBox)
	
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.VAlign(VAlign_Center)
	.Padding(0, 0, 8, 0)
	[
		SNew(STextBlock)
		.Text(LOCTEXT("ControllerLabel", "Controller:"))
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
	]
	
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		SAssignNew(ControllerComboBox, SComboBox<TSharedPtr<int32>>)
		.OptionsSource(&ControllerOptions)
		.OnGenerateWidget(this, &SInputMonitor::GenerateControllerWidget)
		.OnSelectionChanged(this, &SInputMonitor::OnControllerSelectionChanged)
		.Content()
		[
			SNew(STextBlock)
			.Text(this, &SInputMonitor::GetSelectedControllerText)
		]
	];
}

TSharedRef<SWidget> SInputMonitor::CreateActionRow(const FSteamInputAction& Action) const
{
	return SNew(SHorizontalBox)
		.ToolTipText_Lambda([Action]() -> FText
		{
			return FText::Format(
				LOCTEXT("ActionTooltip", "Type: {0}\nHandle: {1}"),
				FText::FromString(UEnum::GetValueAsString(Action.KeyType)),
				Action.bHandleValid ? 
					FText::FromString(FString::Printf(TEXT("0x%llX"), Action.CachedHandle)) : 
					LOCTEXT("InvalidHandle", "Invalid")
			);
		})

		+ SHorizontalBox::Slot()
		.FillWidth(0.4f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromName(Action.ActionName))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
		]

		+ SHorizontalBox::Slot()
		.FillWidth(0.2f)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ActiveDot", "●"))
			.ColorAndOpacity_Lambda([this, Action]() -> FSlateColor
			{
				return IsActionActive(Action) ? 
					FSlateColor(FLinearColor::Green) : 
					FSlateColor(FLinearColor::Gray);
			})
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
		]

		+ SHorizontalBox::Slot()
		.FillWidth(0.4f)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text_Lambda([this, Action]() -> FText
			{
				return GetActionStateText(Action);
			})
			.ColorAndOpacity_Lambda([this, Action]() -> FSlateColor
			{
				return GetActionStateColor(Action);
			})
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
		];
}

FText SInputMonitor::GetActionStateText(const FSteamInputAction& Action) const
{
	if (!IsControllerConnected(SelectedControllerIndex))
	{
		return LOCTEXT("NoData", "---");
	}
	
	switch (Action.KeyType)
	{
	case EKeyType::Button:
		return UUSteamDebugSubsystem::Get()->IsKeyPressed(Action.ActionName, GetIdFromIndex(SelectedControllerIndex)) ?
			LOCTEXT("Pressed", "Pressed") : LOCTEXT("Released", "Released");
		
	case EKeyType::Analog:
		return FText::AsNumber(UUSteamDebugSubsystem::Get()->GetKeyValue(Action.ActionName, GetIdFromIndex(SelectedControllerIndex)));
		
	case EKeyType::Joystick:
	case EKeyType::MouseInput:
		return FText::Format(
			LOCTEXT("JoyStickValue", "({0}, {1})"),
			UUSteamDebugSubsystem::Get()->GetKeyValue(USteamInputSettings::GetXAxisName(Action.ActionName), GetIdFromIndex(SelectedControllerIndex)),
			UUSteamDebugSubsystem::Get()->GetKeyValue(USteamInputSettings::GetYAxisName(Action.ActionName), GetIdFromIndex(SelectedControllerIndex))
			);
	}
	
	return LOCTEXT("NoData", "---");
}

FSlateColor SInputMonitor::GetActionStateColor(const FSteamInputAction& Action) const
{
	if (!IsControllerConnected(SelectedControllerIndex))
	{
		return FSlateColor(FLinearColor::Red);
	}
	
	switch (Action.KeyType)
	{
	case EKeyType::Button:
		return UUSteamDebugSubsystem::Get()->IsKeyPressed(Action.ActionName, GetIdFromIndex(SelectedControllerIndex)) ?
			FSlateColor(FLinearColor::Green) : FSlateColor(FLinearColor::Yellow);
		
		//TODO: Color based on if this frame received an input
	case EKeyType::Analog:
		return UUSteamDebugSubsystem::Get()->GetKeyValue(Action.ActionName, GetIdFromIndex(SelectedControllerIndex)) == 0.0f ?
			FLinearColor::Yellow : FLinearColor::Green;
	case EKeyType::Joystick:
	case EKeyType::MouseInput:
		const bool IsZero = 
			UUSteamDebugSubsystem::Get()->GetKeyValue(USteamInputSettings::GetXAxisName(Action.ActionName), GetIdFromIndex(SelectedControllerIndex)) == 0.0f ||
			UUSteamDebugSubsystem::Get()->GetKeyValue(USteamInputSettings::GetYAxisName(Action.ActionName), GetIdFromIndex(SelectedControllerIndex)) == 0.0f;
		return IsZero ? FLinearColor::Yellow : FLinearColor::Green;
	}
	
	return FSlateColor(FLinearColor::White);
}

bool SInputMonitor::IsActionActive(const FSteamInputAction& Action) const
{
	if (!SteamInput() || !Action.bHandleValid)
	{
		return false;
	}
	
	// Get the controller handle
	InputHandle_t Controllers[STEAM_INPUT_MAX_COUNT];
	
	if (SteamInput()->GetConnectedControllers(Controllers) < SelectedControllerIndex)
	{
		return false;
	}
	
	// Check if action is active in current action set by querying Steam
	switch (Action.KeyType)
	{
	case EKeyType::Button:
		{
			InputDigitalActionData_t Data = SteamInput()->GetDigitalActionData(
				Controllers[SelectedControllerIndex], 
				Action.CachedHandle
			);
			return Data.bActive;
		}
		
	case EKeyType::Analog:
	case EKeyType::Joystick:
	case EKeyType::MouseInput:
		{
			InputAnalogActionData_t Data = SteamInput()->GetAnalogActionData(
				Controllers[SelectedControllerIndex],
				Action.CachedHandle
			);
			return Data.bActive;
		}
	}
	
	return false;
}

FText SInputMonitor::GetControllerDisplayName(int32 ControllerIndex) const
{
	return FText::Format(LOCTEXT("ControllerFormat", "Controller {0}"), ControllerIndex);
}

bool SInputMonitor::IsControllerConnected(int32 Index) const
{
	if (SteamInput())
	{
		InputHandle_t Controllers[STEAM_INPUT_MAX_COUNT];
		SteamInput()->GetConnectedControllers(Controllers);
		return (Controllers[Index] != 0);
	}
	
	return false;
}

void SInputMonitor::OnControllerSelectionChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (NewSelection.IsValid())
	{
		SelectedControllerIndex = *NewSelection;
	}
}

TSharedRef<SWidget> SInputMonitor::GenerateControllerWidget(TSharedPtr<int32> InOption) const
{
	return SNew(STextBlock)
		.Text(GetControllerDisplayName(*InOption))
		.ColorAndOpacity_Lambda([this, InOption]() -> FSlateColor
		{
			return IsControllerConnected(*InOption) ? 
					FSlateColor(FLinearColor::Green) : 
					FSlateColor(FLinearColor::Yellow);
		});
}

FText SInputMonitor::GetSelectedControllerText() const
{
	return GetControllerDisplayName(SelectedControllerIndex);
}

FInputDeviceId SInputMonitor::GetIdFromIndex(const int32 ControllerIndex) const
{
	if (!SteamInput())
		return INPUTDEVICEID_NONE;
	
	InputHandle_t Controllers[STEAM_INPUT_MAX_COUNT];
	if (SteamInput()->GetConnectedControllers(Controllers) < ControllerIndex)
		return INPUTDEVICEID_NONE;
		
	return USteamInputFunctionLibrary::GetDeviceIDFromSteamID(Controllers[ControllerIndex]);
}

FText SInputMonitor::GetActionSetText(const FInputActionSetHandle& ActionSetHandle) const
{
	if (ActionSetHandle == 0)
		return LOCTEXT("NoActionSet", "None");
	
	return FText::Format(LOCTEXT("ActionSetName", "{0} - {1}"), static_cast<InputActionSetHandle_t>(ActionSetHandle), FText::FromName(USteamInputFunctionLibrary::GetActionSetName(ActionSetHandle)));
}

TSharedRef<SWidget> SInputMonitor::CreateActionSetManager()
{
	return SNew(SHorizontalBox)

	// Active Action Set Section
	+ SHorizontalBox::Slot()
	.FillWidth(0.4f)  // Action Set takes 40%
	.VAlign(VAlign_Center)
	.Padding(0, 0, 8, 0)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0, 0, 8, 0)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ActionSetLabel", "Active Action Set:"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.MinDesiredWidth(200.0f)  // Give it a minimum width
			[
				SNew(SComboButton)
				.OnGetMenuContent(this, &SInputMonitor::CreateActionSetDropdownContent)
				.ButtonContent()
				[
					SNew(STextBlock)
					.Text(this, &SInputMonitor::GetCurrentActionSetText)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				]
			]
		]
	]

	// Action Set Layers Section
	+ SHorizontalBox::Slot()
	.FillWidth(0.6f)  // Layers take 60% (more space for comma list)
	.VAlign(VAlign_Center)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0, 0, 8, 0)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ActionLayersLabel", "Active Layers:"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.MinDesiredWidth(300.0f)  // Wider for comma-separated list
			[
				SNew(SComboButton)
				.OnGetMenuContent(this, &SInputMonitor::CreateLayersDropdownContent)
				.ButtonContent()
				[
					SNew(STextBlock)
					.Text(this, &SInputMonitor::GetCurrentLayersText)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				]
			]
		]
	];
}

FText SInputMonitor::GetCurrentActionSetText() const
{
	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	if (DeviceId == INPUTDEVICEID_NONE)
		return LOCTEXT("NoController", "No Controller");

	const FInputActionSetHandle CurrentSet = USteamInputFunctionLibrary::GetActionSetForController(DeviceId);
	
	return GetActionSetText(CurrentSet);
}

TSharedRef<SWidget> SInputMonitor::CreateActionSetDropdownContent()
{
	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	TSharedRef<SVerticalBox> MenuBox = SNew(SVerticalBox);

	if (DeviceId == INPUTDEVICEID_NONE)
	{
		MenuBox->AddSlot()
		.AutoHeight()
		.Padding(8)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("NoControllerSelected", "No controller selected"))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)))
		];
		return MenuBox;
	}

	const FInputActionSetHandle CurrentSet = USteamInputFunctionLibrary::GetActionSetForController(DeviceId);

	// List all cached action sets
	const TMap<FName, InputActionSetHandle_t>& CachedHandles = USteamInputFunctionLibrary::CachedHandles;
	
	for (const auto& Pair : CachedHandles)
	{
		const FName ActionSetName = Pair.Key;
		const bool bIsActive = (Pair.Value == CurrentSet);

		MenuBox->AddSlot()
		.AutoHeight()
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.OnClicked_Lambda([this, ActionSetName]() -> FReply
			{
				OnActionSetClicked(ActionSetName);
				return FReply::Handled();
			})
			.Content()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(8, 4)
				[
					SNew(STextBlock)
					.Text(bIsActive ? LOCTEXT("ActiveMarker", "●") : LOCTEXT("InactiveMarker", "○"))
					.ColorAndOpacity(bIsActive ? FLinearColor::Green : FLinearColor(0.3f, 0.3f, 0.3f))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
				]

				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(GetActionSetText(Pair.Value))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				]
			]
		];
	}

	// Separator
	MenuBox->AddSlot()
	.AutoHeight()
	.Padding(4, 2)
	[
		SNew(SSeparator)
	];

	// Add new action set field
	MenuBox->AddSlot()
	.AutoHeight()
	.Padding(8, 4)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.Padding(0, 0, 4, 0)
		[
			SNew(SEditableTextBox)
			.HintText(LOCTEXT("NewActionSetHint", "Enter new action set name..."))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
			.OnTextChanged_Lambda([this](const FText& NewText)
			{
				NewActionSetName = NewText.ToString();
			})
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("AddAndActivate", "Add & Activate"))
			.OnClicked(this, &SInputMonitor::OnAddNewActionSet)
			.ToolTipText(LOCTEXT("AddActionSetTooltip", "Add this action set to cache and activate it"))
		]
	];

	return MenuBox;
}

FReply SInputMonitor::OnAddNewActionSet()
{
	if (NewActionSetName.IsEmpty())
		return FReply::Handled();

	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	if (DeviceId == INPUTDEVICEID_NONE)
		return FReply::Handled();

	const FName ActionSetName(*NewActionSetName);
	USteamInputFunctionLibrary::ActivateActionSetByName(DeviceId, ActionSetName);

	// Clear the input field
	NewActionSetName.Empty();

	// Close the dropdown
	FSlateApplication::Get().DismissAllMenus();

	return FReply::Handled();
}

void SInputMonitor::OnActionSetClicked(FName ActionSetName)
{
	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	if (DeviceId == INPUTDEVICEID_NONE)
		return;

	USteamInputFunctionLibrary::ActivateActionSetByName(DeviceId, ActionSetName);
	
	// Close the dropdown
	FSlateApplication::Get().DismissAllMenus();
}

FText SInputMonitor::GetCurrentLayersText() const
{
	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	if (DeviceId == INPUTDEVICEID_NONE)
		return LOCTEXT("NoController", "No Controller");

	TArray<InputActionSetHandle_t>* Layers = USteamInputFunctionLibrary::GetActionLayersForController(DeviceId);
	if (!Layers || Layers->Num() == 0)
		return LOCTEXT("NoLayers", "None");

	// Build comma-separated list of layer IDs (hex format)
	TArray<FString> LayerNames;
	for (const InputActionSetHandle_t& Layer : *Layers)
	{
		LayerNames.Add(USteamInputFunctionLibrary::GetActionSetName(Layer).ToString());
	}

	return FText::FromString(FString::Join(LayerNames, TEXT(", ")));
}

TSharedRef<SWidget> SInputMonitor::CreateLayersDropdownContent()
{
	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	TSharedRef<SVerticalBox> MenuBox = SNew(SVerticalBox);

	if (DeviceId == INPUTDEVICEID_NONE)
	{
		MenuBox->AddSlot()
		.AutoHeight()
		.Padding(8)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("NoControllerSelected", "No controller selected"))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)))
		];
		return MenuBox;
	}

	TArray<InputActionSetHandle_t>* Layers = USteamInputFunctionLibrary::GetActionLayersForController(DeviceId);
	const TMap<FName, InputActionSetHandle_t>& CachedHandles = USteamInputFunctionLibrary::CachedHandles;

	// Display all cached action sets as potential layers
	TArray<TPair<FName, InputActionSetHandle_t>> SortedHandles;
	for (const auto& Pair : CachedHandles)
	{
		SortedHandles.Add(Pair);
	}

	// Sort by name for consistency
	SortedHandles.Sort([](const TPair<FName, InputActionSetHandle_t>& A, const TPair<FName, InputActionSetHandle_t>& B)
	{
		return A.Key.LexicalLess(B.Key);
	});

	// Show active layers first (in order), with reordering controls
	if (Layers && Layers->Num() > 0)
	{
		MenuBox->AddSlot()
		.AutoHeight()
		.Padding(8, 4, 8, 2)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ActiveLayersHeader", "Active Layers (top = highest priority):"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
			.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
		];

		// Display in reverse order (highest priority first)
		for (int32 i = Layers->Num() - 1; i >= 0; --i)
		{
			const InputActionSetHandle_t LayerHandle = (*Layers)[i];
			const FName LayerName = USteamInputFunctionLibrary::GetActionSetName(LayerHandle);
			const int32 ActualIndex = i;

			MenuBox->AddSlot()
			.AutoHeight()
			.Padding(4, 1)
			[
				SNew(SHorizontalBox)

				// Checkbox
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4, 0)
				[
					SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged_Lambda([this, LayerHandle](ECheckBoxState NewState)
					{
						OnLayerCheckChanged(NewState, LayerHandle);
					})
				]

				// Layer name
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				.Padding(4, 0)
				[
					SNew(STextBlock)
					.Text(FText::FromName(LayerName))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				]

				// Move up button
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2, 0)
				[
					SNew(SButton)
					.ButtonStyle(FAppStyle::Get(), "SimpleButton")
					.OnClicked_Lambda([this, ActualIndex]() -> FReply
					{
						return OnMoveLayerUp(ActualIndex);
					})
					.IsEnabled(i < Layers->Num() - 1)
					.ToolTipText(LOCTEXT("MoveUpTooltip", "Increase priority"))
					.ContentPadding(FMargin(4, 2))
					[
						SNew(STextBlock)
						.Text(LOCTEXT("UpArrow", "▲"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
					]
				]

				// Move down button
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2, 0)
				[
					SNew(SButton)
					.ButtonStyle(FAppStyle::Get(), "SimpleButton")
					.OnClicked_Lambda([this, ActualIndex]() -> FReply
					{
						return OnMoveLayerDown(ActualIndex);
					})
					.IsEnabled(i > 0)
					.ToolTipText(LOCTEXT("MoveDownTooltip", "Decrease priority"))
					.ContentPadding(FMargin(4, 2))
					[
						SNew(STextBlock)
						.Text(LOCTEXT("DownArrow", "▼"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
					]
				]
			];
		}

		MenuBox->AddSlot()
		.AutoHeight()
		.Padding(4, 4)
		[
			SNew(SSeparator)
		];
	}

	// Show all cached layers with checkboxes
	MenuBox->AddSlot()
	.AutoHeight()
	.Padding(8, 2, 8, 2)
	[
		SNew(STextBlock)
		.Text(LOCTEXT("AllLayersHeader", "All Cached Layers:"))
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8))
		.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
	];

	for (const auto& Pair : SortedHandles)
	{
		const FName LayerName = Pair.Key;
		const InputActionSetHandle_t LayerHandle = Pair.Value;
		const bool bIsActive = IsLayerActive(LayerHandle);

		MenuBox->AddSlot()
		.AutoHeight()
		.Padding(4, 1)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(4, 0)
			[
				SNew(SCheckBox)
				.IsChecked(bIsActive ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged_Lambda([this, LayerHandle](ECheckBoxState NewState)
				{
					OnLayerCheckChanged(NewState, LayerHandle);
				})
			]

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.Padding(4, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromName(LayerName))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(bIsActive ? FLinearColor::White : FLinearColor(0.6f, 0.6f, 0.6f))
			]
		];
	}

	// Separator
	MenuBox->AddSlot()
	.AutoHeight()
	.Padding(4, 4)
	[
		SNew(SSeparator)
	];

	// Add new layer field
	MenuBox->AddSlot()
	.AutoHeight()
	.Padding(8, 4)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.Padding(0, 0, 4, 0)
		[
			SNew(SEditableTextBox)
			.HintText(LOCTEXT("NewLayerHint", "Enter new layer name..."))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
			.OnTextChanged_Lambda([this](const FText& NewText)
			{
				NewLayerName = NewText.ToString();
			})
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("AddLayerButton", "Add Layer"))
			.OnClicked(this, &SInputMonitor::OnAddNewLayer)
			.ToolTipText(LOCTEXT("AddLayerTooltip", "Add this layer to cache and activate it"))
		]
	];

	return MenuBox;
}

FReply SInputMonitor::OnAddNewLayer()
{
	if (NewLayerName.IsEmpty())
		return FReply::Handled();

	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	if (DeviceId == INPUTDEVICEID_NONE)
		return FReply::Handled();

	const FName LayerName(*NewLayerName);
	USteamInputFunctionLibrary::PushActionLayerByName(DeviceId, LayerName);

	// Clear the input field
	NewLayerName.Empty();

	return FReply::Handled();
}

void SInputMonitor::OnLayerCheckChanged(const ECheckBoxState NewState, const FInputActionSetHandle& LayerHandle) const
{
	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	if (DeviceId == INPUTDEVICEID_NONE)
		return;

	if (NewState == ECheckBoxState::Checked)
	{
		// Add layer (push to top)
		USteamInputFunctionLibrary::PushActionLayer(DeviceId, LayerHandle);
	}
	else
	{
		// Remove layer
		USteamInputFunctionLibrary::RemoveActionLayer(DeviceId, LayerHandle);
	}
}

bool SInputMonitor::IsLayerActive(const FInputActionSetHandle& LayerHandle) const
{
	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	if (DeviceId == INPUTDEVICEID_NONE)
		return false;

	TArray<InputActionSetHandle_t>* Layers = USteamInputFunctionLibrary::GetActionLayersForController(DeviceId);
	return Layers && Layers->Contains(LayerHandle);
}

FReply SInputMonitor::OnMoveLayerUp(int32 Index)
{
	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	if (DeviceId == INPUTDEVICEID_NONE)
		return FReply::Handled();

	TArray<InputActionSetHandle_t>* Layers = USteamInputFunctionLibrary::GetActionLayersForController(DeviceId);
	if (!Layers || Index >= Layers->Num() - 1)
		return FReply::Handled();

	// Swap with layer above (higher priority)
	Layers->Swap(Index, Index + 1);

	return FReply::Handled();
}

FReply SInputMonitor::OnMoveLayerDown(int32 Index)
{
	const FInputDeviceId DeviceId = GetIdFromIndex(SelectedControllerIndex);
	if (DeviceId == INPUTDEVICEID_NONE)
		return FReply::Handled();

	TArray<InputActionSetHandle_t>* Layers = USteamInputFunctionLibrary::GetActionLayersForController(DeviceId);
	if (!Layers || Index <= 0)
		return FReply::Handled();

	// Swap with layer below (lower priority)
	Layers->Swap(Index, Index - 1);

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
