// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#pragma once
#include "SSteamWindowBase.h"

#include "Widgets/Input/SComboBox.h"

struct FInputActionSetHandle;
struct FSteamInputAction;

class SVerticalBox;

class SInputMonitor : public SSteamWindowBase
{
public:
	SLATE_BEGIN_ARGS(SInputMonitor) {}
	SLATE_END_ARGS()

	virtual ~SInputMonitor() override = default;
	
	void Construct(const FArguments& InArgs);
protected:
	virtual TSharedRef<SWidget> ConstructContent() override;
	virtual FText GetWindowTitle() const override {return FText::FromString("Input Monitor");}
private:
	bool Regenerate(float InDeltaTime);

	int32 SelectedControllerIndex = 0;
	TArray<TSharedPtr<int32>> ControllerOptions;
	TSharedPtr<SComboBox<TSharedPtr<int32>>> ControllerComboBox;

	TSharedPtr<SVerticalBox> ActionListBox;
	
	// Action Set Management
	FString NewActionSetName;
	FString NewLayerName;

	TSharedRef<SWidget> CreateControllerSelector();
	TSharedRef<SWidget> CreateActionSetManager();
	TSharedRef<SWidget> CreateActionRow(const FSteamInputAction& Action) const;
	
	// ETriggerEvent GetKeyState(FName KeyName, int32 ControllerID);
	FText GetActionStateText(const FSteamInputAction& Action) const;
	FSlateColor GetActionStateColor(const FSteamInputAction& Action) const;

	bool IsActionActive(const FSteamInputAction& Action) const;

	FText GetControllerDisplayName(int32 ControllerIndex) const;
	bool IsControllerConnected(int32 Index) const;

	void OnControllerSelectionChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> GenerateControllerWidget(TSharedPtr<int32> InOption) const;
	FText GetSelectedControllerText() const;
	
	FInputDeviceId GetIdFromIndex(int32 ControllerIndex) const;
	
	// Action Set Management Methods
	FText GetActionSetText(const FInputActionSetHandle& ActionSetHandle) const;
	FText GetCurrentActionSetText() const;
	TSharedRef<SWidget> CreateActionSetDropdownContent();
	FReply OnAddNewActionSet();
	void OnActionSetClicked(FName ActionSetName);
	
	// Action Layer Management Methods  
	FText GetCurrentLayersText() const;
	TSharedRef<SWidget> CreateLayersDropdownContent();
	FReply OnAddNewLayer();
	void OnLayerCheckChanged(ECheckBoxState NewState, const FInputActionSetHandle& LayerHandle) const;
	bool IsLayerActive(const FInputActionSetHandle& LayerHandle) const;
	FReply OnMoveLayerUp(int32 Index);
	FReply OnMoveLayerDown(int32 Index);
	
};
