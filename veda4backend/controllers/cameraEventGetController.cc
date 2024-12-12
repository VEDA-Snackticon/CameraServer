#include "cameraEventGetController.h"

#include <drogon/orm/Mapper.h>

#include "CameraEvent.h"

void cameraEventGetController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    auto transcation = app().getDbClient()->newTransaction();
    // SQL 쿼리 작성: camera_event와 camera를 조인
    std::string query = R"(
    SELECT
        camera_event.transaction_id,
        camera_event.event_camera_id,
        camera_event.time,
        camera.description,
        camera.is_alive,
        camera.ip_addr
    FROM
        camera_event
    LEFT JOIN
        camera
    ON
        camera_event.event_camera_id = camera.id
)";

    transcation->execSqlAsync(query, [callback](const drogon::orm::Result &result) {
        Json::Value json;
        Json::Value eventsArray(Json::arrayValue);

        // 쿼리 결과를 JSON으로 변환
        for (const auto &row : result) {
            Json::Value eventJson;
            eventJson["transactionId"] = row["transaction_id"].as<std::string>();
            eventJson["time"] = row["time"].as<std::string>();
            eventJson["description"] = row["description"].isNull() ? "" : row["description"].as<std::string>();
            eventJson["ipAddr"] = row["ip_addr"].isNull() ? "" : row["ip_addr"].as<std::string>();
            eventsArray.append(eventJson);
        }

        json["events"] = eventsArray;
        // JSON 응답 반환
        auto response = HttpResponse::newHttpJsonResponse(json);
        callback(response);

    }, [callback](const orm::DrogonDbException &e) {
        // 쿼리 실행 실패 시 에러 반환
        auto response = HttpResponse::newHttpJsonResponse(Json::Value{
            {"error", e.base().what()}
        });
        response->setStatusCode(drogon::k500InternalServerError);
        callback(response);
    });
}
