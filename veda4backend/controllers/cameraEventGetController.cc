#include "cameraEventGetController.h"

#include <drogon/orm/Mapper.h>

#include "CameraEvent.h"

void cameraEventGetController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    auto transcation = app().getDbClient()->newTransaction();
    // write your application logic here
    orm::Mapper<drogon_model::veda4::CameraEvent> eventMapper(transcation);

    std::vector<drogon_model::veda4::CameraEvent> camera_events = eventMapper.findAll();


    Json::Value json;
    Json::Value cameraArray(Json::arrayValue);
    for (const auto& event : camera_events) {
        Json::Value cameraJson;
        cameraJson["transactionId"] = event.getValueOfTransactionId();       // 예: ID 필드
        cameraJson["camId"] = event.getValueOfEventCameraId(); // 예: IP 주소 필드
        cameraJson["time"] = event.getValueOfTime().toDbString();
        cameraArray.append(cameraJson);
    }
    json["events"] = cameraArray;

    auto response = HttpResponse::newHttpJsonResponse(json);
    callback(response);
}
