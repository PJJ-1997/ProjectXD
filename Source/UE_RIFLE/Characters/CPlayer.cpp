#include "CPlayer.h"
#include "Global.h"
#include "CAnimInstance.h"
#include "CAnimInstance_Arms.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Component/CStateComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/EffectComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Weapons/CWeaponComponent.h"
#include "Weapons/CWeapon.h"
#include "Widgets/CUserWidget_HUD.h"
#include "Widgets/CUserWidget_CrossHair.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Instance/CreateGameInstance.h"
#include "Net/UnrealNetwork.h" 
#include "Engine/Engine.h"

// Sets default values
ACPlayer::ACPlayer()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->JumpZVelocity = 500.0f;
    JumpMaxCount = 1;
    JumpMaxHoldTime = 3.0f;

    CHelpers::CreateComponent<USpringArmComponent>(this, &SpringArm, "SpringArm", GetMesh());
    CHelpers::CreateComponent<UCameraComponent>(this, &Camera, "Camera", SpringArm);
    CHelpers::CreateComponent<UStaticMeshComponent>(this, &Backpack, "Backpack", GetMesh(), "Backpack");
    CHelpers::CreateComponent<USkeletalMeshComponent>(this, &Arms, "Arms", Camera);
    CHelpers::CreateActorComponent<UCStateComponent>(this, &State, "State");
    CHelpers::CreateActorComponent<UCWeaponComponent>(this, &Weapon, "Weapon");
    CHelpers::CreateActorComponent<UEffectComponent>(this, &Effect, "Effect");

    // Base�� �ΰ� �Ļ� ���Ѽ� 4���� ĳ���ͷ� ����� ������ �ּ�ó��

    // USkeletalMesh* mesh;
    // {
    //     CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/Character/Mesh/SK_Mannequin.SK_Mannequin'");
    //     GetMesh()->SetSkeletalMesh(mesh);
    //     GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
    //     GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
    //
    //     TSubclassOf<UCAnimInstance> animInstance;
    //     CHelpers::GetClass<UCAnimInstance>(&animInstance, "AnimBlueprint'/Game/Player/ABP_Character.ABP_Character_C'");
    //     GetMesh()->SetAnimClass(animInstance);
    // }
    //
    // {
    //     CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/Character_Arms/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'");
    //     Arms->SetSkeletalMesh(mesh);
    //     Arms->SetVisibility(false);
    //
    //     TSubclassOf<UCAnimInstance_Arms> animInstance;
    //     CHelpers::GetClass<UCAnimInstance_Arms>(&animInstance, "AnimBlueprint'/Game/Player/ABP_Character_Arms.ABP_Character_Arms_C'");
    //     Arms->SetAnimClass(animInstance);
    // }

    UStaticMesh* staticMesh;
    CHelpers::GetAsset<UStaticMesh>(&staticMesh, "StaticMesh'/Game/FPS_Weapon_Bundle/Backpack/Backpack.Backpack'");
    Backpack->SetStaticMesh(staticMesh);
    Backpack->SetVisibility(false);

    this->bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    MaxWalkSpeed = 400.0f;  // �⺻ �ȱ� �ӵ� �ʱ�ȭ
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

    SpringArm->SetRelativeLocation(FVector(0, 0, 150));
    SpringArm->SetRelativeRotation(FRotator(0, 90, 0));
    SpringArm->TargetArmLength = 380;
    SpringArm->bUsePawnControlRotation = true;

    Camera->SetRelativeLocation(FVector(-30, 0, 0));
    Camera->bUsePawnControlRotation = true;

    NetUpdateFrequency = 120.0f;
    MinNetUpdateFrequency = 30.0f;
    NetPriority = 3.0f;
    SmoothRotationSpeed = 10.0f;
    bReplicates = true;
    SetReplicateMovement(true);
}

void  ACPlayer::OnStateTypeChanged(EStateType InPrevType, EStateType InNewType)
{
    switch (InNewType)
    {
    case EStateType::Idle:      break;
    case EStateType::Dead:      break;
    case EStateType::Roll:     Roll();     break;
    case EStateType::Hit:      Hitted();   break;
    default:

        break;
    }
}

