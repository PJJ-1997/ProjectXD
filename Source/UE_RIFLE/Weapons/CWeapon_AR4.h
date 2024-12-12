
#pragma once

#include "CoreMinimal.h"
#include "Weapons/CWeapon.h"
#include "CWeapon_AR4.generated.h"

UCLASS()
class UE_RIFLE_API ACWeapon_AR4 : public ACWeapon
{
	GENERATED_BODY()

public:
	ACWeapon_AR4();

protected:
    virtual void BeginPlay() override;
public:
    void  Begin_Equip() override;
    void  End_Equip() override;

public: // 1ÀÎÄª Aim
    void  Begin_Aim() override;
    void  End_Aim() override;
};
