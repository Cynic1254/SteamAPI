#pragma once
#if WITH_EDITOR

#include "Settings/SteamInputSettings.h"

struct FSteamInputAction;

class FSettingsInspector : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

protected:
	static FSteamInputAction* GetActionFromHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle);
	static TSharedPtr<IPropertyHandle> GetActionNameHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle);
	static TSharedPtr<IPropertyHandle> GetKeyTypeHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle);
	static FName GetActionNameValue(const TSharedPtr<IPropertyHandle>& PropertyHandle);
	static EKeyType GetKeyTypeValue(const TSharedPtr<IPropertyHandle>& PropertyHandle);

	static FText GetHandleStatusText(const TSharedPtr<IPropertyHandle>& PropertyHandle);
	static FSlateColor GetHandleStatusColor(const TSharedPtr<IPropertyHandle>& PropertyHandle);
	static const FSlateBrush* GetHandleStatusIcon(const TSharedPtr<IPropertyHandle>& PropertyHandle);
	static FText GetHandleStatusTooltip(const TSharedPtr<IPropertyHandle>& PropertyHandle);

	static void RefreshHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle);
};

#endif