void ACPlayer::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPlayer::InitializeHUD, 0.1f, false);



    if (!GetController())
    {
        return;
    }

    if (Weapon == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Weapon component is not initialized!"));
    }

    APlayerController* PlayerController = GetController<APlayerController>();
    if (PlayerController)
    {
        PlayerController->PlayerCameraManager->ViewPitchMin = PitchRange.X;
        PlayerController->PlayerCameraManager->ViewPitchMax = PitchRange.Y;
    }

    TArray<UMaterialInterface*> materials = GetMesh()->GetMaterials();
    for (int32 i = 0; i < materials.Num(); i++)
    {
        UMaterialInstanceDynamic* temp;
        temp = UMaterialInstanceDynamic::Create(materials[i], this);
        GetMesh()->SetMaterial(i, temp);
        Materials.Add(temp);
    }

    MaxWalkSpeed = 400.0f;
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

    if (GetCapsuleComponent())
    {
        GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACPlayer::OnOverlapBegin);
    }
}

void ACPlayer::InitializeHUD()
{
    APlayerController* PlayerController = GetController<APlayerController>();
    if (PlayerController && HPClass)
    {
        PlayerHUDWidget = CreateWidget<UCUserWidget_HUD>(PlayerController, HPClass);
        if (PlayerHUDWidget)
        {
            PrintLine();
            PlayerHUDWidget->AddToViewport();
            PlayerHUDWidget->SetVisibility(ESlateVisibility::Visible);
            PlayerHUDWidget->UpdateHealth(Health);
        }
    }
}

void ACPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    if (IsLocallyControlled())
    {
        HandleLocalRotation(DeltaTime);
    }
    else
    {
        SmoothClientRotation(DeltaTime);
    }

    //GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
    UpdateAnimationVariables();
}

// Called to bind functionality to input
void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ACPlayer::OnMoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ACPlayer::OnMoveRight);
    PlayerInputComponent->BindAxis("HorizontalLook", this, &ACPlayer::OnHorizontalLook);
    PlayerInputComponent->BindAxis("VerticalLook", this, &ACPlayer::OnVerticalLook);

    PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &ACPlayer::OnRun);
    PlayerInputComponent->BindAction("Run", EInputEvent::IE_Released, this, &ACPlayer::OffRun);
    PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACPlayer::Jump);
    PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACPlayer::StopJumping);
    PlayerInputComponent->BindAction("DropWeapon", EInputEvent::IE_Pressed, this, &ACPlayer::DropWeapon);

    PlayerInputComponent->BindAction("AR4", EInputEvent::IE_Pressed, Weapon, &UCWeaponComponent::SetAR4Mode);
    PlayerInputComponent->BindAction("AK47", EInputEvent::IE_Pressed, Weapon, &UCWeaponComponent::SetAK47Mode);
    PlayerInputComponent->BindAction("Pistol", EInputEvent::IE_Pressed, Weapon, &UCWeaponComponent::SetGunMode);
    PlayerInputComponent->BindAction("Sniper", EInputEvent::IE_Pressed, Weapon, &UCWeaponComponent::SetSniperMode);

    PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, Weapon, &UCWeaponComponent::Begin_Fire);
    PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, Weapon, &UCWeaponComponent::End_Fire);

    PlayerInputComponent->BindAction("Aiming", EInputEvent::IE_Pressed, Weapon, &UCWeaponComponent::Begin_Aim);
    PlayerInputComponent->BindAction("Aiming", EInputEvent::IE_Released, Weapon, &UCWeaponComponent::End_Aim);

    PlayerInputComponent->BindAction("AutoFire", EInputEvent::IE_Pressed, Weapon, &UCWeaponComponent::ToggleAutoFire);
    PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, Weapon, &UCWeaponComponent::Reload);

    PlayerInputComponent->BindAction("Widget", EInputEvent::IE_Pressed, Weapon, &UCWeaponComponent::CreateHUD);

}

void ACPlayer::TakeDamage(FHitResult HitResult, float DamageAmount)
{
    if (HasAuthority())  // ���������� ó��
    {
        Health -= DamageAmount;

        Health = FMath::Clamp(Health, 0.0f, 100.0f);

        if (PlayerHUDWidget)  // C++ HP ���� ������Ʈ
        {
            PlayerHUDWidget->UpdateHealth(Health);
        }

        // �������Ʈ ü�¹� ������Ʈ ȣ��
        UpdateBlueprintHealthBar(Health);

        if (Health <= 0.0f)
        {
            State->SetDeadMode();  // ĳ���� ���� ó��
            Dead();
        }

        OnRep_Health();
    }
}

