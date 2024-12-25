#include "vex.h"

#include <fstream>
#include <sstream>

std::array<ControllerButtonInfo, 12> createControllerButtonArray(const vex::controller &controller)
{
    return {
        ControllerButtonInfo{&controller.ButtonA, "A"},
        ControllerButtonInfo{&controller.ButtonB, "B"},
        ControllerButtonInfo{&controller.ButtonX, "X"},
        ControllerButtonInfo{&controller.ButtonY, "Y"},
        ControllerButtonInfo{&controller.ButtonUp, "Up"},
        ControllerButtonInfo{&controller.ButtonDown, "Down"},
        ControllerButtonInfo{&controller.ButtonLeft, "Left"},
        ControllerButtonInfo{&controller.ButtonRight, "Right"},
        ControllerButtonInfo{&controller.ButtonL1, "L1"},
        ControllerButtonInfo{&controller.ButtonL2, "L2"},
        ControllerButtonInfo{&controller.ButtonR1, "R1"},
        ControllerButtonInfo{&controller.ButtonR2, "R2"}};
}

std::array<ControllerButtonInfo, 12> AllControllerButtons = createControllerButtonArray(primaryController);

std::array<ControllerButtonInfo, 12> getControllerButtonArray(const vex::controller &controller)
{
    return createControllerButtonArray(controller);
}

configManager ConfigManager("config.cfg", "maintenance.txt");

// Constructor
configManager::configManager(const std::string &configFileName, const std::string &maintenanceFileName)
    : driveMode(DriveMode::SplitArcade), configFileName(configFileName), maintenanceFileName(maintenanceFileName), maxOptionSize(4), logToFile(true),
      POLLINGRATE(5), PRINTLOGO(true), CTRLR1POLLINGRATE(25), logLevel(Log::Level::Info), odometer(0), lastService(0), serviceInterval(1000)
{
    readMaintenanceData();

    // Initialize triPorts with pointers to Brain.ThreeWirePort
    triPorts["A"] = &Brain.ThreeWirePort.A;
    triPorts["B"] = &Brain.ThreeWirePort.B;
    triPorts["C"] = &Brain.ThreeWirePort.C;
    triPorts["D"] = &Brain.ThreeWirePort.D;
    triPorts["E"] = &Brain.ThreeWirePort.E;
    triPorts["F"] = &Brain.ThreeWirePort.F;
    triPorts["G"] = &Brain.ThreeWirePort.G;
    triPorts["H"] = &Brain.ThreeWirePort.H;
}

vex::triport::port *configManager::getTriPort(const std::string &portName) const
{
    auto it = triPorts.find(portName);
    if (it != triPorts.end())
    {
        return it->second;
    }
    else
    {
        logHandler("configManager::getTriPort", "Triport not found: " + portName, Log::Level::Error, 5);
        return nullptr; // Return nullptr if not found
    }
}

// Setters with validation
void configManager::setMaxOptionSize(const std::size_t &value)
{
    if (value > 0)
    {
        maxOptionSize = value;
    }
    else
    {
        logHandler("configManager::setMaxOptionSize", "Max option size must be greater than 0", Log::Level::Error, 5);
    }
}

void configManager::setLogToFile(const bool &value)
{
    logToFile = value;
}

void configManager::setPollingRate(const std::size_t &value)
{
    if (value > 0)
    {
        POLLINGRATE = value;
    }
    else
    {
        logHandler("configManager::setPollingRate", "Polling rate must be greater than 0", Log::Level::Error, 5);
    }
}

void configManager::setPrintLogo(const bool &value)
{
    PRINTLOGO = value;
}

void configManager::setCtrlr1PollingRate(const std::size_t &value)
{
    if (value > 0)
    {
        CTRLR1POLLINGRATE = value;
    }
    else
    {
        logHandler("configManager::setCtrlr1PollingRate", "Controller 1 polling rate must be greater than 0", Log::Level::Error, 5);
    }
}

void configManager::setLogLevel(const Log::Level &value)
{
    logLevel = value;
}

void configManager::setTeamNumber(const std::string &value)
{
    if (!value.empty())
    {
        teamNumber = value;
    }
    else
    {
        logHandler("configManager::setTeamNumber", "Team number cannot be empty", Log::Level::Error, 5);
    }
}

