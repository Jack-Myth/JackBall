// Fill out your copyright notice in the Description page of Project Settings.

#include "JackBall.h"
#include "JackBall_Character.h"
#include "BallTouchableMesh.h"
#include "DestructibleActor.h"
#include "DestructibleMesh.h"
#include "DestructibleComponent.h"
#include <GameFramework/Actor.h>


// Sets default values
AJackBall_Character::AJackBall_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->SetCapsuleSize(50, 50, false);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(50,false);
	JackBallSelf = CreateDefaultSubobject<UStaticMeshComponent>("JackBall");
	JackBallSelf->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	JackBallSelf->SetRelativeLocation(FVector(0, 0, 0));
	JackBallSelf->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")).Object);
	JackBallSelf->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/JackBall/Materials/BallSurface_Android.BallSurface_Android'")).Object);
	/*if (UGameplayStatics::GetPlatformName()=="Windows")
	{
		JackBallSelf->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/JackBall/Materials/BallSurface.BallSurface'")).Object);
	}
	else {
		JackBallSelf->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("MaterialInstanceConstant'/Game/TestForMaterials/MaterialField/TestMaterial_Glass_Inst.TestMaterial_Glass_Inst'")).Object);
	}*/
	JackBallSelf->OnComponentHit.AddDynamic(this, &AJackBall_Character::HitCheck);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AJackBall_Character::HitCheck);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	GetCharacterMovement()->MaxWalkSpeed = 1200;
	GetCharacterMovement()->JumpZVelocity = 800;
	JumpMaxHoldTime = 0.5;
	GetCharacterMovement()->GravityScale = 2;
}

// Called when the game starts or when spawned
void AJackBall_Character::BeginPlay()
{
	Super::BeginPlay();
	//UGameplayStatics::PlaySound2D(GetWorld(), LoadObject<USoundBase>(nullptr, TEXT("SoundWave'/Game/JackBall/Sounds/unitybgm.unitybgm'")));
	//GetWorldTimerManager().SetTimer(TimeHandleForMovement, this, &AJackBall_Character::MoveForward, 0.02, true, 0.02);
}

// Called every frame
void AJackBall_Character::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	CameraComponent->SetWorldLocation(FVector(GetActorLocation().X - 200, 0, 500));
	CameraComponent->SetWorldRotation(FRotator(-30, 0, 0));
	//GetCharacterMovement()->Velocity = FVector(1000, GetCharacterMovement()->Velocity.Y, GetCharacterMovement()->Velocity.Z);
	AddMovementInput(GetActorForwardVector(), 10,true);
	if (GetActorLocation().Z < -300)
		HitFunction(NULL, NULL, NULL, FVector(0, 0, 0), FHitResult(NULL, NULL, GetActorLocation(), FVector(0, 0, 0)));
}

// Called to bind functionality to input
void AJackBall_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Touch", IE_Pressed, this, &AJackBall_Character::ActionForTouchBegin);
	PlayerInputComponent->BindAction("Touch", IE_Released, this, &AJackBall_Character::ActionForTouchEnd);
	PlayerInputComponent->BindAction("Back", IE_Pressed, this, &AJackBall_Character::ActionForExit);
}

void AJackBall_Character::HitCheck(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComponent,FVector Impluse,const FHitResult &Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Hitted"));
	if (!dynamic_cast<ABallTouchableMesh*>(OtherActor))
	{
		HitFunction(HitComponent, OtherActor, OtherComponent, Impluse, Hit);
		//TmpForTest ? GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TmpForTest->GetPathName()) : GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "NULL");
		return;
	}
}

void AJackBall_Character::HitFunction(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector Impluse, const FHitResult &Hit)
{
	if (IsGameOver)
		return;
	IsGameOver = true;
	PrimaryActorTick.SetTickFunctionEnable(false);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	if (UGameplayStatics::GetPlatformName() == "Windows")
	{
		ADestructibleActor* TmpDesActor = GetWorld()->SpawnActor<ADestructibleActor>(GetActorLocation(), FRotator(0, 0, 0));
		TmpDesActor->GetDestructibleComponent()->SetDestructibleMesh(LoadObject<UDestructibleMesh>(NULL, TEXT("DestructibleMesh'/Game/JackBall/Mesh/Sphere_DM.Sphere_DM'"), NULL, LOAD_None, NULL));
		TmpDesActor->GetDestructibleComponent()->ApplyDamage(1000, Hit.Location, Impluse, 1000);
	}
	UGameplayStatics::PlaySound2D(GetWorld(), LoadObject<USoundBase>(nullptr, TEXT("SoundWave'/Game/JackBall/Sounds/Dead.Dead'")));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LoadObject<UParticleSystem>(NULL, TEXT("ParticleSystem'/Game/HDParticlePack/Particles/P_BurstStop.P_BurstStop'")), Hit.Location);
	TArray<AActor*> TmpForWinZone;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("WinZone"), TmpForWinZone);
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), LoadClass<UBlueprint>(NULL, TEXT("Blueprint'/Game/JackBall/Blueprints/WinZone.WinZone'")), TmpForWinZone);
	if (TmpForWinZone.Num() != 0)
	{
		FOutputDeviceNull ar;
		TmpForWinZone[0]->CallFunctionByNameWithArguments(TEXT("DeathEvent"), ar, NULL,true);
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("BadWinZone"));
}

void AJackBall_Character::ActionForTouchBegin()
{
	if (!GetCharacterMovement()->IsFalling())
		UGameplayStatics::PlaySound2D(GetWorld(), LoadObject<USoundBase>(nullptr, TEXT("SoundWave'/Game/JackBall/Sounds/Hit.Hit'")));
	Jump();
}

void AJackBall_Character::ActionForTouchEnd()
{
	StopJumping();
}

void AJackBall_Character::AddScore(int ScoreToAdd)
{
	Score += ScoreToAdd;
}

void AJackBall_Character::ActionForExit()
{
	TArray<AActor*> TmpForWinZone;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Pause"), TmpForWinZone);
}