void ACPlayer::OnMoveForward(float InAxisValue)
{
    FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
    FVector direction = FQuat(rotator).GetForwardVector().GetSafeNormal2D();

    AddMovementInput(direction, InAxisValue);
}

void ACPlayer::OnMoveRight(float InAxisValue)
{
    FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
    FVector direction = FQuat(rotator).GetRightVector().GetSafeNormal2D();

    AddMovementInput(direction, InAxisValue);
}

void ACPlayer::OnHorizontalLook(float InAxisValue)
{
    AddControllerYawInput(InAxisValue);

    if (HasAuthority())
    {
        CharacterRotation = GetActorRotation();
    }
    else
    {
        ServerRotate(GetActorRotation());
    }
}

void ACPlayer::OnVerticalLook(float InAxisValue)
{
    AddControllerPitchInput(InAxisValue);

    if (HasAuthority())
    {
        CharacterRotation = GetActorRotation();
    }
    else
    {
        ServerRotate(GetActorRotation());
    }
}

void ACPlayer::OnRun()
{
    if (HasAuthority())
    {
        MaxWalkSpeed = 600.0f;
        GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
    }
    else
    {
        Server_SetMaxWalkSpeed(600.0f);
        GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    }
}

void ACPlayer::OffRun()
{
    if (HasAuthority())
    {
        MaxWalkSpeed = 400.0f;
        GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
    }
    else
    {
        Server_SetMaxWalkSpeed(400.0f);
        GetCharacterMovement()->MaxWalkSpeed = 400.0f;
    }
}

void ACPlayer::HandleLocalRotation(float DeltaTime)
{
    // ī�޶� �������� ȸ�� ó��
    FRotator NewRotation = UGameplayStatics::GetPlayerController(this, 0)->PlayerCameraManager->GetCameraRotation();
    NewRotation.Pitch = 0.0f; // ĳ���ʹ� �������θ� ȸ��

    // ���ÿ��� ȸ�� ����
    SetActorRotation(NewRotation);

    // ������ ȸ�� ������ ����
    ServerRotate(NewRotation);

    // ȸ�� ��ǥ ������Ʈ
    TargetRotation = NewRotation;
}

void ACPlayer::SmoothClientRotation(float DeltaTime)
{
    FRotator CurrentRotation = GetActorRotation();
    FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, SmoothRotationSpeed);
    SetActorRotation(SmoothedRotation);
}

void ACPlayer::ServerRotate_Implementation(FRotator NewRotation)
{
    // �������� ȸ�� ó�� �� Ŭ���̾�Ʈ�� ����
    SetActorRotation(NewRotation);
    MulticastRotate(NewRotation);
}

bool ACPlayer::ServerRotate_Validate(FRotator NewRotation)
{
    // ���� ���� (�ʿ��� ��� �߰�)
    return true;
}

void ACPlayer::MulticastRotate_Implementation(FRotator NewRotation)
{
    // �ٸ� Ŭ���̾�Ʈ���� ȸ�� ��ǥ ������Ʈ
    if (!IsLocallyControlled())
    {
        TargetRotation = NewRotation;
    }

    // ��� Ŭ���̾�Ʈ���� ȸ�� ����
    SetActorRotation(NewRotation);
}

void ACPlayer::Server_SetMaxWalkSpeed_Implementation(float NewSpeed)
{
    MaxWalkSpeed = NewSpeed;
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

bool ACPlayer::Server_SetMaxWalkSpeed_Validate(float NewSpeed)
{
    return true;
}


void ACPlayer::UpdateAnimationVariables()
{
    Speed = GetVelocity().Size2D();
    FRotator rotator = GetVelocity().ToOrientationRotator();  // Player Angle
    FRotator rotator2 = GetControlRotation();                 // Camera
    FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(rotator, rotator2);  // 0~180,0~180

    static FRotator PrevRotation = FRotator::ZeroRotator;
    PrevRotation = UKismetMathLibrary::RInterpTo(PrevRotation, delta, GetWorld()->GetDeltaSeconds(), 125);
    Direction = PrevRotation.Yaw;

    float RawPitch = GetBaseAimRotation().Pitch;
    RawPitch = FMath::ClampAngle(RawPitch, -90.0f, 90.0f);
    Pitch = UKismetMathLibrary::FInterpTo(Pitch, RawPitch, GetWorld()->GetDeltaSeconds(), 25);
}

void ACPlayer::OnRep_Speed()
{
    // Speed ���ø����̼��� ����Ǿ��� �� ȣ��Ǵ� �Լ�
}

void ACPlayer::OnRep_Direction()
{
    // Direction ���ø����̼��� ����Ǿ��� �� ȣ��Ǵ� �Լ�
}

void ACPlayer::OnRep_Pitch()
{
    // Pitch ���ø����̼��� ����Ǿ��� �� ȣ��Ǵ� �Լ�
}

void ACPlayer::OnRep_Health()
{
    // Ŭ���̾�Ʈ���� Health ���� �� ����Ʈ ���
    if (Effect)
    {
        Effect->PlayEffect(GetActorLocation(), GetActorRotation());
        Effect->PlaySound(GetActorLocation());
    }

    // HUD ������Ʈ
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->UpdateHealth(Health);
    }
}

