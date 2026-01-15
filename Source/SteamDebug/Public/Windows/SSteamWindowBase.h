// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

#pragma once

#include "Widgets/SCompoundWidget.h"

class SSteamWindowBase : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSteamWindowBase) {}
	SLATE_END_ARGS()

	virtual ~SSteamWindowBase() override = default;

	void Construct(const FArguments& InArgs);

protected:
	virtual TSharedRef<SWidget> ConstructContent() = 0;
	virtual FText GetWindowTitle() const = 0;
};
