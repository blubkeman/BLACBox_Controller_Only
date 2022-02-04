#include "Arduino.h"
#include "Motor_Control.h"

// ========================================
// DriveController Class Functions
// ========================================

// ----------------------------------------
// Constructor

DriveController::DriveController(const char* mac = nullptr) : PSController(mac, DRIVE_CONTROLLER_TYPE) {}

// ----------------------------------------
// Destructor

DriveController::~DriveController() {}

// ----------------------------------------
// Notify()

void DriveController::notify()
{
  uint32_t currentTime = millis();
  uint32_t lagTime = (currentTime > fLastTime) ? currentTime - fLastTime : 0;
  if (lagTime > 5000)
  {
    DEBUG_PRINTLN("More than 5 seconds. Disconnect");
    emergencyStop();
    disconnect();
  }
  else if (lagTime > 500)
  {
    DEBUG_PRINTLN("It has been 500ms. Shutdown motors");
    emergencyStop();
  }
  else
  {
    interpretController(this);
  }
  fLastTime = currentTime;
}

// ----------------------------------------
// onConnect()

void DriveController::onConnect()
{
  DEBUG_PRINTLN("Drive Stick Connected");
  setPlayer(1);
  enableController();
  fLastTime = millis();
}

// ----------------------------------------
// onDisconnect()

void DriveController::onDisconnect()
{
  DEBUG_PRINTLN("Drive Stick Disconnected");
  disableController();
}

// ----------------------------------------
// updateLED()

void DriveController::updateLED()
{
#if DRIVE_CONTROLLER_TYPE == kPS4
  if (!getEnabled())
  {
    setLED(255, 0, 0);      // Red
    return;
  }
  switch (fCurrentSpeed) {
    case Speed_Profile_e::WALK :
    {
      setLED(255, 255, 0);  // Yellow
      break;
    }
    case Speed_Profile_e::JOG :
    {
      setLED(0, 255, 0);    // Green
      break;
    }
    case Speed_Profile_e::RUN :
    {
      setLED(0, 0, 255);    // Blue
      break;
    }
    case Speed_Profile_e::SPRINT :
    {
      setLED(255, 0, 255);  // Purple
      break;
    }
  }
#endif  // kPS4
}

// ----------------------------------------
// getSpeed()

uint8_t DriveController::getSpeed(void)
{
  return fCurrentSpeed;
}

// ----------------------------------------
// getSpeedName()

String DriveController::getSpeedName(void)
{
  String out;
  switch (fCurrentSpeed)
  {
    case WALK   : { out = "Walk"; break; }
    case JOG    : { out = "Jog"; break; }
    case RUN    : { out = "Run"; break; }
    case SPRINT : { out = "Sprint"; break; }
    default     : { out = "Unknown"; break; }
  }
  return out;
}

// ----------------------------------------
// nextSpeed()

void DriveController::nextSpeed(void)
{
  if ( fCurrentSpeed == SPRINT ) {
    fCurrentSpeed = WALK;
  } else {
    fCurrentSpeed++;
  }
}

// ========================================
// DomeController Class Functions
// ========================================

// ----------------------------------------
// Constructor

DomeController::DomeController(const char * mac = nullptr) : PSController(mac) {}

// ----------------------------------------
// Destructor

DomeController::~DomeController(void) {}

void DomeController::notify(void)
{
  uint32_t currentTime = millis();
  uint32_t lagTime = (currentTime > fLastTime) ? currentTime - fLastTime : 0;
  if (lagTime > 5000)
  {
    DEBUG_PRINTLN("More than 5 seconds. Disconnect");
    domeEmergencyStop();
    disconnect();
  }
  else if (lagTime > 300)
  {
    DEBUG_PRINTLN("It has been 300ms. Shutdown motors");
    domeEmergencyStop();
  }
  else
  {
    process();
  }
  fLastTime = currentTime;
}

// ----------------------------------------
// process()

