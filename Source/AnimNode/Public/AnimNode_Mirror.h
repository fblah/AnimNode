/*
	By fBlah (Ajit D'Monte)
*/

#pragma once
#include "CoreMinimal.h"
#include "AnimationMirrorData.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimationAsset.h"
#include "Animation/AnimNodeBase.h"
#include "AlphaBlend.h"


#include "AnimNode_Mirror.generated.h"


USTRUCT(BlueprintInternalUseOnly)
struct ANIMNODE_API FAnimNode_Mirror : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()	

	/** Base Pose */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
		FPoseLink BasePose;
	
	/** Mirror Data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links, meta = (PinShownByDefault))
		UAnimationMirrorData* mAnimMirrorData;

	
public:
	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext & Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext & Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	// End of FAnimNode_Base interface

	// Constructor 
public:
	FAnimNode_Mirror();

protected:	
	AActor* OwningActor;

private:
	void MirrorPose(FTransform& input_pose, const uint8 mirror_axis, const uint8 pos_fwd_mirror);
};