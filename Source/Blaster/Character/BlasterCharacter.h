// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "BlasterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter,public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	//播放蒙太奇

	//开火蒙太奇，供CombatComponent调用
	void PlayFireMontage(bool bAiming);
	
	//播放填弹蒙太奇
	void PlayReloadMontage();

	virtual void OnRep_ReplicatedMovement() override;

	//淘汰函数
	void Elim(bool bPlayerLeftGame);

	UFUNCTION(NetMulticast,Reliable)
	void MulticastElim(bool bPlayerLeftGame);

	//播放淘汰蒙太奇
	void PlayElimMontage();

	//播放扔手榴弹蒙太奇
	void PlayThrowGrenadeMontage();

	//播放切换武器蒙太奇
	void PlaySwapMontage();

	virtual void Destroyed() override;

	UPROPERTY(Replicated)
	bool bDisableGameplay=false;

	//狙击镜部件
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();

	void SpawnDefaultWeapon();

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	bool bFinishedSwapping = false;

	//离开游戏RPC
	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	//皇冠效果相关函数
	UFUNCTION(NetMulticast,Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	void SetTeamColor(ETeam Team);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//输入
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	//瞄准偏移
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();

	//受击蒙太奇
	void PlayHitReactMontage();

	void GrenadeButtonPressed();

	//受击委托的回调函数
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	//调查相关的类并且初始化HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);

	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();

	void SetSpawnPoint();
	void OnPlayerStateInitialized();

	//Hit boxes used for server-side rewind
	UPROPERTY(EditAnywhere)
	class UBoxComponent* head;

	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* backpack;

	UPROPERTY(EditAnywhere)
	UBoxComponent* blanket;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

private:
	UPROPERTY(VisibleAnywhere,Category=Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	//不添加meta就不能使用BluepringReadOnly
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	class UWidgetComponent* OverheadWidget;

	//在服务器上更改时，所有客户端都能改变,复制只在变量更改时才起作用
	UPROPERTY(ReplicatedUsing=OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	//需要复制的武器变量
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	//组件们
	
	//手持武器
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	//Buff组件
	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* Buff;

	//延迟补偿组件
	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;

	//Remote Procedure Calls RPC 远程过程调用
	UFUNCTION(Server,Reliable)
	void ServerEquipButtonPressed();

	//瞄准偏移变量
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	//原地转向
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	//开火蒙太奇
	UPROPERTY(EditAnywhere,Category=Combat)
	class UAnimMontage* FireWeaponMontage;

	//填弹蒙太奇
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	//受击蒙太奇
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	//淘汰蒙太奇
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;

	//扔手榴弹蒙太奇
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGrenadeMontage;

	//切换武器蒙太奇
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwapMontage;

	//相机靠近玩家隐形
	void HideCameraIfCharcterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	//代理原地转向相关
	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	//玩家生命值相关
	UPROPERTY(EditAnywhere,Category="Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing =OnRep_Health, VisibleAnywhere,Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	//玩家防御值相关
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	//淘汰相关变量
	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();

	bool bLeftGame = false;

	//溶解效果相关
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	//是一个委托
	///** Signature of function to handle timeline float track */
	//DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTimelineFloat, float, Output);
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	//Dynamic instance是可以再运行的时候更改
	UPROPERTY(VisibleAnywhere,Category=Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	//在蓝图上设置的材质实例，和dynamic material instance一起用
	UPROPERTY(VisibleAnywhere,Category=Elim)
	UMaterialInstance* DissolveMaterialInstance;

	//队伍颜色相关
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* RedDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* RedMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* BlueDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* BlueMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* OriginalMaterial;

	//淘汰扫描机器人
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;
	
	//皇冠效果
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CrownSystem;

	UPROPERTY()
	class UNiagaraComponent* CrownComponent;

/// <summary>
/// 手榴弹相关
/// </summary>
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;


/// <summary>
/// 默认自带的武器
/// </summary>
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

public:	//成员变量
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
    bool IsHoldingTheFlag() const;
	ETeam GetTeam();
	void SetHoldingTheFlag(bool bHolding);
};
