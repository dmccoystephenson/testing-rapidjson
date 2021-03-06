#include <string>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
int indent = 0;
bool debugFlag = true;

/**
 * Log a message to the console.
 */
void log(std::string message) {
    std::cout << "[LOG] " << message << std::endl;
}

/**
 * Log a debug message to the console.
 */
void debug(std::string message) {
    if (debugFlag) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}

/**
 * Convert a string into a document.
 */
rapidjson::Document getDocument(std::string jsonString) {
    rapidjson::Document document;
    document.Parse(jsonString.c_str());
    return document;
}

/**
 * Convert a document into a string.
 */
std::string stringifyDocument(rapidjson::Document& document) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return buffer.GetString();
}

std::string getIndent(int numSpaces) {
    std::string toReturn = "";
    for (int i = 0; i < numSpaces*2; i++) {
        toReturn = toReturn + " ";
    }
    return toReturn;
}

void printValue(rapidjson::Value& valueToPrint) {
    if (valueToPrint.IsObject()) {
        for (auto& m : valueToPrint.GetObject()) {
            std::string name = m.name.GetString();
            std::string type = kTypeNames[m.value.GetType()];
            std::cout << getIndent(indent) << name  << " - " << type << std::endl;
            if (type == "Object" || type == "Array") {
                indent++;
                auto& value = valueToPrint[name.c_str()];
                printValue(value);
                indent--;
            }
        }
    }
    else if (valueToPrint.IsArray()) {
        for (auto& m : valueToPrint.GetArray()) {
            std::string type = kTypeNames[m.GetType()];
            if (type == "Object" || type == "Array") {
                indent++;
                printValue(m);
                indent--;
            }
        }
    }
}

void printDocument(rapidjson::Document& document) {
    printValue(document);
}

/**
 * @brief Recursively search for a member in a value and remove it. Returns when the member is found and removed for the first time.
 * 
 * @param value The value to begin searching.
 * @param member The member to remove.
 * @param success The flag that the caller can check upon return to see if the operation was successful.
 */
void findAndRemoveMember(rapidjson::Value& value, std::string member, bool& success) {
    if (success) {
        // return if search has already succeeded
        return;
    }
    if (value.IsObject()) {
        if (value.HasMember(member.c_str())) {
            value.RemoveMember(member.c_str());
            success = true;
            return;
        }
        for (auto& m : value.GetObject()) {
            std::string type = kTypeNames[m.value.GetType()];
            if (type == "Object" || type == "Array") {
                std::string name = m.name.GetString();
                auto& v = value[name.c_str()];
                findAndRemoveMember(v, member, success);
            }
        }
    }
    else if (value.IsArray()) {
        for (auto& m : value.GetArray()) {
            std::string type = kTypeNames[m.GetType()];
            if (type == "Object" || type == "Array") {
                findAndRemoveMember(m, member, success);
            }
        }
    }
}

/**
 * @brief Recursively check if a member is present. Returns upon finding the first instance of the member.
 * 
 * @param value The value to begin searching.
 * @param member The member to remove.
 * @param success The flag that the caller can check upon return to see if the operation was successful.
 */
void isMemberPresent(rapidjson::Value& value, std::string member, bool& success) {
    if (success) {
        // return if search has already succeeded
        return;
    }
    if (value.IsObject()) {
        if (value.HasMember(member.c_str())) {
            success = true;
        }
        for (auto& m : value.GetObject()) {
            std::string type = kTypeNames[m.value.GetType()];
            if (type == "Object" || type == "Array") {
                std::string name = m.name.GetString();
                auto& v = value[name.c_str()];
                isMemberPresent(v, member, success);
            }
        }
    }
    else if (value.IsArray()) {
        for (auto& m : value.GetArray()) {
            std::string type = kTypeNames[m.GetType()];
            if (type == "Object" || type == "Array") {
                isMemberPresent(m, member, success);
            }
        }
    }
}

/**
 * Get a pre-defined document for testing purposes.
 */