void ACPlayer::OnRep_MaxWalkSpeed()
{
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ACPlayer::MulticastPlayHitEffect_Implementation()
{
    if (Effect)
    {
        Effect->PlayEffect(GetActorLocation(), GetActorRotation());
        Effect->PlaySound(GetActorLocation());
    }
}

void ACPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACPlayer, CharacterRotation);
    DOREPLIFETIME(ACPlayer, MaxWalkSpeed);
    DOREPLIFETIME(ACPlayer, Speed);
    DOREPLIFETIME(ACPlayer, Direction);
    DOREPLIFETIME(ACPlayer, Pitch);
    DOREPLIFETIME(ACPlayer, Health);
}

void ACPlayer::Roll()
{
}

void ACPlayer::Hitted()
{
}

void ACPlayer::Dead()
{
    if (HasAuthority()) // ���������� ����
    {
        Multicast_PlayDeathMontage();
    }
}

void ACPlayer::DropWeapon()
{
    if (Weapon && Weapon->GetCurrentWeapon())
    {
        ACWeapon* DroppedWeapon = Weapon->GetCurrentWeapon();

        // ��� �� �Ѿ� ���� ����
        DroppedWeapon->SaveAmmo(DroppedWeapon->CurrMagazineCount, DroppedWeapon->CurrentAmmo);

        FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
        FRotator DropRotation = GetActorRotation();

        if (HasAuthority())
        {
            // �Ѿ� ���� �����ϸ� ���
            Multicast_DropWeapon(DropLocation, DropRotation, DroppedWeapon->GetClass(), DroppedWeapon->GetWeaponType(),
                DroppedWeapon->CurrMagazineCount, DroppedWeapon->CurrentAmmo);
            Multicast_SetUnarmedMode();
            Multicast_AddDroppedWeapon(DroppedWeapon->GetWeaponType());
        }
        else
        {
            Server_DropWeapon();
        }
    }
}

void ACPlayer::Server_DropWeapon_Implementation()
{
    if (Weapon && Weapon->GetCurrentWeapon())
    {
        // ����� ��ġ, ȸ�� �� ���� ���� ��������
        FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
        FRotator DropRotation = GetActorRotation();
        TSubclassOf<ACWeapon> WeaponClass = Weapon->GetCurrentWeapon()->GetClass();
        EWeaponType DroppedWeaponType = Weapon->GetCurrentWeapon()->GetWeaponType();

        // ���� ������ �Ѿ� ���� ��������
        int32 MagazineCount = Weapon->GetCurrentWeapon()->CurrMagazineCount;
        int32 AmmoCount = Weapon->GetCurrentWeapon()->CurrentAmmo;

        // �α� ��� (������)
        UE_LOG(LogTemp, Log, TEXT("Server_DropWeapon: Dropping weapon with ammo: MagazineCount = %d, AmmoCount = %d"),
            MagazineCount, AmmoCount);

        // ��Ƽĳ��Ʈ�� ���� ��� Ŭ���̾�Ʈ�� ���� ��� ���� ����ȭ
        Multicast_DropWeapon(DropLocation, DropRotation, WeaponClass, DroppedWeaponType, MagazineCount, AmmoCount);

        // ��� Ŭ���̾�Ʈ�� ���� ���� ����ȭ
        Multicast_SetUnarmedMode();

        // ��� Ŭ���̾�Ʈ�� ���� ��� Ÿ�� ����ȭ
        Multicast_AddDroppedWeapon(DroppedWeaponType);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_DropWeapon: No weapon to drop."));
    }
}

