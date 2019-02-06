/*
	By fBlah (Ajit D'Monte)
*/

#include "AnimGraphNode_Mirror.h"
#include "AnimationGraphSchema.h"


/////////////////////////////////////////////////////
// UAnimGraphNode_Test

UAnimGraphNode_Mirror::UAnimGraphNode_Mirror(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//Title Color!
FLinearColor UAnimGraphNode_Mirror::GetNodeTitleColor() const
{
	return FLinearColor(1.f, .55f, 0.f);
}

//Node Category
FString UAnimGraphNode_Mirror::GetNodeCategory() const
{
	return FString("Anim Node Mirror Pose");
}
FString UAnimGraphNode_Mirror::GetControllerDescription() const
{
	return TEXT("Anim Node Mirror");
}

FText UAnimGraphNode_Mirror::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	FString Result = *GetControllerDescription();
	Result += (TitleType == ENodeTitleType::ListView) ? TEXT("") : TEXT("\n");
	return FText::FromString(Result);
}


//Node Tooltip
FText UAnimGraphNode_Mirror::GetTooltipText() const
{
	return FText::FromString("Test Mirror node");
}

//Node Output Pin(Output is in Component Space, Change at own RISK!)
void UAnimGraphNode_Mirror::CreateOutputPins()
{
	const UAnimationGraphSchema* Schema = GetDefault<UAnimationGraphSchema>();
	CreatePin(EGPD_Output, Schema->PC_Struct, TEXT(""), FPoseLink::StaticStruct(), /*bIsArray=*/ false, /*bIsReference=*/ false, TEXT("Pose"));
}