void DomeController::process(void)
{
  if (!fGestureCollect)
  {
  #ifdef DOME_CONTROLLER_GESTURES
    if (event.button_up.l3)
    {
      DEBUG_PRINTLN("GESTURE START COLLECTING\n");
      disableDomeController();
      fGestureCollect = true;
      fGesturePtr = fGestureBuffer;
      fGestureTimeOut = millis() + GESTURE_TIMEOUT_MS;
    }
  #else
    // Event handling map these actions for your droid.
    // You can choose to either respond to key down or key up
    if (event.button_down.l3)
    {
      DEBUG_PRINTLN("DOME L3 DOWN");
    }
    else if (event.button_up.l3)
    {
      DEBUG_PRINTLN("DOME L3 UP");
    }
  #endif
    if (event.button_down.cross)
    {
      DEBUG_PRINTLN("DOME X DOWN");
    }
    else if (event.button_up.cross)
    {
      DEBUG_PRINTLN("DOME X UP");
    }

    if (event.button_down.circle)
    {
      DEBUG_PRINTLN("DOME O DOWN");
    }
    else if (event.button_up.circle)
    {
      DEBUG_PRINTLN("DOME O UP");
    }

    if (event.button_down.up)
    {
      DEBUG_PRINTLN("DOME Started pressing the up button");
    }
    else if (event.button_up.up)
    {
      DEBUG_PRINTLN("DOME Released the up button");
    }

    if (event.button_down.right)
    {
      DEBUG_PRINTLN("DOME Started pressing the right button");
    }
    else if (event.button_up.right)
    {
      DEBUG_PRINTLN("DOME Released the right button");
    }

    if (event.button_down.down)
    {
      DEBUG_PRINTLN("DOME Started pressing the down button");
    }
    else if (event.button_up.down)
    {
      DEBUG_PRINTLN("DOME Released the down button");
    }

    if (event.button_down.left)
    {
      DEBUG_PRINTLN("DOME Started pressing the left button");
    }
    else if (event.button_up.left)
    {
      DEBUG_PRINTLN("DOME Released the left button");
    }

    if (event.button_down.ps)
    {
      DEBUG_PRINTLN("DOME PS DOWN");
    }
    else if (event.button_up.ps)
    {
      DEBUG_PRINTLN("DOME PS UP");
    }
    return;
  }
  else if (fGestureTimeOut < millis())
  {
    DEBUG_PRINTLN("GESTURE TIMEOUT\n");
    enableDomeController();
    fGesturePtr = fGestureBuffer;
    fGestureCollect = false;
  }
  else
  {
    if (event.button_up.l3)
    {
      // delete trailing '5' from gesture
      unsigned glen = strlen(fGestureBuffer);
      if (glen > 0 && fGestureBuffer[glen-1] == '5')
          fGestureBuffer[glen-1] = 0;
      DEBUG_PRINT("GESTURE: "); DEBUG_PRINTLN(fGestureBuffer);
      enableDomeController();
      fGestureCollect = false;
    }
    if (event.button_up.cross)
      addGesture('X');
    if (event.button_up.circle)
      addGesture('O');
    if (event.button_up.up)
      addGesture('U');
    if (event.button_up.right)
      addGesture('R');
    if (event.button_up.down)
      addGesture('D');
    if (event.button_up.left)
      addGesture('L');
    if (event.button_up.ps)
      addGesture('P');
    if (!fGestureAxis)
    {
      if (abs(state.analog.stick.lx) > 50 && abs(state.analog.stick.ly) > 50)
      {
        // Diagonal
        if (state.analog.stick.lx < 0)
            fGestureAxis = (state.analog.stick.ly < 0) ? '1' : '7';
        else
            fGestureAxis = (state.analog.stick.ly < 0) ? '3' : '9';
        addGesture(fGestureAxis);
      }
      else if (abs(state.analog.stick.lx) > 100)
      {
        // Horizontal
        fGestureAxis = (state.analog.stick.lx < 0) ? '4' : '6';
        addGesture(fGestureAxis);
      }
      else if (abs(state.analog.stick.ly) > 100)
      {
        // Vertical
        fGestureAxis = (state.analog.stick.ly < 0) ? '2' : '8';
        addGesture(fGestureAxis);
      }
    }
    if (fGestureAxis && abs(state.analog.stick.lx) < 10 && abs(state.analog.stick.ly) < 10)
    {
      addGesture('5');
      fGestureAxis = 0;   
    }
  }
}

// ----------------------------------------
// onConnect()

void DomeController::onConnect(void)
{
  DEBUG_PRINTLN("Dome Stick Connected");
  setPlayer(2);
  enableDomeController();
  fLastTime = millis();
}

// ----------------------------------------
// onDisconnect()

void DomeController::onDisconnect(void)
{
  DEBUG_PRINTLN("Dome Stick Disconnected");
  disableDomeController();
}

// ----------------------------------------
// addGesture()

