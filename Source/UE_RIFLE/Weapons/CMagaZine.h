
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CMagaZine.generated.h"

UCLASS()
class UE_RIFLE_API ACMagaZine : public AActor
{
	GENERATED_BODY()
	
public:	
	ACMagaZine();

protected:
	virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
        class USceneComponent* Root;
    UPROPERTY(VisibleAnywhere)
        class UStaticMeshComponent* Mesh_Full;
    UPROPERTY(VisibleAnywhere)
        class UStaticMeshComponent* Mesh_Empty;

public:
    void SetEject();
private:
    bool bEject = false;


};
