// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#if WITH_EDITOR

#include "Settings/SettingsInspector.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Settings/SteamInputSettings.h"
#include "Controller/FSteamInputController.h"

#define LOCTEXT_NAMESPACE "SteamInputActionCustomization"

TSharedRef<IPropertyTypeCustomization> FSettingsInspector::MakeInstance()
{
	return MakeShareable(new FSettingsInspector);
}

void FSettingsInspector::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const TSharedPtr<IPropertyHandle> ActionNameHandle = GetActionNameHandle(PropertyHandle);
    const TSharedPtr<IPropertyHandle> KeyTypeHandle = GetKeyTypeHandle(PropertyHandle);
    
    HeaderRow
    .NameContent()
    [
        SNew(STextBlock)
        .Text(LOCTEXT("SteamActionLabel", "Steam Action"))
        .Font(IDetailLayoutBuilder::GetDetailFont())
    ]
    .ValueContent()
    .MaxDesiredWidth(600.0f)
    [
        SNew(SHorizontalBox)

        // Action Name - Use AutoWidth with MinDesiredWidth
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        .Padding(0, 0, 8, 0)
        [
            SNew(SBox)
            .MinDesiredWidth(150.0f)
            [
                ActionNameHandle.IsValid() ? ActionNameHandle->CreatePropertyValueWidget() : SNullWidget::NullWidget
            ]
        ]

        // Key Type
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        .Padding(0, 0, 8, 0)
        [
            SNew(SBox)
            .MinDesiredWidth(100.0f)
            [
                KeyTypeHandle.IsValid() ? KeyTypeHandle->CreatePropertyValueWidget() : SNullWidget::NullWidget
            ]
        ]

        // Status Icon
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        .Padding(0, 0, 4, 0)
        [
            SNew(SImage)
            .Image_Lambda([this, PropertyHandle]() -> const FSlateBrush*
            {
                return GetHandleStatusIcon(PropertyHandle);
            })
            .ColorAndOpacity_Lambda([this, PropertyHandle]() -> FSlateColor
            {
                return GetHandleStatusColor(PropertyHandle);
            })
            .ToolTipText_Lambda([this, PropertyHandle]() -> FText
            {
                return GetHandleStatusTooltip(PropertyHandle);
            })
        ]

        // Status Text
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        .Padding(0, 0, 8, 0)
        [
            SNew(STextBlock)
            .Text_Lambda([this, PropertyHandle]() -> FText
            {
                return GetHandleStatusText(PropertyHandle);
            })
            .ColorAndOpacity_Lambda([this, PropertyHandle]() -> FSlateColor
            {
                return GetHandleStatusColor(PropertyHandle);
            })
            .Font(IDetailLayoutBuilder::GetDetailFont())
            .ToolTipText_Lambda([this, PropertyHandle]() -> FText
            {
                return GetHandleStatusTooltip(PropertyHandle);
            })
        ]

        // Refresh Button
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew(SButton)
            .ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
            .ContentPadding(FMargin(2, 2))
            .OnClicked_Lambda([this, PropertyHandle]() -> FReply
            {
                RefreshHandle(PropertyHandle);
                return FReply::Handled();
            })
            .ToolTipText(LOCTEXT("RefreshHandleTooltip", "Refresh Steam Input handle for this action"))
            [
                SNew(SImage)
                .Image(FAppStyle::GetBrush("Icons.Refresh"))
                .ColorAndOpacity(FSlateColor::UseForeground())
            ]
        ]

        // Spacer to push everything left
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        [
            SNullWidget::NullWidget
        ]
    ];
}

void FSettingsInspector::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// Add advanced debug information (collapsed by default)
    ChildBuilder.AddCustomRow(LOCTEXT("AdvancedInfo", "Advanced"))
    .NameContent()
    [
        SNew(STextBlock)
        .Text(LOCTEXT("HandleInfoLabel", "Handle Info"))
        .Font(IDetailLayoutBuilder::GetDetailFont())
    ]
    .ValueContent()
    [
        SNew(SVerticalBox)
        
        // Handle value
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(0, 2)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("HandleValueLabel", "Handle: "))
                .Font(IDetailLayoutBuilder::GetDetailFont())
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text_Lambda([this, PropertyHandle]() -> FText
                {
                    if (FSteamInputAction* Action = GetActionFromHandle(PropertyHandle))
                    {
                        if (Action->CachedHandle != 0)
                        {
                            return FText::FromString(FString::Printf(TEXT("0x%016llX"), Action->CachedHandle));
                        }
                    }
                    return LOCTEXT("NoHandle", "None");
                })
                .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
                .ColorAndOpacity(FSlateColor::UseSubduedForeground())
            ]
        ]
        
        // Steam status
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(0, 2)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("SteamStatusLabel", "Steam: "))
                .Font(IDetailLayoutBuilder::GetDetailFont())
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text_Lambda([]() -> FText
                {
                    return SteamInput() ? LOCTEXT("SteamAvailable", "Available") : LOCTEXT("SteamUnavailable", "Not Available");
                })
                .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
                .ColorAndOpacity_Lambda([]() -> FSlateColor
                {
                    return SteamInput() ? FSlateColor(FLinearColor::Green) : FSlateColor(FLinearColor::Red);
                })
            ]
        ]
    ];
}

TSharedPtr<IPropertyHandle> FSettingsInspector::GetActionNameHandle(
	const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
	return PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSteamInputAction, ActionName));
}

TSharedPtr<IPropertyHandle> FSettingsInspector::GetKeyTypeHandle(
	const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
	return PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSteamInputAction, KeyType));
}

