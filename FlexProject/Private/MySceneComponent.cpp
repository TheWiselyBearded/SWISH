// Fill out your copyright notice in the Description page of Project Settings.

#include "MySceneComponent.h"
#include "BluetoothConnector.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

// Sets default values for this component's properties
UMySceneComponent::UMySceneComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMySceneComponent::BeginPlay()
{
	Super::BeginPlay();
	BluetoothConnector::joyInit();
	// ...
	
}
void UMySceneComponent::BeginDestroy() {
	Super::BeginDestroy();

	//UE_LOG(LogTemp, Warning, TEXT("Begin Destroy called."));
	BluetoothConnector::shutdownThread();
}
int countingticks = 0;
int framecounter = 0;
float timetime = 0;

float EMITTER_DURATION = 3.0f;
float lasttime = 0;
float sign = 1;

int req_height_steps = 0;
int req_theta_steps = 0;
int req_radius_steps = 0;

#define HISTORY 1
int runningHistoryTheta[HISTORY];
int runningHistoryRadius[HISTORY];
int runningHistoryHeight[HISTORY];
int prevTheta;

int full_rotation = 200;
float UNREAL_BUCKET_RADIUS = 10.0f;////20.0f;	// in real life, diameter of bucket is 254mm /SES: this value was set to 100. /50cm = 500mm (phsyical bucket and steppers work w/ mm
float PHYS_BUCKET_RADIUS_STEPS = 214.0f;// 214.0f;		// bidrectional, full step mode of motor.
float UNREAL_BUCKET_HEIGHT = 38.1f;//in real life, height of bucket is 356mm  /SES: total is a 100 why is this 70? because of motion controller offset.
float PHYS_BUCKET_HEIGHT_STEPS = 6250.0f;
float scalefactor_h = PHYS_BUCKET_HEIGHT_STEPS / UNREAL_BUCKET_HEIGHT;	// 6250 total steps, 1000mm virtual bucket.
float scalefactor_theta = full_rotation /(2.0f*3.14159f);	// 2 * pi because radians.
float scalefactor_radius = PHYS_BUCKET_RADIUS_STEPS / UNREAL_BUCKET_RADIUS; // bucket is 50cm, so 500mm, 428 physical steps total.

float mass_x, mass_y, mass_z, mass_h, mass_theta, mass_radius;

int max_theta_steps = 200;
int max_radius_steps = 428;
int max_height_steps = 6000;

int prev_req_height_steps;

int b4_theta_steps;
int b4_height_steps;
int b4_radius_steps;

bool isResting = false;



