// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "JackBall_Character.generated.h"

UCLASS()
class JACKBALL_API AJackBall_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AJackBall_Character();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//virtual void EndP();

	void MoveForward();
	UFUNCTION()
		void HitCheck(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector Impluse, const FHitResult &Hit);
	UFUNCTION(BlueprintCallable,Category="JackBall")
		void AddScore(int ScoreToAdd);
	void HitFunction(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector Impluse, const FHitResult &Hit);
	void ActionForTouchBegin();
	void ActionForTouchEnd();
	void ActionForExit();
	UStaticMeshComponent* JackBallSelf;
	FTimerHandle TimeHandleForMovement,TimeHandleForTouchMove;
	UCameraComponent* CameraComponent;
	FVector ActorLocation;
	bool IsGameOver=false;
	UPROPERTY(BlueprintReadOnly,Category="JackBall")
		int Score=0;
};