void configManager::setLoadingGifPath(const std::string &value)
{
    if (!value.empty())
    {
        loadingGifPath = value;
    }
    else
    {
        logHandler("configManager::setLoadingGifPath", "Loading GIF path cannot be empty", Log::Level::Error, 5);
    }
}

void configManager::setAutoGifPath(const std::string &value)
{
    if (!value.empty())
    {
        autoGifPath = value;
    }
    else
    {
        logHandler("configManager::setAutoGifPath", "Auto GIF path cannot be empty", Log::Level::Error, 5);
    }
}

void configManager::setDriverGifPath(const std::string &value)
{
    if (!value.empty())
    {
        driverGifPath = value;
    }
    else
    {
        logHandler("configManager::setDriverGifPath", "Driver GIF path cannot be empty", Log::Level::Error, 5);
    }
}

void configManager::setCustomMessage(const std::string &value)
{
    customMessage = value;
}

int configManager::getMotorPort(const std::string &motorName) const
{
    auto it = motorPorts.find(motorName);
    if (it != motorPorts.end())
    {
        return it->second;
    }
    else
    {
        logHandler("configManager::getMotorPort", "Motor port not found: " + motorName, Log::Level::Error, 5);
        return -1; // Return an invalid port number if not found
    }
}

std::string configManager::getGearRatio(const std::string &motorName) const
{
    auto it = motorGearRatios.find(motorName);
    if (it != motorGearRatios.end())
    {
        return it->second;
    }
    else
    {
        logHandler("configManager::getGearRatio", "Motor gear ratio not found for: " + motorName + ". Using default ratio 18_1.", Log::Level::Warn, 3);
        return "18_1"; // Default ratio
    }
}

bool configManager::getMotorReversed(const std::string &motorName) const
{
    auto it = motorReversed.find(motorName);
    if (it != motorReversed.end())
    {
        return it->second;
    }
    else
    {
        logHandler("configManager::getMotorReversed", "Motor reversed state not found for: " + motorName + ". Using default state false.", Log::Level::Warn, 3);
        return false; // Default reversed state
    }
}

vex::gearSetting configManager::getGearSetting(const std::string &ratio) const
{
    if (ratio == "6_1")
    {
        return vex::gearSetting::ratio6_1;
    }
    else if (ratio == "18_1")
    {
        return vex::gearSetting::ratio18_1;
    }
    else if (ratio == "36_1")
    {
        return vex::gearSetting::ratio36_1;
    }
    else
    {
        logHandler("configManager::getGearSetting", "Invalid gear ratio: " + ratio + ". Using default ratio 18_1.", Log::Level::Warn, 3);
        return vex::gearSetting::ratio18_1; // Default
    }
}

void configManager::updateOdometer(const int &averagePosition)
{
    odometer += averagePosition;
    writeMaintenanceData();
}

void configManager::checkServiceInterval()
{
    if (odometer - lastService >= serviceInterval)
    {
        logHandler("Service", "Service needed! Distance: " + std::to_string(odometer), Log::Level::Warn, 5);
    }
}

configManager::ConfigType configManager::stringToConfigType(const std::string &str)
{
    if (str == "Brain")
    {
        return ConfigType::Brain;
    }
    else if (str == "Controller")
    {
        return ConfigType::Controller;
    }
    else
    {
        logHandler("configManager::stringToConfigType", "Invalid config type", Log::Level::Error, 5);
        return ConfigType::Brain; // Default return to avoid compilation error
    }
}

Log::Level configManager::stringToLogLevel(const std::string &str)
{
    if (str == "Trace")
        return Log::Level::Trace;
    else if (str == "Debug")
        return Log::Level::Debug;
    else if (str == "Info")
        return Log::Level::Info;
    else if (str == "Warn")
        return Log::Level::Warn;
    else if (str == "Error")
        return Log::Level::Error;
    else if (str == "Fatal")
        return Log::Level::Fatal;
    else
        logHandler("configManager::stringToLogLevel", "Invalid log level", Log::Level::Error, 5);
    return Log::Level::Info; // Default return to avoid compilation error
}