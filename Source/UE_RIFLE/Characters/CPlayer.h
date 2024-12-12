#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Component/EffectComponent.h"
#include "Component/CStateComponent.h"
#include "Animation/AnimMontage.h"
#include "Interface/DamageInterface.h"
#include "Interface/ICharacter.h"
#include "Widgets/CUserWidget_CrossHair.h"
#include "CPlayer.generated.h"

UCLASS()
class UE_RIFLE_API ACPlayer : public ACharacter, public IDamageInterface, public IICharacter
{
    GENERATED_BODY()

public:
    ACPlayer();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    virtual void  TakeDamage(FHitResult HitResult, float DamageAmount) override;

public:
    UFUNCTION()
    void  OnStateTypeChanged(EStateType InPrevType, EStateType InNewType);

    UFUNCTION(BlueprintImplementableEvent, Category = "Health")
    void UpdateBlueprintHealthBar(float CurrentHealth);

    //블루프린트 확인하기 위해 public으로 빼놓고 BluePrintReadOnly 추가
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BluePrintReadOnly)
    class UCWeaponComponent* Weapon;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class USpringArmComponent* SpringArm;

    UPROPERTY(EditAnywhere)
    class UCStateComponent* State;
private:
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* DeathMontage;

    UPROPERTY(EditAnywhere)
    FVector2D PitchRange = FVector2D(-40.0f, +45.0f);


    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* Backpack;

    UPROPERTY(VisibleAnywhere)
    class USkeletalMeshComponent* Arms;

    UPROPERTY(EditAnywhere)
    UEffectComponent* Effect;

    UPROPERTY(EditAnywhere, Category = "Settings")
    TSubclassOf<class UCUserWidget_HUD> HPClass;

    UPROPERTY()
    UCUserWidget_HUD* PlayerHUDWidget;

public:
    void  Roll();
    void  Hitted();
    void  Dead();
    void  DropWeapon();
    void DestroyCharacter();
    void HandleDropWeapon();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayDeathMontage();

    // 오버랩 이벤트 함수 - 드롭된 무기를 다시 회수하기 위한 함수
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:
    // 서버에서만 실행되는 무기 드롭 함수
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_DropWeapon();
    void Server_DropWeapon_Implementation();
    bool Server_DropWeapon_Validate();

    // 모든 클라이언트에 무기 드롭 상태를 동기화하는 멀티캐스트 함수
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_DropWeapon(FVector DropLocation, FRotator DropRotation, TSubclassOf<ACWeapon> WeaponClass, EWeaponType DroppedWeaponType, int32 MagazineCount, int32 AmmoCount);


    // 모든 클라이언트에서 비무장 상태를 설정하는 멀티캐스트 함수
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetUnarmedMode();

    // 모든 클라이언트에서 드롭된 무기 타입을 동기화하는 멀티캐스트 함수
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_AddDroppedWeapon(EWeaponType DroppedWeaponType);

public:  // 노티파이에서 호출
    virtual void End_Roll() override;
    virtual void End_Hit()   override;
    virtual void End_Dead()  override;


    void OnMoveForward(float InAxisValue);
    void OnMoveRight(float InAxisValue);
    void OnHorizontalLook(float InAxisValue);
    void OnVerticalLook(float InAxisValue);

    void OnRun();
    void OffRun();

    void InitializeHUD();

private:
    void HandleLocalRotation(float DeltaTime);
    void SmoothClientRotation(float DeltaTime);

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRotate(FRotator NewRotation);
    void ServerRotate_Implementation(FRotator NewRotation);
    bool ServerRotate_Validate(FRotator NewRotation);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastRotate(FRotator NewRotation);
    void MulticastRotate_Implementation(FRotator NewRotation);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayHitEffect();

    FRotator TargetRotation;
    FRotator LastRotation;
    float SmoothRotationSpeed;

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SetMaxWalkSpeed(float NewSpeed);
    void Server_SetMaxWalkSpeed_Implementation(float NewSpeed);
    bool Server_SetMaxWalkSpeed_Validate(float NewSpeed);

    UPROPERTY(Replicated)
    FRotator CharacterRotation;

    UPROPERTY(ReplicatedUsing = OnRep_MaxWalkSpeed)
    float MaxWalkSpeed;

    TArray<class UMaterialInstanceDynamic*> Materials;

public:
    // 애니메이션 변수
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Speed, Category = "Animation")
    float Speed;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Direction, Category = "Animation")
    float Direction;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Pitch, Category = "Animation")
    float Pitch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Health, Category = "Health", meta = (AllowPrivateAccess = "true"))
    float Health = 100.0f;

private:
    void UpdateAnimationVariables();

    UFUNCTION()
    void OnRep_Speed();

    UFUNCTION()
    void OnRep_Direction();

    UFUNCTION()
    void OnRep_Pitch();

    UFUNCTION()
    void OnRep_Health();

    UFUNCTION()
    void OnRep_MaxWalkSpeed();
public:
    FORCEINLINE class UStaticMeshComponent* GetBackpack() { return Backpack; }
    FORCEINLINE class USkeletalMeshComponent* GetArms() { return Arms; }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};