/*
	By fBlah (Ajit D'Monte)
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AnimGraphNode_Base.h"
#include "AnimNode_Mirror.h"

#include "AnimGraphNode_Mirror.generated.h"


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ANIMNODEEDITOR_API UAnimGraphNode_Mirror : public UAnimGraphNode_Base
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_Mirror Node;
	
public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FString GetNodeCategory() const override;
	virtual FText GetTooltipText() const override;
	virtual void CreateOutputPins() override;
	// End of UEdGraphNode interface

protected:
	virtual FString GetControllerDescription() const;	
	
};
