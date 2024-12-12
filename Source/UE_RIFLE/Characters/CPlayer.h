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

    //�������Ʈ Ȯ���ϱ� ���� public���� ������ BluePrintReadOnly �߰�
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

    // ������ �̺�Ʈ �Լ� - ��ӵ� ���⸦ �ٽ� ȸ���ϱ� ���� �Լ�
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:
    // ���������� ����Ǵ� ���� ��� �Լ�
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_DropWeapon();
    void Server_DropWeapon_Implementation();
    bool Server_DropWeapon_Validate();

    // ��� Ŭ���̾�Ʈ�� ���� ��� ���¸� ����ȭ�ϴ� ��Ƽĳ��Ʈ �Լ�
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_DropWeapon(FVector DropLocation, FRotator DropRotation, TSubclassOf<ACWeapon> WeaponClass, EWeaponType DroppedWeaponType, int32 MagazineCount, int32 AmmoCount);


    // ��� Ŭ���̾�Ʈ���� ���� ���¸� �����ϴ� ��Ƽĳ��Ʈ �Լ�
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetUnarmedMode();

    // ��� Ŭ���̾�Ʈ���� ��ӵ� ���� Ÿ���� ����ȭ�ϴ� ��Ƽĳ��Ʈ �Լ�
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_AddDroppedWeapon(EWeaponType DroppedWeaponType);

public:  // ��Ƽ���̿��� ȣ��
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
    // �ִϸ��̼� ����
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