float fps = 60.0f;
int firstOmissions = fps*5;//omission time = 5 seconds
// Called every frame
void UMySceneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	timetime += DeltaTime;
	// Ignore emitter duration for storing data.
	if (timetime < EMITTER_DURATION) {
		return;
	}
	// Ref prior frame height steps for comparison before enqueue.
	if (!isResting) {
		prev_req_height_steps = req_height_steps;
	}
	TArray<int> tdata;
	// If req height steps is closer to lid, moving upward, increase FPS.
	/*if (fps < 30.0f) {
		fps += 5.0f;
	} else if (fps > 0.0f) {
		fps -= 5.0f;
	}*/
	//(req_height_steps < 2500) ? 10.0f : 20.0f;

	//if (timetime - lasttime < 1 / fps)return;
	//lasttime = timetime;
	int delta_theta_thresh =(int)(200 / fps);//(200 steps per second)
	int delta_radius_thresh = (int)(200 / fps);//(200 steps per second) 
	int delta_height_thresh = (int)(500 / fps);//(500 steps per second)

	/*if (timetime > 30) {
		timetime = 0;
		lasttime = 0;
	}*/
	/*req_theta_steps += delta_theta_thresh*sign;
	if (req_theta_steps > 400) {
		sign = -1;
		req_theta_steps += delta_theta_thresh * sign * 2;
	}
	else if (req_theta_steps < 0) {
		sign = 1;
		req_theta_steps += delta_theta_thresh * sign * 2;
	}*/
	/*if (timetime < 10) {
		req_theta_steps += delta_theta_thresh;
		req_radius_steps += delta_radius_thresh;
		req_height_steps += delta_height_thresh;
	}
	else if (timetime > 20) {
		req_theta_steps -= delta_theta_thresh;
		req_radius_steps -= delta_radius_thresh;
		req_height_steps -= delta_height_thresh;
	}*/
	/*req_radius_steps = req_radius_steps < max_radius_steps / 2 ? req_radius_steps : max_radius_steps / 2;
	req_height_steps = req_height_steps < max_height_steps ? req_height_steps : max_height_steps;

	//prev_theta_steps = prev_theta_steps > -max_theta_steps ? prev_theta_steps : -max_theta_steps;
	req_radius_steps = req_radius_steps > -max_radius_steps / 2 ? req_radius_steps : -max_radius_steps / 2;
	req_height_steps = req_height_steps > 0 ? req_height_steps : 0;

		//move to a certain position
	tdata.Add(req_theta_steps);
	tdata.Add(req_radius_steps + max_radius_steps / 2);
	tdata.Add(req_radius_steps + max_radius_steps / 2);
	tdata.Add(req_height_steps);
	BluetoothConnector::message_queue->Enqueue(tdata);
	

	return;
	*/
	/*if (timetime - lasttime < 1 / fps)return;
	lasttime = timetime;*/
	float timeToDisplay = 1 / fps;// 0.1f;
	FVector pos = GetComponentLocation();
	FRotator rot = GetComponentRotation();
	//position of center of mass in bucket coordinates
	mass_x = meteor_posX - pos.X;// -10;
	mass_y = meteor_posY - pos.Y;// +30;//// ACCOUNT FOR OFFSET OF ASSET.
	mass_z = meteor_posZ - pos.Z; 
	FVector rotatedvector = rot.UnrotateVector(FVector(mass_x, mass_y, mass_z));
	// Assign changed values of rotation from world space.
	mass_x = rotatedvector.X;
	mass_y = rotatedvector.Y;
	mass_z = rotatedvector.Z;
	UE_LOG(LogTemp, Warning, TEXT("METEOR_POS X,Y,Z:, %f, %f, %f"), meteor_posX, meteor_posY, meteor_posZ);
	UE_LOG(LogTemp, Warning, TEXT("POS X,Y,Z:, %f, %f, %f"), pos.X, pos.Y, pos.Z);
	UE_LOG(LogTemp, Warning, TEXT("MASS X,Y,Z:, %f, %f, %f"), mass_x, mass_y, mass_z);

	//bucket cylindrical coordinates 
	mass_h = -(mass_z);
	mass_theta = atan2(mass_x, mass_y);
	mass_radius = sqrt(mass_y*mass_y + mass_x * mass_x);

	UE_LOG(LogTemp, Warning, TEXT("MASSH,THETA,RADIUS:, %f, %f, %f"), mass_h, mass_theta, mass_radius);
	/*GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("CYL_HTR: %f, %f, %f"),
		mass_h,
		mass_theta,
		mass_radius), true, FVector2D(2, 2));*/
	/*GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("b4T,b4R,b4H,T,R,H: %d, %d, %d, %d, %d, %d"),
		b4_theta_steps,
		b4_radius_steps,
		b4_height_steps,
		req_theta_steps,
		req_radius_steps,
		req_height_steps), true, FVector2D(2, 2));*/
	/*GEngine->AddOnScreenDebugMessage(-1, timeToDisplay, FColor::Blue, FString::Printf(TEXT("pos XYZ: %f, %f, %f"),
		pos.X,
		pos.Y,
		pos.Z), true, FVector2D(2, 2));
	GEngine->AddOnScreenDebugMessage(-1, timeToDisplay, FColor::Blue, FString::Printf(TEXT("meteor XYZ: %f, %f, %f"),
		meteor_posX,
		meteor_posY,
		meteor_posZ), true, FVector2D(2, 2));*/
	//bucket cylindrical coordinates in steps
	int desired_height_steps = (int)(mass_h * scalefactor_h);
	//int desired_height_steps = 0;
	int desired_theta_steps = (int)(mass_theta * scalefactor_theta);
	int desired_radius_steps = (int)(mass_radius * scalefactor_radius);

	UE_LOG(LogTemp, Warning, TEXT("DESIREDSTEPSH,THETA,RADIUS:, %d, %d, %d"), desired_height_steps, desired_theta_steps, desired_radius_steps);

	//Skip frames 
	//timetime += DeltaTime;
	framecounter++;
	int frameskip = (int)(60 / fps);
	if (framecounter == frameskip) {
		framecounter = 0;
	}
	else { return; }

	//omit first few seconds while connecting
	if (firstOmissions > 0) {
		firstOmissions--;
		return;
	}

	float SCALINGFACTOR = 5.0f; //for radius prioritization

	// Calc diff between current steps and previous.
	int delta_theta_steps = desired_theta_steps - req_theta_steps;
	int delta_radius_steps = desired_radius_steps - req_radius_steps;
	int delta_height_steps = desired_height_steps - req_height_steps;
	//1: Constrain deltatheta so that its absolute value is no more than a half rotation. 
	//		Adding or subtracting a full rotation gives equivalent resulting theta angles.
	//2: Choose either A:(+radius,theta) or B:(-radius,theta+halfrotation), depending on which one uses fewer delta steps.
	//3: Delta steps are computed
	int deltaradA = desired_radius_steps - req_radius_steps;
	int deltathetaA = (desired_theta_steps - req_theta_steps) % full_rotation;
	if (deltathetaA > full_rotation / 2) { deltathetaA -= full_rotation; }	// + change, C.clockwise move
	else if (deltathetaA < -full_rotation / 2) { deltathetaA += full_rotation; }	// - change, clockwise move .

	int deltaradB = -desired_radius_steps - req_radius_steps;
	int deltathetaB = ((desired_theta_steps + (full_rotation / 2) - req_theta_steps)) % full_rotation;
	if (deltathetaB > full_rotation / 2) { deltathetaB -= full_rotation; }
	else if (deltathetaB < -full_rotation / 2) { deltathetaB += full_rotation; }
	
	// max r, theta for potential outcomes considered, choose min of that for shortest path.
	int m0 = FMath::Max(abs(deltaradA), (int)abs(deltathetaA*SCALINGFACTOR));
	int m1 = FMath::Max(abs(deltaradB), (int)abs(deltathetaB*SCALINGFACTOR));
	if (m0<m1) {
		delta_theta_steps = deltathetaA;
		delta_radius_steps = deltaradA;
	} else {
		delta_theta_steps = deltathetaB;
		delta_radius_steps = deltaradB;
	}


	UE_LOG(LogTemp, Warning, TEXT("ACHOICE:DRA, DTA, DRB, DTB:, %d, %d, %d, %d"),deltaradA, deltathetaA, deltaradB, deltathetaB);

	//float delta_theta_thresh = 200 / fps;//(200 steps per second)
	//float delta_radius_thresh = 200/fps;//(200 steps per second) 
	//float delta_height_thresh = 500/fps;//(500 steps per second)
	b4_theta_steps = req_theta_steps + delta_theta_steps;
	b4_height_steps = req_height_steps + delta_height_steps;
	b4_radius_steps = req_radius_steps + delta_radius_steps;
	
	//Range checking for deltas
	//delta_theta_steps constrained within [-delta_theta_thresh, delta_theta_thresh]
	delta_theta_steps = delta_theta_steps < delta_theta_thresh ? delta_theta_steps : delta_theta_thresh;
	delta_radius_steps = delta_radius_steps < delta_radius_thresh ? delta_radius_steps : delta_radius_thresh;
	//delta_height_steps = delta_height_steps < delta_height_thresh ? delta_height_steps : delta_height_thresh;
	delta_theta_steps = delta_theta_steps > -delta_theta_thresh ? delta_theta_steps : -delta_theta_thresh;
	delta_radius_steps = delta_radius_steps > -delta_radius_thresh ? delta_radius_steps : -delta_radius_thresh;
	delta_height_steps = delta_height_steps > -delta_height_thresh ? delta_height_steps : -delta_height_thresh;

	//Apply deltas to motor positions
	req_theta_steps += delta_theta_steps;
	req_radius_steps += delta_radius_steps;
	req_height_steps += delta_height_steps;
	//Range checking for motor positions - leave theta unconstrained as it can go in circles
	//prev_theta_steps = prev_theta_steps < max_theta_steps ? prev_theta_steps : max_theta_steps;
	req_radius_steps = req_radius_steps < max_radius_steps/2 ? req_radius_steps : max_radius_steps/2;
	req_height_steps = req_height_steps < max_height_steps ? req_height_steps : max_height_steps;

	//prev_theta_steps = prev_theta_steps > -max_theta_steps ? prev_theta_steps : -max_theta_steps;
	req_radius_steps = req_radius_steps > -max_radius_steps / 2 ? req_radius_steps : -max_radius_steps / 2;
	req_height_steps = req_height_steps > 0 ? req_height_steps : 0;

	//UE_LOG(LogTemp, Warning, TEXT("DESIREDSTEPS AFTER RANGECHECKING H,THETA,RADIUS:, %d, %d, %d"), req_height_steps, req_theta_steps, req_radius_steps);
	//UE_LOG(LogTemp, Warning, TEXT("REQUESTED MOTOR POS: TIME|THETA|RAD|H:, %d, %d, %d, %d"), (int)(timetime*1000), (req_theta_steps%200+200)%200, req_radius_steps+214, req_height_steps);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::Printf(TEXT("T, R, H: %d, %d, %d"), (req_theta_steps % 200 + 200) % 200, req_radius_steps + 214, req_height_steps));
	UE_LOG(LogTemp, Warning, TEXT("b4after H,THETA,RADIUS:, %d, %d, %d, %d, %d, %d"), b4_height_steps, b4_theta_steps, b4_radius_steps, req_height_steps, req_theta_steps, req_radius_steps);
	
	/*GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, 
		FString::Printf(TEXT("B4A HTR: %d, %d, %d, %d, %d, %d"),
		b4_height_steps, b4_theta_steps, b4_radius_steps, 
		req_height_steps, req_theta_steps, req_radius_steps), 
		true, FVector2D(2, 2));*/

	int meanTheta = 0;
	int meanRadius = 0;
	int meanHeight = 0;
	for (int i = 0; i < HISTORY-1; i++) {
		runningHistoryTheta[i] = runningHistoryTheta[i + 1];
		runningHistoryRadius[i] = runningHistoryRadius[i + 1];
		runningHistoryHeight[i] = runningHistoryHeight[i + 1];
		meanTheta += runningHistoryTheta[i];
		meanRadius += runningHistoryRadius[i];
		meanHeight += runningHistoryHeight[i];
	}
	runningHistoryTheta[HISTORY - 1] = req_theta_steps;
	runningHistoryRadius[HISTORY - 1] = req_radius_steps;
	runningHistoryHeight[HISTORY - 1] = req_height_steps;

	meanTheta += req_theta_steps;
	meanRadius += req_radius_steps;
	meanHeight += req_height_steps;

	meanTheta /= HISTORY;
	meanRadius /= HISTORY;
	meanHeight /= HISTORY;
	
	
	if (meanRadius  < 20 && meanRadius > -20) {
		meanRadius = 0;
		meanTheta = prevTheta;
	}
	prevTheta = meanTheta;
	/*GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,
		FString::Printf(TEXT("B4A HTR: %d, %d, %d, %d, %d, %d"),
			b4_height_steps, b4_theta_steps, b4_radius_steps,
			meanHeight, meanTheta, meanRadius),
		true, FVector2D(2, 2));*/



	if (abs(req_height_steps - prev_req_height_steps) < 10) {
			isResting = true;
			//GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Blue,FString::Printf(TEXT("req height steps rest")), true, FVector2D(2, 2));
			tdata.Add(meanTheta);
			tdata.Add(meanRadius + max_radius_steps / 2);
			tdata.Add(meanRadius + max_radius_steps / 2);
			// Transmit same data for height until: height change > thresh
			tdata.Add(meanHeight);
		} else {
			// Safe check delta steps doesn't exceed a full radius change in instance of frame.
			tdata.Add(meanTheta);
			tdata.Add(meanRadius + max_radius_steps / 2);
			tdata.Add(meanRadius + max_radius_steps / 2);
			tdata.Add(meanHeight);
			isResting = false;
		}
	// If the steps are at rest position.
	//if (abs(req_height_steps - prev_req_height_steps) < 10) {
	//	isResting = true;
	//	//GEngine->AddOnScreenDebugMessage(-1, 1.1f, FColor::Blue,FString::Printf(TEXT("req height steps rest")), true, FVector2D(2, 2));
	//	tdata.Add(req_theta_steps);
	//	tdata.Add(req_radius_steps + max_radius_steps / 2);
	//	tdata.Add(req_radius_steps + max_radius_steps / 2);
	//	// Transmit same data for height until: height change > thresh
	//	tdata.Add(prev_req_height_steps);
	//} else {
	//	// Safe check delta steps doesn't exceed a full radius change in instance of frame.
	//	tdata.Add(req_theta_steps);
	//	tdata.Add(req_radius_steps + max_radius_steps / 2);
	//	tdata.Add(req_radius_steps + max_radius_steps / 2);
	//	tdata.Add(req_height_steps);
	//	isResting = false;
	//}
	/*tdata.Add(req_theta_steps);
	tdata.Add(req_radius_steps + max_radius_steps / 2);
	tdata.Add(req_radius_steps + max_radius_steps / 2);
	tdata.Add(req_height_steps);*/
	
		BluetoothConnector::message_queue->Enqueue(tdata);

}

