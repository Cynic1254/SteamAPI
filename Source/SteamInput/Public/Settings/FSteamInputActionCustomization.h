#pragma once

#if WITH_EDITOR
#include "SteamInputSettings.h"

class FSteamInputActionCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:
	void OnKeyTypeChanged();
	void RefreshHandle();

	TWeakObjectPtr<USteamInputSettings> SettingsHandle;
	
	TSharedPtr<IPropertyHandle> KeyTypeHandle;
	TSharedPtr<IPropertyHandle> CachedHandleHandle;
	TSharedPtr<IPropertyHandle> HandleValidHandle;
	
	FName CurrentActionName;
};

#endif
