#pragma once

#include "AllowWindowsPlatformTypes.h"
#include "bluetoothapis.h"
#include <WinSock2.h>
#include <ws2bth.h>
#include "HideWindowsPlatformTypes.h"
#include "SerialPort.h"
#include "Queue.h"
/**
* A thread which get data from bluetooth dongle
*/
class BluetoothConnector : public FRunnable
{

	//Contstructor    
public:
	BluetoothConnector();
	virtual ~BluetoothConnector();

	//Private attributes
private:
	SOCKET bluetoothSocket;                        //The socket which listen to the bluetooth device
	SOCKET connected;                            //The socket which is connected to the smartphone via bluetooth
	FThreadSafeCounter stopTaskCounter;            //stop the thread with this counter
	static BluetoothConnector* connector;        //A static singleton to access the thread
	FRunnableThread* thread;                    //The running thread
	
												//Public methods from FRunnable
public:
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	static SerialPort * arduino;
	//Public methods
public:
	void EnsureCompletion();                                        //Proper shutdown
	static void joyInit();                                         //A static method to initialize the thread
	static void shutdownThread();                                    //A static method to kill the thread
	static TQueue<TArray<int>, EQueueMode::Spsc> * message_queue;
};
