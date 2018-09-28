// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActor.h"
#include "BluetoothConnector.h"

// Sets default values
AMyActor::AMyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();

	BluetoothConnector::shutdownThread();
	BluetoothConnector::joyInit();
	
	//BluetoothConnector::message_queue->Enqueue("asdf");
}



void AMyActor::BeginDestroy() {
	Super::BeginDestroy();

	//UE_LOG(LogTemp, Warning, TEXT("Begin Destroy called."));
	BluetoothConnector::shutdownThread();
}
/*
int countingticks = 0;
TArray<int> tdata;

int prev_h_steps = 0; 
int prev_theta_steps = 0; 
int prev_radius_steps = 0; 
int framecounter = 0;
float MAX_RADIUS = 500.0f;	// 50cm = 500mm (phsyical bucket and steppers work w/ mm
float MAX_RADIUS_STEPS = 214.0f;		// bidrectional, full step mode of motor.
int full_rotation = 200;
//TODO: Update/check the scale values
float scalefactor_h = 6250.0f/1000.0f;	// 6250 total steps, 1000mm virtual bucket.
float scalefactor_theta = 200.0f / (2*3.14159f);
// bucket is 50cm, so 500mm, 428 physical steps total.
float scalefactor_radius = MAX_RADIUS_STEPS / MAX_RADIUS;
*/

// Called every frame
void AMyActor::Tick(float DeltaTime) {
	//framecounter++;
	//framecounter %= 4;
	Super::Tick(DeltaTime);
	/*
	//if (framecounter ==0 && BluetoothConnector::arduino != NULL && BluetoothConnector::arduino->isConnected()) {
	if (BluetoothConnector::arduino != NULL && BluetoothConnector::arduino->isConnected()) {
		//UE_LOG(LogTemp, Warning, TEXT(“flex   pos_x,y,z: %f, %f, %f”), meteor_posX, meteor_posY, meteor_posZ);

		//Assume that the Actor Location is the center of the bucket.
		FVector pos = GetActorLocation();
		FRotator rot = GetActorRotation();

		//TODO: Assign actor script as bucket asset. Right now actor is in blank space
		// and causing location issues for calculation.
		//position of center of mass in bucket coordinates
		float mass_x = meteor_posX - pos.X;
		float mass_y = meteor_posY - pos.Y;
		float mass_z = meteor_posZ - pos.Z;
		UE_LOG(LogTemp, Warning, TEXT("METEOR_POSX: %f, Y: %f, Z: %f"), meteor_posX, meteor_posY, meteor_posZ);
		UE_LOG(LogTemp, Warning, TEXT("POSX: %f, Y: %f, Z: %f"), pos.X, pos.Y, pos.Z);
		UE_LOG(LogTemp, Warning, TEXT("MASSX: %f, Y: %f, Z: %f"), mass_x, mass_y, mass_z);

		//bucket cylindrical coordinates 
		float mass_h = mass_z;
		float mass_theta = atan2(mass_y, mass_x);
		float mass_radius = sqrt(mass_x*mass_x + mass_y * mass_y);
		UE_LOG(LogTemp, Warning, TEXT("MASSH: %f, THETA: %f, RADIUS: %f"), mass_h, mass_theta, mass_radius);

		//bucket cylindrical coordinates in steps
		int mass_h_steps = (int)(mass_h * scalefactor_h);
		int mass_theta_steps = (int)(mass_theta * scalefactor_theta);
		int mass_radius_steps = (int)(mass_radius * scalefactor_radius);
		// Calc diff between current steps and previous.
		int delta_theta_steps = mass_theta_steps - prev_theta_steps;
		int delta_radius_steps = mass_radius_steps - prev_radius_steps;
		int delta_height_steps = mass_h_steps - prev_h_steps;
		//1: Constrain deltatheta so that its absolute value is no more than a half rotation. 
		//		Adding or subtracting a full rotation gives equivalent resulting theta angles.
		//2: Choose either A:(+radius,theta) or B:(-radius,theta+halfrotation), depending on which one uses fewer delta steps.
		//3: Delta steps are computed
		int deltaradA = mass_radius_steps - prev_theta_steps;
		int deltathetaA = (mass_theta_steps - prev_theta_steps) % full_rotation;
		if (deltathetaA > full_rotation / 2) { deltathetaA -= full_rotation; }	// + change, C.clockwise move
		else if (deltathetaA < -full_rotation / 2) { deltathetaA += full_rotation; }	// - change, clockwise move overlap.

		int deltaradB = -mass_radius_steps - prev_theta_steps;
		int deltathetaB = ((mass_theta_steps + (full_rotation / 2) - prev_theta_steps)) % full_rotation;
		if (deltathetaB > full_rotation / 2) { deltathetaB -= full_rotation; }
		else if (deltathetaB < -full_rotation / 2) { deltathetaB += full_rotation; }
		// max r, theta for potential outcomes considered, choose min of that for shortest path.
		int m0 = FMath::Max(abs(deltaradA), abs(deltathetaA));
		int m1 = FMath::Max(abs(deltaradB), abs(deltathetaB));
		if (m0<m1) {
			delta_theta_steps = deltathetaA;
			delta_radius_steps = deltaradA;
		} else {
			delta_theta_steps = deltathetaB;
			delta_radius_steps = deltaradB;
		}
		UE_LOG(LogTemp, Warning, TEXT("DTHETA:\t%d, DR:\t%d, DH:\t%d"), delta_theta_steps, delta_radius_steps, delta_height_steps);
		//TODO: Radius going through middle.
		// Look at range values for bucket, range of bucket numerator, # of steps to get to end physical bucket as denominator
		// range of digital, range of physical.
		tdata.Add(delta_theta_steps);
		tdata.Add(delta_radius_steps);
		tdata.Add(delta_radius_steps);
		tdata.Add(delta_height_steps);

		prev_theta_steps += delta_theta_steps;
		prev_radius_steps += delta_radius_steps;
		prev_h_steps += delta_height_steps;

		BluetoothConnector::message_queue->Enqueue(tdata);
	}
	*/
}
