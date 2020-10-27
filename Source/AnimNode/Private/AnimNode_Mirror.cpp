/*
	By fBlah (Ajit D'Monte)
*/


// I used the code on this site:
// http://peyman-mass.blogspot.in/2015/11/mirroring-3d-character-animations.html

#include "AnimNode_Mirror.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationMirrorData.h"
#include "Runtime/Launch/Resources/Version.h"

FAnimNode_Mirror::FAnimNode_Mirror()
	: FAnimNode_Base()	
{
	mAnimMirrorData = NewObject<UAnimationMirrorData>();
}

void FAnimNode_Mirror::Initialize_AnyThread(const FAnimationInitializeContext & Context)
{
	//Init the Inputs
	BasePose.Initialize(Context);	
	
	//Get the Actor Owner
	OwningActor = Context.AnimInstanceProxy->GetSkelMeshComponent()->GetOwner();
}

void FAnimNode_Mirror::CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)
{
	BasePose.CacheBones(Context);
	
}
void FAnimNode_Mirror::Update_AnyThread(const FAnimationUpdateContext & Context)
{
	//***************************************
	// Evaluate Graph, see AnimNode_Base, AnimNodeBase.h
#if ENGINE_MINOR_VERSION >= 22
        GetEvaluateGraphExposedInputs().Execute(Context);
#endif 
#if ENGINE_MINOR_VERSION < 22
        EvaluateGraphExposedInputs.Execute(Context);
#endif 
	//***************************************
	
	//Try Again if not found
	if (!OwningActor) OwningActor =
		Context.AnimInstanceProxy->GetSkelMeshComponent()->GetOwner();

	//Not found
	if (!OwningActor)
	{		
		return;		
	}

	//************************************************
	// FPoseLinkBase::Update Active Pose - this is what makes 
	//the glowing line thing happen and animations loop
	//***********************************************
	BasePose.Update(Context);
}