rapidjson::Document getTestDocument() {
    const char* jsonString = "{\"metadata\":{\"latency\":1,\"logFileName\":\"wsmpforward.coer\",\"payloadType\":\"us.dot.its.jpo.ode.model.OdeBsmPayload\",\"receivedAt\":\"2017-08-02T19:56:45.822Z[UTC]\",\"sanitized\":false,\"schemaVersion\":1,\"serialId\":{\"bundleId\":4,\"bundleSize\":1,\"recordId\":2,\"serialNumber\":0,\"streamId\":\"0bfda39b-0bf1-4e2e-a1f1-b858426f7408\"},\"validSignature\":false},\"payload\":{\"data\":{\"coreData\":{\"accelSet\":{\"accelYaw\":0},\"accuracy\":{\"semiMajor\":12.7,\"semiMinor\":12.7},\"brakes\":{\"abs\":\"unavailable\",\"auxBrakes\":\"unavailable\",\"brakeBoost\":\"unavailable\",\"scs\":\"unavailable\",\"traction\":\"unavailable\",\"wheelBrakes\":{\"leftFront\":false,\"leftRear\":false,\"rightFront\":false,\"rightRear\":false,\"unavailable\":true}},\"heading\":321.0125,\"id\":\"G1\",\"msgCnt\":1,\"position\":{\"elevation\":154.7,\"latitude\":35.94911,\"longitude\":-83.928343},\"secMark\":36799,\"size\":{\"length\":250,\"width\":150},\"speed\":22.1},\"partII\":[{\"id\":\"VEHICLESAFETYEXT\",\"value\":{\"pathHistory\":{\"crumbData\":[{\"elevationOffset\":-19.8,\"latOffset\":7.55e-5,\"lonOffset\":0.0002609,\"timeOffset\":32.2},{\"elevationOffset\":-25.8,\"latOffset\":7.32e-5,\"lonOffset\":0.0003135,\"timeOffset\":34},{\"elevationOffset\":-34.5,\"latOffset\":0.0001027,\"lonOffset\":0.0004479,\"timeOffset\":37.2},{\"elevationOffset\":-128.2,\"latOffset\":0.000232,\"lonOffset\":0.0011832,\"timeOffset\":73.44}]},\"pathPrediction\":{\"confidence\":50,\"radiusOfCurve\":0}}},{\"id\":\"SUPPLEMENTALVEHICLEEXT\",\"value\":{\"classDetails\":{\"fuelType\":\"UNKNOWNFUEL\",\"hpmsType\":\"NONE\",\"keyType\":0,\"regional\":[],\"role\":\"BASICVEHICLE\"},\"regional\":[],\"vehicleData\":{\"bumpers\":{\"front\":0.5,\"rear\":0.6},\"height\":1.9},\"weatherProbe\":{},\"accelLong\":\"test\",\"accelLat\":\"test\",\"accelVert\":\"test\",\"yawRate\":\"test\",\"steeringWheelAngle\":\"test\",\"leftTurnSignalOn\":\"test\",\"rightTurnSignalOn\":\"test\",\"hazardSignalOn\":\"test\",\"fogLightOn\":\"test\",\"lowBeamHeadlightsOn\":\"test\",\"highBeamHeadlightsOn\":\"test\",\"automaticLightControlOn\":\"test\",\"daytimeRunningLightsOn\":\"test\",\"parkingLightsOn\":\"test\",\"wiperStatusFront\":\"test\",\"wiperStatusRear\":\"test\",\"wiperRateFront\":\"test\",\"wiperRateRear\":\"test\",\"eventAirBagDeployment\":\"test\",\"sunSensor\":\"test\",\"coefficientOfFriction\":\"test\",\"ambientAirTemperature\":\"test\",\"ambientAirPressure\":\"test\",\"transmissionState\":\"test\",\"vehicleSpeed\":\"test\",\"antiLockBrakeStatus\":\"test\",\"stabilityControlStatus\":\"test\",\"tractionControlStatus\":\"test\",\"brakeBoostApplied\":\"test\",\"brakeAppliedStatus\":\"test\",\"auxiliaryBrakeStatus\":\"test\",\"tirePressure\":\"test\",\"acceleratorPedalPosition\":\"test\",\"brakePedalPosition\":\"test\",\"disabledVehicle\":\"test\",\"stalledVehicle\":\"test\",\"airBagDeployment\":\"test\"}}]},\"dataType\":\"us.dot.its.jpo.ode.plugin.j2735.J2735Bsm\",\"schemaVersion\":1},\"schemaVersion\":1}";
    return getDocument(jsonString);
}

int main() {
    log("Executing program");

    rapidjson::Document document = getTestDocument();
    // printDocument(document);

    std::string members[] = { 
                                "accelLong",
                                "accelLat",
                                "accelVert",
                                "yawRate",
                                "steeringWheelAngle",
                                "leftTurnSignalOn",
                                "rightTurnSignalOn",
                                "hazardSignalOn",
                                "fogLightOn",
                                "lowBeamHeadlightsOn",
                                "highBeamHeadlightsOn",
                                "automaticLightControlOn",
                                "daytimeRunningLightsOn",
                                "parkingLightsOn",
                                "wiperStatusFront",
                                "wiperStatusRear",
                                "wiperRateFront",
                                "wiperRateRear",
                                "eventAirBagDeployment",
                                "sunSensor",
                                "coefficientOfFriction",
                                "ambientAirTemperature",
                                "ambientAirPressure",
                                "transmissionState",
                                "vehicleSpeed",
                                "antiLockBrakeStatus",
                                "stabilityControlStatus",
                                "tractionControlStatus",
                                "brakeBoostApplied",
                                "brakeAppliedStatus",
                                "auxiliaryBrakeStatus",
                                "tirePressure",
                                "acceleratorPedalPosition",
                                "brakePedalPosition",
                                "disabledVehicle",
                                "stalledVehicle",
                                "airBagDeployment",
                            };

    auto& payload = document["payload"];
    auto& data = payload["data"];
    auto& partII = data["partII"];
    // printValue(partII);

    for (int i = 0 ; i < sizeof(members)/sizeof(members[0]); i++) {
        std::string member = members[i];

        bool psuccess1 = false;
        isMemberPresent(partII, member, psuccess1);
        std::cout << "Is the '" << member << "' member present... Before redaction? " << psuccess1;

        bool rsuccess = false;
        findAndRemoveMember(partII, member, rsuccess);

        bool psuccess2 = false;
        isMemberPresent(partII, member, psuccess2);
        std::cout << " ----- After redaction? " << psuccess2 << std::endl;
    }

    // printValue(partII);

    log("Program finished executing");
    return 0;
}