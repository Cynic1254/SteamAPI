#pragma once
#include "SSteamWindowBase.h"

class SInputMonitor : public SSteamWindowBase
{
protected:
	virtual TSharedRef<SWidget> ConstructContent() override;
	virtual FText GetWindowTitle() const override {return FText::FromString("Input Monitor");}
};
