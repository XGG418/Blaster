// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterTypes/CombatState.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter= Cast<ABlasterCharacter>(TryGetPawnOwner());
	}

	if (BlasterCharacter == nullptr) return;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	//设置变量部分
	//判断是否跳跃起来了在半空中
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	//瞄准
	//只在服务器上设置了，客户端并没有设置,需复制变量
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	bIsCrouched = BlasterCharacter->bIsCrouched; //bIsCrouched已经被复制了
	bAiming = BlasterCharacter->IsAiming();

	TurningInPlace = BlasterCharacter->GetTurningInPlace();

	bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();

	bElimmed = BlasterCharacter->IsElimmed();

	bHoldingTheFlag = BlasterCharacter->IsHoldingTheFlag();

	//Offset Yaw for Strafing
	//0 直视 向右增加，向左减少
	FRotator AimRotation= BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity()); 
	//if (!BlasterCharacter->HasAuthority()&& !BlasterCharacter->IsLocallyControlled())
	//{
		//说明这些值即使在客户端上改变，服务器上也会同步改变a
		//UE_LOG(LogTemp, Warning, TEXT("AimRotation Yaw %f:"), AimRotation.Yaw);
		//UE_LOG(LogTemp, Warning, TEXT("MovementRotation Yaw %f:"), MovementRotation.Yaw);
	//}
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 15.f); //插值
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean,Target,DeltaTime,6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	//瞄准偏移
	AO_Yaw = BlasterCharacter->GetAO_Yaw();
	AO_Pitch = BlasterCharacter->GetAO_Pitch();

	//Fabrik IK 解决左手没有正确放到枪托上的问题
	if (bWeaponEquipped && EquippedWeapon&&EquippedWeapon->GetWeaponMesh()&&BlasterCharacter->GetMesh() )
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"),ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (BlasterCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation= UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
		}

		/*FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
		FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f,FColor::Red);
		DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), BlasterCharacter->GetHitTarget(), FColor::Orange);*/

		bUseFABRIK = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
		bool bFABRIKOverride = BlasterCharacter->IsLocallyReloading() && 
			BlasterCharacter->GetCombatState() != ECombatState::ECS_ThrowingGrenade && 
			BlasterCharacter->GetCombatState() != ECombatState::ECS_SwappingWeapons && 
			BlasterCharacter->bFinishedSwapping;
		if(bFABRIKOverride)
		{
			bUseFABRIK = !BlasterCharacter->IsLocallyReloading();
		}
		bUseAimOffset = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !BlasterCharacter->GetDisableGameplay();
		bTransformRightHand = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !BlasterCharacter->GetDisableGameplay();
	}
}
