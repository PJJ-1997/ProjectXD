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

    // Base로 두고 파생 시켜서 4개의 캐릭터로 만들기 때문에 주석처리

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

    MaxWalkSpeed = 400.0f;  // 기본 걷기 속도 초기화
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
    if (HasAuthority())  // 서버에서만 처리
    {
        Health -= DamageAmount;

        Health = FMath::Clamp(Health, 0.0f, 100.0f);

        if (PlayerHUDWidget)  // C++ HP 위젯 업데이트
        {
            PlayerHUDWidget->UpdateHealth(Health);
        }

        // 블루프린트 체력바 업데이트 호출
        UpdateBlueprintHealthBar(Health);

        if (Health <= 0.0f)
        {
            State->SetDeadMode();  // 캐릭터 죽음 처리
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
    // 카메라 방향으로 회전 처리
    FRotator NewRotation = UGameplayStatics::GetPlayerController(this, 0)->PlayerCameraManager->GetCameraRotation();
    NewRotation.Pitch = 0.0f; // 캐릭터는 수평으로만 회전

    // 로컬에서 회전 적용
    SetActorRotation(NewRotation);

    // 서버에 회전 데이터 전송
    ServerRotate(NewRotation);

    // 회전 목표 업데이트
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
    // 서버에서 회전 처리 및 클라이언트에 전송
    SetActorRotation(NewRotation);
    MulticastRotate(NewRotation);
}

bool ACPlayer::ServerRotate_Validate(FRotator NewRotation)
{
    // 검증 로직 (필요한 경우 추가)
    return true;
}

void ACPlayer::MulticastRotate_Implementation(FRotator NewRotation)
{
    // 다른 클라이언트에서 회전 목표 업데이트
    if (!IsLocallyControlled())
    {
        TargetRotation = NewRotation;
    }

    // 모든 클라이언트에서 회전 적용
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
    // Speed 리플리케이션이 변경되었을 때 호출되는 함수
}

void ACPlayer::OnRep_Direction()
{
    // Direction 리플리케이션이 변경되었을 때 호출되는 함수
}

void ACPlayer::OnRep_Pitch()
{
    // Pitch 리플리케이션이 변경되었을 때 호출되는 함수
}

void ACPlayer::OnRep_Health()
{
    // 클라이언트에서 Health 변경 시 이펙트 재생
    if (Effect)
    {
        Effect->PlayEffect(GetActorLocation(), GetActorRotation());
        Effect->PlaySound(GetActorLocation());
    }

    // HUD 업데이트
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
    if (HasAuthority()) // 서버에서만 실행
    {
        Multicast_PlayDeathMontage();
    }
}

void ACPlayer::DropWeapon()
{
    if (Weapon && Weapon->GetCurrentWeapon())
    {
        ACWeapon* DroppedWeapon = Weapon->GetCurrentWeapon();

        // 드랍 전 총알 상태 저장
        DroppedWeapon->SaveAmmo(DroppedWeapon->CurrMagazineCount, DroppedWeapon->CurrentAmmo);

        FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
        FRotator DropRotation = GetActorRotation();

        if (HasAuthority())
        {
            // 총알 정보 전달하며 드랍
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
        // 드롭할 위치, 회전 및 무기 유형 가져오기
        FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
        FRotator DropRotation = GetActorRotation();
        TSubclassOf<ACWeapon> WeaponClass = Weapon->GetCurrentWeapon()->GetClass();
        EWeaponType DroppedWeaponType = Weapon->GetCurrentWeapon()->GetWeaponType();

        // 현재 무기의 총알 정보 가져오기
        int32 MagazineCount = Weapon->GetCurrentWeapon()->CurrMagazineCount;
        int32 AmmoCount = Weapon->GetCurrentWeapon()->CurrentAmmo;

        // 로그 출력 (디버깅용)
        UE_LOG(LogTemp, Log, TEXT("Server_DropWeapon: Dropping weapon with ammo: MagazineCount = %d, AmmoCount = %d"),
            MagazineCount, AmmoCount);

        // 멀티캐스트를 통해 모든 클라이언트에 무기 드롭 상태 동기화
        Multicast_DropWeapon(DropLocation, DropRotation, WeaponClass, DroppedWeaponType, MagazineCount, AmmoCount);

        // 모든 클라이언트에 비무장 상태 동기화
        Multicast_SetUnarmedMode();

        // 모든 클라이언트에 무기 드롭 타입 동기화
        Multicast_AddDroppedWeapon(DroppedWeaponType);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_DropWeapon: No weapon to drop."));
    }
}

bool ACPlayer::Server_DropWeapon_Validate()
{
    return true;  // 서버 호출이 유효한지 확인 (간단히 true 반환 가능)
}



// 모든 클라이언트에 무기 드롭 상태를 동기화하는 멀티캐스트 함수
void ACPlayer::Multicast_DropWeapon_Implementation(FVector DropLocation, FRotator DropRotation, TSubclassOf<ACWeapon> WeaponClass, EWeaponType DroppedWeaponType, int32 MagazineCount, int32 AmmoCount)
{
    if (Weapon && Weapon->GetCurrentWeapon())
    {
        // 현재 무기의 콜리전을 비활성화
        Weapon->GetCurrentWeapon()->DeactivateCollision();

        // 총알 정보 저장
        int32 MagazineCount = Weapon->GetCurrentWeapon()->CurrMagazineCount;
        int32 AmmoCount = Weapon->GetCurrentWeapon()->CurrentAmmo;
        Weapon->GetCurrentWeapon()->SaveAmmo(MagazineCount, AmmoCount);

        // 새로운 무기 액터를 생성
        ACWeapon* DroppedWeapon = GetWorld()->SpawnActor<ACWeapon>(WeaponClass, DropLocation, DropRotation);
        if (DroppedWeapon)
        {
            // 드롭된 무기의 물리 및 콜리전 설정
            DroppedWeapon->SetActorEnableCollision(true);
            DroppedWeapon->Mesh->SetVisibility(true);
            DroppedWeapon->Mesh->SetSimulatePhysics(true);
            DroppedWeapon->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            DroppedWeapon->Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

            // 드롭된 무기의 콜리전 활성화
            DroppedWeapon->ActivateCollision();

            // 드롭된 무기에 총알 정보를 전달
            DroppedWeapon->CurrMagazineCount = MagazineCount;
            DroppedWeapon->CurrentAmmo = AmmoCount;

            // 로그 출력
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


// 모든 클라이언트에서 비무장 상태를 설정하는 멀티캐스트 함수
void ACPlayer::Multicast_SetUnarmedMode_Implementation()
{
    if (Weapon)
    {
        Weapon->SetUnarmedMode(); // 모든 클라이언트에서 비무장 상태로 설정
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
            Weapon->RemoveDroppedWeapon(OverlappedWeapon->GetWeaponType()); // 드롭된 무기 배열에서 제거
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
    // 캐릭터의 WeaponComponent를 가져와서 Unarmed 모드로 전환
    UCWeaponComponent* WeaponComp = CHelpers::GetComponent<UCWeaponComponent>(this);
    if (WeaponComp == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponComp is NULL in End_Dead"));
        return;
    }
    WeaponComp->SetUnarmedMode();
    GetWorldTimerManager().SetTimerForNextTick(this, &ACPlayer::DestroyCharacter);
}