void FAnimNode_Mirror::Evaluate_AnyThread(FPoseContext & Output)
{
	// Return Base Pose, Un Modified 
	BasePose.Evaluate(Output);
	if (!mAnimMirrorData)
	{
		return;
	}

	//Evaluate is returning the Output to this function,
	//which is returning the Output to the rest of the Anim Graph

	//In this case, we are passing the Output out variable into the BasePose

	//Basically saying, give us back the unmodified Base Pose

	//i.e, the bulk of your anim tree.
	if (Output.AnimInstanceProxy)
	{
		TArray<FCompactPoseBoneIndex> lAr;
		int32 lCurrentMirroredBoneInd = 0;
		int32 lMirBoneCount = mAnimMirrorData->GetBoneMirrorDataStructure().Num();
		int32 lpelvisBone = Output.AnimInstanceProxy->GetSkelMeshComponent()->GetBoneIndex(mAnimMirrorData->GetPelvisBoneName());
		//Mirror Mapped Bones  
		for (uint8 i = 0; i < lMirBoneCount; i += 2)
		{
			//FCompactPoseBoneIndex lInd1 = FCompactPoseBoneIndex(Output.AnimInstanceProxy->GetSkelMeshComponent()->GetBoneIndex(mAnimMirrorData->GetBoneMirrorDataStructure()[i]));
			//FCompactPoseBoneIndex lInd2 = FCompactPoseBoneIndex(Output.AnimInstanceProxy->GetSkelMeshComponent()->GetBoneIndex(mAnimMirrorData->GetBoneMirrorDataStructure()[i + 1]));

			// Fix for Array Out of Bounds
			// Thank you WILLIBOLD#3632 from discord
			FCompactPoseBoneIndex lInd1 = Output.Pose.GetBoneContainer().MakeCompactPoseIndex(FMeshPoseBoneIndex(Output.AnimInstanceProxy->GetSkelMeshComponent()->GetBoneIndex(mAnimMirrorData->GetBoneMirrorDataStructure()[i])));
			FCompactPoseBoneIndex lInd2 = Output.Pose.GetBoneContainer().MakeCompactPoseIndex(FMeshPoseBoneIndex(Output.AnimInstanceProxy->GetSkelMeshComponent()->GetBoneIndex(mAnimMirrorData->GetBoneMirrorDataStructure()[i + 1])));

			FTransform lT1 = Output.Pose[lInd1];
			FTransform lT2 = Output.Pose[lInd2];			

			MirrorPose(lT1, (uint8)mAnimMirrorData->GetBoneMirrorAxisDataStructure()[i / 2], (uint8)mAnimMirrorData->GetBoneRightAxisDataStructure()[i / 2]);
			MirrorPose(lT2, (uint8)mAnimMirrorData->GetBoneMirrorAxisDataStructure()[i / 2], (uint8)mAnimMirrorData->GetBoneRightAxisDataStructure()[i / 2]);

			if (!(uint8)mAnimMirrorData->GetBoneMirrorAxisDataStructure()[i / 2]) {
				Output.Pose[lInd1].SetRotation(Output.Pose.GetRefPose(lInd1).GetRotation() * Output.Pose.GetRefPose(lInd2).GetRotation().Inverse() * lT2.GetRotation());
				Output.Pose[lInd2].SetRotation(Output.Pose.GetRefPose(lInd2).GetRotation() * Output.Pose.GetRefPose(lInd1).GetRotation().Inverse() * lT1.GetRotation());
			}
			
			if (!(uint8)mAnimMirrorData->GetBoneRightAxisDataStructure()[i / 2]) {
				Output.Pose[lInd1].SetLocation((Output.Pose.GetRefPose(lInd2).GetRotation().Inverse() * lT2.GetRotation() * (lT2.GetLocation() - Output.Pose.GetRefPose(lInd2).GetLocation()))
					+ Output.Pose.GetRefPose(lInd1).GetLocation());

				Output.Pose[lInd2].SetLocation((Output.Pose.GetRefPose(lInd1).GetRotation().Inverse() * lT1.GetRotation() * (lT1.GetLocation() - Output.Pose.GetRefPose(lInd1).GetLocation()))
					+ Output.Pose.GetRefPose(lInd2).GetLocation());
			}

			if ((uint8)mAnimMirrorData->GetBoneMirrorAxisDataStructure()[i / 2] >= 7) {
				Output.Pose[lInd1].SetRotation(lT2.GetRotation());
				Output.Pose[lInd2].SetRotation(lT1.GetRotation());
			}
			if ((uint8)mAnimMirrorData->GetBoneRightAxisDataStructure()[i / 2] >= 7) {
				Output.Pose[lInd1].SetLocation((Output.Pose.GetRefPose(lInd2).GetRotation().Inverse() * lT2.GetRotation() * (lT2.GetLocation() - Output.Pose.GetRefPose(lInd2).GetLocation()))
					+ Output.Pose.GetRefPose(lInd1).GetLocation());

				Output.Pose[lInd2].SetLocation((Output.Pose.GetRefPose(lInd1).GetRotation().Inverse() * lT1.GetRotation() * (lT1.GetLocation() - Output.Pose.GetRefPose(lInd1).GetLocation()))
					+ Output.Pose.GetRefPose(lInd2).GetLocation());
			}

			lAr.Add(lInd1);
			lAr.Add(lInd2);

		}

		//Mirror Unmapped Bones  
		FCompactPoseBoneIndex lPoseBoneCount = FCompactPoseBoneIndex(Output.Pose.GetNumBones());

		for (FCompactPoseBoneIndex i = FCompactPoseBoneIndex(0); i < lPoseBoneCount;)
		{
			if (!lAr.Contains(i))
			{
				if (!i.IsRootBone())
				{
					FTransform lT = Output.Pose[i];
					lT.SetRotation(Output.Pose.GetRefPose(i).GetRotation().Inverse() * Output.Pose[i].GetRotation());
					lT.SetLocation(Output.Pose[i].GetLocation() - Output.Pose.GetRefPose(i).GetLocation());

					if (i.GetInt() != lpelvisBone)
					{
						MirrorPose(lT, (uint8)mAnimMirrorData->MirrorAxis_Rot, (uint8)mAnimMirrorData->RightAxis);
						Output.Pose[i].SetRotation(Output.Pose.GetRefPose(i).GetRotation() * lT.GetRotation());
						Output.Pose[i].SetLocation(Output.Pose.GetRefPose(i).GetLocation() + lT.GetLocation());
					}
					else
					{
						MirrorPose(lT, (uint8)mAnimMirrorData->PelvisMirrorAxis_Rot, (uint8)mAnimMirrorData->PelvisRightAxis);
						Output.Pose[i].SetRotation(Output.Pose.GetRefPose(i).GetRotation() * lT.GetRotation());
						Output.Pose[i].SetLocation(Output.Pose.GetRefPose(i).GetLocation() + lT.GetLocation());
					}
				}
			}
			++i;
		}
	}
}

