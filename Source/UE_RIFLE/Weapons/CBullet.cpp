#include "Weapons/CBullet.h"
#include "Global.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Interface/DamageInterface.h"

ACBullet::ACBullet()
{
    CHelpers::CreateComponent<UCapsuleComponent>(this, &Capsule, "Capsule");
    CHelpers::CreateComponent<UStaticMeshComponent>(this, &Mesh, "Mesh", Capsule);
    CHelpers::CreateActorComponent<UProjectileMovementComponent>(this, &Projectile, "Projectile");

    Capsule->SetRelativeRotation(FRotator(90, 0, 0));
    Capsule->SetCapsuleHalfHeight(50);
    Capsule->SetCapsuleRadius(2);
    // �ݸ��� Setting�� ���ϸ� BP�� �����ٽ� ���־�� �Ѵ�
    Capsule->SetCollisionProfileName("BlockAllDynamic");
    UStaticMesh *mesh;
    CHelpers::GetAsset<UStaticMesh>(&mesh, "StaticMesh'/Game/Meshes/Sphere.Sphere'");
    Mesh->SetStaticMesh(mesh);
    Mesh->SetRelativeScale3D(FVector(1, 0.025f, 0.025f));
    Mesh->SetRelativeRotation(FRotator(90, 0, 0));

    UMaterialInstanceConstant* material;
    CHelpers::GetAsset<UMaterialInstanceConstant>(&material, "MaterialInstanceConstant'/Game/Materials/M_Bullet_Inst.M_Bullet_Inst'");
    Mesh->SetMaterial(0, material);

    Projectile->InitialSpeed = 2e+4f;
    Projectile->MaxSpeed = 20000;
    Projectile->ProjectileGravityScale = 0;

}

void ACBullet::BeginPlay()
{
	Super::BeginPlay();
    Projectile->SetActive(false);
    Capsule->OnComponentHit.AddDynamic(this, &ACBullet::OnHit);
	
}

void ACBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
   // IDamageInterface* DamageActor = Cast<IDamageInterface>(OtherActor);
   // if (DamageActor)
   // {
   //     DamageActor->TakeDamage(Damage);  // ���÷� 10�� ������ �ο�
   // }

    Destroy();
}

void ACBullet::Shoot(const FVector & InDeirection)
{
    //// �������� ���� 3���Ŀ� �������
    //SetLifeSpan(3.0f);
    //Projectile->Velocity = InDeirection * Projectile->InitialSpeed;
    //Projectile->SetActive(true);
}

void ACBullet::SetDamage(float InDamage)
{
    Damage = InDamage;
}
