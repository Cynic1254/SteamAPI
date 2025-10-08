#pragma once

class SSteamWindowBase : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSteamWindowBase) {}
	SLATE_END_ARGS()

	virtual ~SSteamWindowBase() override = default;

	void Construct(const FArguments& InArgs)
	{
		ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(this, &SSteamWindowBase::GetWindowTitle)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
			]

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				ConstructContent()
			]
		];
	}

protected:
	virtual TSharedRef<SWidget> ConstructContent() = 0;
	virtual FText GetWindowTitle() const = 0;
};
