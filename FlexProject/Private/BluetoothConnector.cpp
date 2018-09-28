#include "BluetoothConnector.h"
#include "SerialPort.h"
#include <string>

BluetoothConnector* BluetoothConnector::connector = NULL;
TQueue<TArray<int>, EQueueMode::Spsc> * BluetoothConnector::message_queue = NULL;
SerialPort * BluetoothConnector::arduino = NULL;
//String for getting the output from arduino
char output[MAX_DATA_LENGTH];
char *port_name = "\\\\.\\COM5";

BluetoothConnector::BluetoothConnector() {

	thread = FRunnableThread::Create(this, TEXT("BluetoothConnector"), 0, TPri_BelowNormal);

}

BluetoothConnector::~BluetoothConnector() {

	delete thread;
	thread = NULL;
	
}

//Initialize the thread
bool BluetoothConnector::Init() {

	return true;

}

//Message Queue

//Run the thread
uint32 BluetoothConnector::Run() {
	TArray<int> message;

	if (!arduino) {
		arduino = new SerialPort("\\\\.\\COM5");
	}
	if (arduino->isConnected()) {
		UE_LOG(LogTemp, Warning, TEXT("Connection Established"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ERROR, check port name"));
		return 0;
	}
	
	int16_t * int_in = new int16_t[4];
	int16_t * int_out = new int16_t[4];

	while (stopTaskCounter.GetValue() == 0) {
		if (!message_queue->IsEmpty()){
			message_queue->Dequeue(message);
			if (arduino->isConnected()){
				int a = message[0];
				int b = message[1];
				int c = message[2];
				int d = message[3];
				
				int_in[0] = a;
				int_in[1] = b;
				int_in[2] = c;
				int_in[3] = d;
				UE_LOG(LogTemp, Warning, TEXT("Bluetooth going to send %i, %i, %i, %i"),a,b,c,d);

				arduino->writeSerialPort((char *)int_in, 8);
				
				UE_LOG(LogTemp, Warning, TEXT("Sent: %i,%i,%i,%i"), int_in[0], int_in[1], int_in[2], int_in[3]);

				arduino->readSerialPort((char *)int_out, 8);
				
				UE_LOG(LogTemp, Warning, TEXT("Received: %i,%i,%i,%i"), int_out[0], int_out[1], int_out[2], int_out[3]);
				
			}
		}
	}
	delete arduino;
	//}
	return 0;

}


//Do a joy initialize of the thread
void BluetoothConnector::joyInit() {

	if (!connector && FPlatformProcess::SupportsMultithreading()) {

		connector = new BluetoothConnector();
		message_queue = new TQueue<TArray<int>, EQueueMode::Spsc>();

	}
}

//Stop the thread
void BluetoothConnector::Stop() {

	stopTaskCounter.Increment();
	
}

//Ensure that the thread stop
void BluetoothConnector::EnsureCompletion() {
	Stop();
	thread->WaitForCompletion();
}

//Shut the thread down
void BluetoothConnector::shutdownThread() {
	if (connector) {
		connector->EnsureCompletion();
		
		if (arduino) {
			delete arduino;
			arduino = NULL;
		}
		delete connector;

		connector = NULL;
	}
	
}