bool ACPlayer::Server_DropWeapon_Validate()
{
    return true;  // ���� ȣ���� ��ȿ���� Ȯ�� (������ true ��ȯ ����)
}



// ��� Ŭ���̾�Ʈ�� ���� ��� ���¸� ����ȭ�ϴ� ��Ƽĳ��Ʈ �Լ�
void ACPlayer::Multicast_DropWeapon_Implementation(FVector DropLocation, FRotator DropRotation, TSubclassOf<ACWeapon> WeaponClass, EWeaponType DroppedWeaponType, int32 MagazineCount, int32 AmmoCount)
{
    if (Weapon && Weapon->GetCurrentWeapon())
    {
        // ���� ������ �ݸ����� ��Ȱ��ȭ
        Weapon->GetCurrentWeapon()->DeactivateCollision();

        // �Ѿ� ���� ����
        int32 MagazineCount = Weapon->GetCurrentWeapon()->CurrMagazineCount;
        int32 AmmoCount = Weapon->GetCurrentWeapon()->CurrentAmmo;
        Weapon->GetCurrentWeapon()->SaveAmmo(MagazineCount, AmmoCount);

        // ���ο� ���� ���͸� ����
        ACWeapon* DroppedWeapon = GetWorld()->SpawnActor<ACWeapon>(WeaponClass, DropLocation, DropRotation);
        if (DroppedWeapon)
        {
            // ��ӵ� ������ ���� �� �ݸ��� ����
            DroppedWeapon->SetActorEnableCollision(true);
            DroppedWeapon->Mesh->SetVisibility(true);
            DroppedWeapon->Mesh->SetSimulatePhysics(true);
            DroppedWeapon->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            DroppedWeapon->Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

            // ��ӵ� ������ �ݸ��� Ȱ��ȭ
            DroppedWeapon->ActivateCollision();

            // ��ӵ� ���⿡ �Ѿ� ������ ����
            DroppedWeapon->CurrMagazineCount = MagazineCount;
            DroppedWeapon->CurrentAmmo = AmmoCount;

            // �α� ���
            UE_LOG(LogTemp, Log, TEXT("Dropped weapon %s with ammo: MagazineCount = %d, CurrentAmmo = %d"),
                *DroppedWeapon->GetName(), MagazineCount, AmmoCount);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to spawn dropped weapon."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No weapon equipped to drop."));
    }
}


// ��� Ŭ���̾�Ʈ���� ���� ���¸� �����ϴ� ��Ƽĳ��Ʈ �Լ�
void ACPlayer::Multicast_SetUnarmedMode_Implementation()
{
    if (Weapon)
    {
        Weapon->SetUnarmedMode(); // ��� Ŭ���̾�Ʈ���� ���� ���·� ����
    }
}

void ACPlayer::Multicast_AddDroppedWeapon_Implementation(EWeaponType DroppedWeaponType)
{
    if (Weapon)
    {
        Weapon->AddDroppedWeapon(DroppedWeaponType);
    }
}

void ACPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA(ACWeapon::StaticClass()))
    {
        ACWeapon* OverlappedWeapon = Cast<ACWeapon>(OtherActor);
        if (Weapon && Weapon->IsWeaponDropped(OverlappedWeapon->GetWeaponType()))
        {
            Weapon->RemoveDroppedWeapon(OverlappedWeapon->GetWeaponType()); // ��ӵ� ���� �迭���� ����
        }
    }

}

void ACPlayer::DestroyCharacter()
{
    //Destroy();
}

void ACPlayer::Multicast_PlayDeathMontage_Implementation()
{
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        if (DeathMontage)
        {
            AnimInstance->Montage_Play(DeathMontage, 1.0f);
        }
    }
}

void ACPlayer::End_Roll()
{
}

void ACPlayer::End_Hit()
{
}

void ACPlayer::End_Dead()
{
    // ĳ������ WeaponComponent�� �����ͼ� Unarmed ���� ��ȯ
    UCWeaponComponent* WeaponComp = CHelpers::GetComponent<UCWeaponComponent>(this);
    if (WeaponComp == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponComp is NULL in End_Dead"));
        return;
    }
    WeaponComp->SetUnarmedMode();
    GetWorldTimerManager().SetTimerForNextTick(this, &ACPlayer::DestroyCharacter);
}