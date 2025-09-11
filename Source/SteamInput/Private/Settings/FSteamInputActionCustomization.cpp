#if WITH_EDITOR

#include "Settings/FSteamInputActionCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Settings/SteamInputSettings.h"

TSharedRef<IPropertyTypeCustomization> FSteamInputActionCustomization::MakeInstance()
{
	return MakeShared<FSteamInputActionCustomization>();
}

void FSteamInputActionCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	KeyTypeHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSteamInputAction, KeyType));
	CachedHandleHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSteamInputAction, CachedHandle));
	HandleValidHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSteamInputAction, bHandleValid));

	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth()
		[
			KeyTypeHandle->CreatePropertyValueWidget()
		]
		+ SHorizontalBox::Slot()
		.Padding(5, 0,0,0)
		.AutoWidth()
		[
			HandleValidHandle->CreatePropertyValueWidget()
		]
	];

	if (KeyTypeHandle.IsValid())
	{
		KeyTypeHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FSteamInputActionCustomization::OnKeyTypeChanged));
	}
}

void FSteamInputActionCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ChildBuilder.AddProperty(CachedHandleHandle.ToSharedRef()).IsEnabled(false);
}

void FSteamInputActionCustomization::OnKeyTypeChanged()
{
	RefreshHandle();
}

void FSteamInputActionCustomization::RefreshHandle()
{
	USteamInputSettings* Settings = GetMutableDefault<USteamInputSettings>();
	if (Settings)
	{
		Settings->RefreshHandles();
	}
}
#endif