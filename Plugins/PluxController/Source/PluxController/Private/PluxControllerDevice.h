#include "CoreMinimal.h"
#include "Runnable.h"
#include "PluxAPI/mainPlux.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogPluxControllerEvent, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogPluxThread, Log, All);

// Definition of a MemoryDev-derived class to implement the device callbacks
class PluxControllerDevice : public Plux::MemoryDev
{
public:
   // Constructors
	PluxControllerDevice(const Plux::String &path) : MemoryDev(path) {
		running = MakeShareable(new bool(false));

		//// Algorithm inits
		l = FVector(0.f, l_value, 0.f);
		r0 = FVector(0.f, height, 0.f);

		r1 = r0 + l / 2;
		r2 = r0 - l / 2;

		UE_LOG(LogPluxThread, Log, TEXT("r1: %s, r2:%s"),*r1.ToString(), *r2.ToString())
   }
	PluxControllerDevice(Plux::BaseDev &baseDev) : MemoryDev(baseDev) {}

   ~PluxControllerDevice() {
   }
   
   // Callbacks override
   bool onRawFrame(int nSeq, const int data[])
   {
	   PluxControllerStates->GonAxes[0].Scale = (float)data[0];
	   PluxControllerStates->GonAxes[1].Scale = (float)data[1];
	   PluxControllerStates->GonAxes[2].Scale = (float)data[2];
	   PluxControllerStates->GonAxes[3].Scale = (float)data[3];

	   PluxControllerStates->TimeStamp.Scale = FPlatformTime::ToSeconds(FPlatformTime::Cycles());
	   return !*running.Get();
   }
   
   bool onEvent(const Plux::Event &evt)
   {
	   UE_LOG(LogPluxControllerEvent, Log, TEXT("Event: %d"), (int32)evt.type);
      switch (evt.type)
      {
         case Plux::Event::TypeDigInUpdate:
         {
            const Plux::EvtDigInUpdate &evtDig = static_cast<const Plux::EvtDigInUpdate&>(evt);
			UE_LOG(LogPluxControllerEvent, Log, TEXT("Digital input event - Clock source: %d  Clock value: %d  New input state: %d"), (int32)evtDig.timestamp.source, (int32)evtDig.timestamp.value, (int32)evtDig.state);
            break;
         }
            
         case Plux::Event::TypeSchedChange:
         {
            const Plux::EvtSchedChange &evtSchChg = static_cast<const Plux::EvtSchedChange&>(evt);
			UE_LOG(LogPluxControllerEvent, Log, TEXT("Schedule change event - Action: %d  Schedule start time: %lu"), (int32)evtSchChg.action, evtSchChg.schedStartTime);

            break;
         }
            
         case Plux::Event::TypeSync:
         {
            const Plux::EvtSync &evtSync = static_cast<const Plux::EvtSync&>(evt);
			UE_LOG(LogPluxControllerEvent, Log, TEXT("Sync event:"));
			for (std::vector<Plux::Clock>::const_iterator it = evtSync.timestamps.cbegin();
				it != evtSync.timestamps.cend(); it++)
				UE_LOG(LogPluxControllerEvent, Log, TEXT("Clock source: %d  Clock value: %d"), (int32)it->source, (int32)it->value);
            break;
         }
            
         case Plux::Event::TypeDisconnect:
         {
            const Plux::EvtDisconnect &evtDisc = static_cast<const Plux::EvtDisconnect&>(evt);
			UE_LOG(LogPluxControllerEvent, Warning, TEXT("Disconnect event - Reason: %d"),(int32)evtDisc.reason);
            return true;   // Exit loop() after receiving a disconnect event
         }
      }
      
      return false;
   }
   
   bool onInterrupt(void *param)
   {
	   UE_LOG(LogPluxControllerEvent, Log, TEXT("Interrupt - Parameter: %p"), param);
      return false;
   }
   
   bool onTimeout(void)
   {
      return false;
   }
   

   
   bool onSessionEvent(const Plux::Event &evt)
   {
      switch (evt.type)
      {
         case Plux::Event::TypeDigInUpdate:
         {
            const Plux::EvtDigInUpdate &evtDig = static_cast<const Plux::EvtDigInUpdate&>(evt);
			UE_LOG(LogPluxControllerEvent, Log, TEXT("Session digital input event - Clock source: %d  Clock value: %d  New input state: %d"),
				(int32)evtDig.timestamp.source, (int32)evtDig.timestamp.value, (int32)evtDig.state);
			lastDigState = evtDig.state ? 1 : 0;
            break;
         }
            
         case Plux::Event::TypeSync:
         {
            const Plux::EvtSync &evtSync = static_cast<const Plux::EvtSync&>(evt);
			UE_LOG(LogPluxControllerEvent, Log, TEXT("Session sync event:"));
			for (std::vector<Plux::Clock>::const_iterator it = evtSync.timestamps.cbegin();
				it != evtSync.timestamps.cend(); it++)
				UE_LOG(LogPluxControllerEvent, Log, TEXT("Clock source: %d  Clock value: %d"), (int32)it->source, (int32)it->value);
         }
      }
      
      return false;
   }
   
