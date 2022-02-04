#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

#include "..\User_Settings.h"
#include "ReelTwo.h"
#include "bt/PSController/PSController.h"
#if DOME_DRIVE != DOME_DRIVE_NONE
#include "drive/DomeDrivePWM.h"
#if DOME_DRIVE == DOME_DRIVE_SABER
#include "drive/DomeDriveSabertooth.h"
#endif
#endif
#include "drive/TankDrivePWM.h"
#include "drive/TankDriveRoboteq.h"
#if DRIVE_SYSTEM == DRIVE_SYSTEM_SABER
#include "drive/TankDriveSabertooth.h"
#endif

// ----------------------------------------
// Enumerations

enum Speed_Profile_e
{
  WALK,
  JOG,
  RUN,
  SPRINT
};

// ----------------------------------------
// Class Definitions

#ifdef USE_RELAY
class DriveController : public PSController, Adafruit_Mini_Relay_FeatherWing
#else
class DriveController : public PSController
#endif
{
public:
  DriveController(const char *);
  ~DriveController();

  virtual void notify(void) override;
  virtual void onConnect(void) override;
  virtual void onDisconnect(void) override;

  void updateLED(void);
  uint8_t getSpeed(void);
  String getSpeedName(void);
  void nextSpeed(void);

private:
  uint32_t fLastTime = 0;
  uint8_t fCurrentSpeed = WALK;
};

extern void interpretController(DriveController *);
extern void enableController(void);
extern void disableController(void);
extern void emergencyStop(void);
extern void enableDomeController(void);
extern void disableDomeController(void);
extern void domeEmergencyStop(void);
extern bool getEnabled(void);

// ----------------------------------------

class DomeController : public PSController
{
public:
  DomeController(const char* mac);
  ~DomeController(void);

  virtual void notify(void);
  void process(void);
  virtual void onConnect(void) override;
  virtual void onDisconnect(void) override;

  uint32_t fLastTime = 0;

protected:
  bool fGestureCollect = false;
  char fGestureBuffer[MAX_GESTURE_LENGTH+1];
  char* fGesturePtr = fGestureBuffer;
  char fGestureAxis = 0;
  uint32_t fGestureTimeOut = 0;

  void addGesture(char);
};

#endif