void DomeController::addGesture(char ch)
{
  if (fGesturePtr-fGestureBuffer < sizeof(fGestureBuffer)-1)
  {
    *fGesturePtr++ = ch;
    *fGesturePtr = '\0';
    fGestureTimeOut = millis() + GESTURE_TIMEOUT_MS;
  }
}

// ========================================
// Object instantiations
// ========================================

// ----------------------------------------
// driveStick Instantiation

DriveController driveStick(DRIVE_STICK_BT_ADDR);

// ----------------------------------------
// domeStick and domeDrive Instantiation

#if DOME_DRIVE != DOME_DRIVE_NONE && DRIVE_CONTROLLER_TYPE == kPS3Nav
// If dome drive is enabled and we are using PS3 Nav Controllers
DomeController domeStick(DOME_STICK_BT_ADDR);
#elif DOME_DRIVE != DOME_DRIVE_NONE
// Dome Drive enabled using either PS3 or PS4 controller
// the right side of the controller will be used to control the dome (left/right)
#define domeStick driveStick
#endif

#ifdef NEED_DOME_PWM_PINS
DomeDrivePWM domeDrive(servoDispatch, DOME_PWM_SETTINGS, domeStick);
#elif DOME_DRIVE == DOME_DRIVE_SABER
DomeDriveSabertooth domeDrive(129, DOME_DRIVE_SERIAL, domeStick);
#endif

void domeEmergencyStop()
{
#if DOME_DRIVE != DOME_DRIVE_NONE
  domeDrive.stop();
#endif
}

void enableDomeController()
{
#if DOME_DRIVE != DOME_DRIVE_NONE
  domeDrive.setEnable(true);
#endif
}

void disableDomeController()
{
#if DOME_DRIVE != DOME_DRIVE_NONE
  domeEmergencyStop();
  domeDrive.setEnable(false);
#endif
}

// ----------------------------------------
// servoDispatch Instantiation

ServoDispatchDirect<SizeOfArray(servoSettings)> servoDispatch(servoSettings);

// ----------------------------------------
// TankDrive Instantiation

#if DRIVE_SYSTEM == DRIVE_SYSTEM_SABER
// Tank Drive assign:
//    Serial1 for Sabertooth packet serial commands
TankDriveSabertooth tankDrive(128, Serial1, driveStick);

#elif DRIVE_SYSTEM == DRIVE_SYSTEM_PWM
// Tank Drive assign:
//    servo index 0 (LEFT_MOTOR_PWM)
//    servo index 1 (RIGHT_MOTOR_PWM)
//    servo index 2 (THROTTLE_MOTOR_PWM)
TankDrivePWM tankDrive(servoDispatch, DRIVE_PWM_SETTINGS, driveStick);

#elif DRIVE_SYSTEM == DRIVE_SYSTEM_ROBOTEQ_PWM
// Tank Drive assign:
//    servo index 0 (LEFT_MOTOR_PWM)
//    servo index 1 (RIGHT_MOTOR_PWM)
//    servo index 2 (THROTTLE_MOTOR_PWM)
TankDriveRoboteq tankDrive(servoDispatch, DRIVE_PWM_SETTINGS, driveStick);

#elif DRIVE_SYSTEM == DRIVE_SYSTEM_ROBOTEQ_SERIAL
// Tank Drive assign:
//    servo index 0 (LEFT_MOTOR_PWM)
//    servo index 1 (RIGHT_MOTOR_PWM)
//    servo index 2 (THROTTLE_MOTOR_PWM)
TankDriveRoboteq tankDrive(Serial1, driveStick);

#elif DRIVE_SYSTEM == DRIVE_SYSTEM_ROBOTEQ_PWM_SERIAL
// Tank Drive assign:
//    servo index 0 (LEFT_MOTOR_PWM)
//    servo index 1 (RIGHT_MOTOR_PWM)
//    servo index 2 (THROTTLE_MOTOR_PWM)
//    Serial1 for Roboteq serial commands
TankDriveRoboteq tankDrive(Serial1, servoDispatch, DRIVE_PWM_SETTINGS, driveStick);

#else
#error Unsupported DRIVE_SYSTEM
#endif

void emergencyStop()
{
  tankDrive.stop();
}

void enableController()
{
  tankDrive.setEnable(true);
}

void disableController()
{
  emergencyStop();
  tankDrive.setEnable(false);
}

bool getEnabled()
{
  return tankDrive.getEnable();
}
