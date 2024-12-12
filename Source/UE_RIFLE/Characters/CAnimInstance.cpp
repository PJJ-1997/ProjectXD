
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
    Character = Cast<ACharacter>(TryGetPawnOwner()); // ���������� ĳ����(���� ����)

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

    // ���� ���� �� �� IK ����ġ ���̱�
    if (bIsInAir)
    {
        bUseHandIK = false;  // ���߿� ���� ���� �� IK ��Ȱ��ȭ
    }
    else
    {
        bUseHandIK = !Weapon->IsUnarmedMode();  // �ٴڿ� ���� ���� ���� ���¿� ���� IK Ȱ��ȭ
    }

    bInAim = Weapon->IsInAim();
    //bUseHandIK = !Weapon->IsUnarmedMode();
    LeftHandLocation = Weapon->GetLeftHandLocation();
}

void UCAnimInstance::OnWeponTypeChanged(EWeaponType InPrevType, EWeaponType InNewType)
{
    WeaponType = InNewType;
}