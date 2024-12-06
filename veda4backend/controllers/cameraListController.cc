#include "cameraListController.h"

#include <drogon/orm/Mapper.h>

#include "Camera.h"

void cameraListController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here

    std::shared_ptr<drogon::orm::Transaction> transaction = app().getDbClient()->newTransaction();

    orm::Mapper<drogon_model::veda4::Camera> cameraMapper(transaction);

    std::vector<drogon_model::veda4::Camera> allCamera = cameraMapper.findAll();

    Json::Value json;
    Json::Value cameraArray(Json::arrayValue);
    for (const auto& camera : allCamera) {
        Json::Value cameraJson;
        cameraJson["description"] = camera.getValueOfDescription();       // 예: ID 필드
        cameraJson["ipAddr"] = camera.getValueOfIpAddr(); // 예: IP 주소 필드
        cameraJson["groupNumber"] = camera.getValueOfGroupNumber();
        cameraJson["isMaster"] = camera.getValueOfIsMaster();
        cameraArray.append(cameraJson);
    }

    json["cameras"] = cameraArray;
    std::shared_ptr<drogon::HttpResponse> response = HttpResponse::newHttpJsonResponse(json);
    callback(response);
}
