
#include "CAnimInstance.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CPlayer.h"
#include "Weapons/CWeaponComponent.h"
#include "Kismet/KismetMathLibrary.h"


UCAnimInstance::UCAnimInstance()
{
    bIsInAir = false;
}

void UCAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    Character = Cast<ACharacter>(TryGetPawnOwner()); // 블프에서의 캐스팅(변수 세팅)

    CheckNull(Character);

    UActorComponent* comp = Character->GetComponentByClass(UCWeaponComponent::StaticClass());
    Weapon = Cast<UCWeaponComponent>(comp);

    if (Weapon)
    {
        Weapon->OnWeaponTypeChanged.AddDynamic(this, &UCAnimInstance::OnWeponTypeChanged);
    }
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    CheckNull(Character);

    ACPlayer* Player = Cast<ACPlayer>(Character);
    if (Player)
    {
        Speed = Player->Speed;
        Direction = Player->Direction;
        Pitch = Player->Pitch;
    }

    bIsInAir = Character->GetCharacterMovement()->IsFalling();

    // 점프 중일 때 팔 IK 가중치 줄이기
    if (bIsInAir)
    {
        bUseHandIK = false;  // 공중에 있을 때는 팔 IK 비활성화
    }
    else
    {
        bUseHandIK = !Weapon->IsUnarmedMode();  // 바닥에 있을 때만 무기 상태에 따라 IK 활성화
    }

    bInAim = Weapon->IsInAim();
    //bUseHandIK = !Weapon->IsUnarmedMode();
    LeftHandLocation = Weapon->GetLeftHandLocation();
}

void UCAnimInstance::OnWeponTypeChanged(EWeaponType InPrevType, EWeaponType InNewType)
{
    WeaponType = InNewType;
}