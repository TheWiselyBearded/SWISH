// Fill out your copyright notice in the Description page of Project Settings.
#include "MyActorComponent.h"
#include "GameFramework/Actor.h"
#include "BluetoothConnector.h"

// Sets default values
UMyActorComponent::UMyActorComponent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsBeginPlay = true;
	bWantsInitializeComponent = true;
}

// Called when the game starts or when spawned
void UMyActorComponent::BeginPlay()
{
	Super::BeginPlay();

	BluetoothConnector::shutdownThread();
	BluetoothConnector::joyInit();

	//BluetoothConnector::message_queue->Enqueue("asdf");
}

/*void UMyActorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	UE_LOG(LogTemp, Warning, TEXT("ActorComponent method call"));
}*/



void UMyActorComponent::BeginDestroy() {
	Super::BeginDestroy();

	//UE_LOG(LogTemp, Warning, TEXT("Begin Destroy called."));
	BluetoothConnector::shutdownThread();
}
