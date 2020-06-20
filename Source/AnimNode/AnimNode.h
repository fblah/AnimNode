/*
	By fBlah (Ajit D'Monte)
*/

#pragma once
 
//Use for plugin

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAnimNodeModule : public IModuleInterface
{
public:

	// IModuleInterface implementation 
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};



//Use for module
//#include "ModuleManager.h"