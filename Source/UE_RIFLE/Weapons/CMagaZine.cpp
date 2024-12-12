
#include "Weapons/CMagaZine.h"
#include "Global.h"
#include "Components/StaticMeshComponent.h"


ACMagaZine::ACMagaZine()
{
    CHelpers::CreateComponent<USceneComponent>(this, &Root, "Root");
    CHelpers::CreateComponent<UStaticMeshComponent>(this, &Mesh_Full, "Mesh_Full", Root);
    CHelpers::CreateComponent<UStaticMeshComponent>(this, &Mesh_Empty, "Mesh_Empty", Root);


    UStaticMesh* mesh;
    CHelpers::GetAsset<UStaticMesh>(&mesh, "StaticMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SM_AR4_Mag.SM_AR4_Mag'");
    Mesh_Full->SetStaticMesh(mesh);
    Mesh_Full->SetCollisionProfileName("Magazine");


    CHelpers::GetAsset<UStaticMesh>(&mesh, "StaticMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SM_AR4_Mag_Empty.SM_AR4_Mag_Empty'");
    Mesh_Empty->SetStaticMesh(mesh);
    Mesh_Empty->SetCollisionProfileName("Magazine");

   // CLog::Print(bEject);

}

// BeginPlay()전에 호출 할수 있는 방법은 무엇이 있는지
// SetEject()를 호출 
void ACMagaZine::BeginPlay()
{
	Super::BeginPlay();

    if (bEject == false)
        Mesh_Full->SetVisibility(true);
}
// SpawnActorDiffer()로 Call한후  이함수를 호출  FinishSpawnActor()->Constructor(배치전)
// Constructor에 패트롤Path에 넣은것
void ACMagaZine::SetEject()
{
    bEject = true;
    Mesh_Full->SetVisibility(false);
    Mesh_Empty->SetSimulatePhysics(true);
}