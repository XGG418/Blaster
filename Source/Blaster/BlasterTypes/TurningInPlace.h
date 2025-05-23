#pragma once
/// <summary>
/// 原地转向枚举
/// </summary>
UENUM(BlueprintType)
enum class ETurningInPlace :uint8
{
	ETIP_Left UMETA(DisplayName = "Turning Left"),
	ETIP_Right UMETA(DisplayName = "Turning Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),

	ETIP_Max UMETA(DisplayName = "DefaultMax")
};