FName FSettingsInspector::GetActionNameValue(const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
	const TSharedPtr<IPropertyHandle> ActionNameHandle = GetActionNameHandle(PropertyHandle);
	if (ActionNameHandle.IsValid())
	{
		FName ActionName;
		if (ActionNameHandle->GetValue(ActionName) == FPropertyAccess::Success)
		{
			return ActionName;
		}
	}
	return NAME_None;
}

EKeyType FSettingsInspector::GetKeyTypeValue(const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
	const TSharedPtr<IPropertyHandle> KeyTypeHandle = GetKeyTypeHandle(PropertyHandle);
	if (KeyTypeHandle.IsValid())
	{
		uint8 KeyTypeValue;
		if (KeyTypeHandle->GetValue(KeyTypeValue) == FPropertyAccess::Success)
		{
			return static_cast<EKeyType>(KeyTypeValue);
		}
	}
	return EKeyType::Button;
}

FSteamInputAction* FSettingsInspector::GetActionFromHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
	if (!PropertyHandle.IsValid())
		return nullptr;

	TArray<void*> RawData;
	PropertyHandle->AccessRawData(RawData);
    
	if (RawData.Num() > 0 && RawData[0])
	{
		return static_cast<FSteamInputAction*>(RawData[0]);
	}
    
	return nullptr;
}

FText FSettingsInspector::GetHandleStatusText(const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
	FSteamInputAction* Action = GetActionFromHandle(PropertyHandle);
	if (!Action)
	{
		return LOCTEXT("ErrorStatus", "Error");
	}

	// Check if Steam Input is available
	if (!SteamInput())
	{
		return LOCTEXT("SteamNotAvailable", "Steam N/A");
	}

	// Get the action name safely
	FName ActionName = GetActionNameValue(PropertyHandle);
	if (ActionName.IsNone() || ActionName.ToString().IsEmpty())
	{
		return LOCTEXT("NoActionName", "No Name");
	}

	// Refresh the handle (this updates bHandleValid)
	Action->GenerateHandle();

	if (Action->bHandleValid && Action->CachedHandle != 0)
	{
		return LOCTEXT("ValidStatus", "Valid");
	}

	return LOCTEXT("InvalidStatus", "Invalid");
}

FSlateColor FSettingsInspector::GetHandleStatusColor(const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
	FSteamInputAction* Action = GetActionFromHandle(PropertyHandle);
	if (!Action)
	{
		return FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)); // Gray
	}

	if (!SteamInput())
	{
		return FSlateColor(FLinearColor(1.0f, 0.8f, 0.0f)); // Orange/Yellow
	}

	// Get the action name safely
	FName ActionName = GetActionNameValue(PropertyHandle);
	if (ActionName.IsNone() || ActionName.ToString().IsEmpty())
	{
		return FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)); // Gray for no name
	}

	// Refresh the handle
	Action->GenerateHandle();

	if (Action->bHandleValid && Action->CachedHandle != 0)
	{
		return FSlateColor(FLinearColor(0.0f, 0.8f, 0.0f)); // Green
	}

	return FSlateColor(FLinearColor(0.8f, 0.2f, 0.2f)); // Red
}

const FSlateBrush* FSettingsInspector::GetHandleStatusIcon(const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
	FSteamInputAction* Action = GetActionFromHandle(PropertyHandle);
	if (!Action)
	{
		return FAppStyle::GetBrush("Icons.Warning");
	}

	if (!SteamInput())
	{
		return FAppStyle::GetBrush("Icons.Warning");
	}

	// Get the action name safely
	FName ActionName = GetActionNameValue(PropertyHandle);
	if (ActionName.IsNone() || ActionName.ToString().IsEmpty())
	{
		return FAppStyle::GetBrush("Icons.Info");
	}

	// Refresh the handle
	Action->GenerateHandle();

	if (Action->bHandleValid && Action->CachedHandle != 0)
	{
		return FAppStyle::GetBrush("Symbols.Check");
	}

	return FAppStyle::GetBrush("Symbols.X");
}

FText FSettingsInspector::GetHandleStatusTooltip(const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
	FSteamInputAction* Action = GetActionFromHandle(PropertyHandle);
	if (!Action)
	{
		return LOCTEXT("ErrorTooltip", "Error accessing action data");
	}

	if (!SteamInput())
	{
		return LOCTEXT("SteamNotAvailableTooltip", "Steam Input API is not available. Make sure Steam is running and the Steam Input module is loaded.");
	}

	FName ActionName = GetActionNameValue(PropertyHandle);
	if (ActionName.IsNone() || ActionName.ToString().IsEmpty())
	{
		return LOCTEXT("NoActionNameTooltip", "Action name is empty. Enter a valid action name that matches your Steam Input configuration.");
	}

	// Refresh the handle
	Action->GenerateHandle();

	if (Action->bHandleValid && Action->CachedHandle != 0)
	{
		return FText::Format(LOCTEXT("ValidTooltip", "Action '{0}' is valid and has handle: 0x{1}"), 
						   FText::FromName(ActionName), 
						   FText::FromString(FString::Printf(TEXT("%016llX"), Action->CachedHandle)));
	}

	return FText::Format(LOCTEXT("InvalidTooltip", "Action '{0}' is invalid. Make sure this action name exists in your Steam Input configuration file."), 
	                     FText::FromName(ActionName));
}

void FSettingsInspector::RefreshHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
	if (FSteamInputAction* Action = GetActionFromHandle(PropertyHandle))
	{
		Action->GenerateHandle();
	}
}

#endif