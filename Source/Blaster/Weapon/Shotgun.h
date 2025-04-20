// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 霰弹枪
 */
UCLASS()
class BLASTER_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()
	
public:
	/*virtual void Fire(const FVector& HitTarget) override;*/
	virtual void FireShotgun(const TArray<FVector_NetQuantize>& HitTargets);
	void ShotgunTraceEndWithScatter(const FVector& HitTarget,TArray<FVector_NetQuantize>& HitTargets);

private:
	//猎枪射出的金属碎片的数量
	UPROPERTY(EditAnywhere,Category="Weapon Scatter")
	uint32 NumberOfPellets = 10;
};