   void SetPluxControllerState(TSharedPtr<FPluxControllerState, ESPMode::ThreadSafe> _PluxControllerStates) {
	   PluxControllerStates = _PluxControllerStates;
   }


   // variables used by onSession...() callbacks
   TSharedPtr<bool, ESPMode::ThreadSafe> running;
   TSharedPtr<FPluxControllerState, ESPMode::ThreadSafe> PluxControllerStates;
   int lastDigState;

   // Algorithm variables
   float l_value = 0.2f;
   float height = 0.25f;
   FVector l;
   FVector r0, r1, r2;
   FVector a1, a2;
};

class FPluxThread : public FRunnable
{
public:
	FPluxThread(const char *DeviceMAC, int Freq) :Freq(Freq)
	{
		try {
			Dev = new PluxControllerDevice(DeviceMAC);
		}
		catch (const Plux::Error::InvalidOperation &e)
		{
			// catch Invalid Operation exception with error code
			UE_LOG(LogPluxThread, Warning, TEXT("Biosignal Plux Library Exception: %s\tcode: %d"), UTF8_TO_TCHAR(e.getDescription()), e.code);
			return;
		}
		catch (const Plux::Exception &e)
		{
			// catch all other PLUX exceptions
			UE_LOG(LogPluxThread, Warning, TEXT("Biosignal Plux Library Exception: %s"), UTF8_TO_TCHAR(e.getDescription()));
			return;
		}
	}

	void SetControllerState(TSharedPtr<FPluxControllerState, ESPMode::ThreadSafe> PluxControllerStates) 
	{
		if (!Dev)return;
		Dev->SetPluxControllerState(PluxControllerStates);
	}

	virtual bool Init() override 
	{
		if (!Dev)return false;
		UE_LOG(LogPluxThread, Log, TEXT("Starting Biosignal Plux Session"));
		try {
			Dev->start(Freq, { 1,2,3,4,5,6,7,8 },16);
		}
		catch (const Plux::Error::InvalidOperation &e)
		{
			// catch Invalid Operation exception with error code
			UE_LOG(LogPluxThread, Warning, TEXT("Biosignal Plux Library Exception: %s\tcode: %d"), UTF8_TO_TCHAR(e.getDescription()), e.code);
			return false;

		}
		catch (const Plux::Exception &e)
		{
			// catch all other PLUX exceptions
			UE_LOG(LogPluxThread, Warning, TEXT("Biosignal Plux Library Exception: %s"), UTF8_TO_TCHAR(e.getDescription()));
			return false;
		}
		return true;
	}

	virtual uint32 Run() override 
	{
		if (!Dev)return -1;
		UE_LOG(LogPluxThread, Log, TEXT("Thread Running"));
		try {
			*Dev->running = true;
			Dev->loop();
		}
		catch (const Plux::Error::InvalidOperation &e)
		{
			// catch Invalid Operation exception with error code
			UE_LOG(LogPluxThread, Warning, TEXT("Biosignal Plux Library Exception: %s\tcode: %d"), UTF8_TO_TCHAR(e.getDescription()), e.code);
			*Dev->running = false;
			return -1;

		}
		catch (const Plux::Exception &e)
		{
			// catch all other PLUX exceptions
			UE_LOG(LogPluxThread, Warning, TEXT("Biosignal Plux Library Exception: %s"), UTF8_TO_TCHAR(e.getDescription()));
			*Dev->running = false;
			return -1;
		}
		return 0;
	}

	virtual void Stop() override 
	{
		if (Dev)
			*Dev->running = false;
	}

	virtual void Exit() override 
	{
		if (!Dev)return;
		UE_LOG(LogPluxThread, Log, TEXT("Ending Biosignal Plux Session"));
		try {
			Dev->stop();
		}
		catch (const Plux::Error::InvalidOperation &e)
		{
			// catch Invalid Operation exception with error code
			UE_LOG(LogPluxThread, Warning, TEXT("Biosignal Plux Library Exception: %s\tcode: %d"), UTF8_TO_TCHAR(e.getDescription()), e.code);

		}
		catch (const Plux::Exception &e)
		{
			// catch all other PLUX exceptions
			UE_LOG(LogPluxThread, Warning, TEXT("Biosignal Plux Library Exception: %s"), UTF8_TO_TCHAR(e.getDescription()));
		}
		delete Dev;
		Dev = NULL;
	}

	PluxControllerDevice* Dev = NULL;
	int Freq;
};