void FAnimNode_Mirror::MirrorPose(FTransform& input_pose, const uint8 mirror_axis, const uint8 pos_fwd_mirror)
{

	FVector lMirroredLoc = input_pose.GetLocation();
	switch (pos_fwd_mirror) {
		case 1:
		case 7:
		{
			lMirroredLoc.X = -lMirroredLoc.X;
			break;
		}
		case 2:
		case 8:
		{
			lMirroredLoc.Y = -lMirroredLoc.Y;
			break;
		}
		case 3:
		case 9:
		{
			lMirroredLoc.Z = -lMirroredLoc.Z;
			break;
		}
		case 4:
		case 10:
		{
			lMirroredLoc.X = -lMirroredLoc.X;
			lMirroredLoc.Y = -lMirroredLoc.Y;
			break;
		}
		case 5:
		case 11:
		{
			lMirroredLoc.Y = -lMirroredLoc.Y;
			lMirroredLoc.Z = -lMirroredLoc.Z;
			break;
		}
		case 6:
		case 12:
		{
			lMirroredLoc.X = -lMirroredLoc.X;
			lMirroredLoc.Z = -lMirroredLoc.Z;
			break;
		}
	}
	
	input_pose.SetLocation(lMirroredLoc);


	switch (mirror_axis)
	{
		case 1:
		case 7:
		{
			const float lY = -input_pose.GetRotation().Y;
			const float lZ = -input_pose.GetRotation().Z;
			input_pose.SetRotation(FQuat(input_pose.GetRotation().X, lY, lZ, input_pose.GetRotation().W));
			break;
		}

		case 2:
		case 8:
		{
			const  float lX = -input_pose.GetRotation().X;
			const float lZ = -input_pose.GetRotation().Z;
			input_pose.SetRotation(FQuat(lX, input_pose.GetRotation().Y, lZ, input_pose.GetRotation().W));
			break;
		}

		case 3:
		case 9:
		{
			const float lX = -input_pose.GetRotation().X;
			const float lY = -input_pose.GetRotation().Y;
			input_pose.SetRotation(FQuat(lX, lY, input_pose.GetRotation().Z, input_pose.GetRotation().W));
			break;
		}
		case 4:
		case 10:
		{
			const float lZ = -input_pose.GetRotation().Z;
		
			input_pose.SetRotation(FQuat(input_pose.GetRotation().X, input_pose.GetRotation().Y, lZ, input_pose.GetRotation().W));
			break;
		}
		case 5:
		case 11:
		{
			const float lX = -input_pose.GetRotation().X;		
			input_pose.SetRotation(FQuat(lX, input_pose.GetRotation().Y, input_pose.GetRotation().Z, input_pose.GetRotation().W));
			break;
		}
		case 6:
		case 12:
		{
			const float lY = -input_pose.GetRotation().Y;
			input_pose.SetRotation(FQuat(input_pose.GetRotation().X, lY, input_pose.GetRotation().Z, input_pose.GetRotation().W));
			break;
		}		
	}
};