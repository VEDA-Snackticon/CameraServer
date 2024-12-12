#include "cameraFileGetController.h"

void cameraFileGetController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
    auto transcation = app().getDbClient()->newTransaction();

// SQL 쿼리 작성: camera_file, camera_event, camera를 조인
std::string query = R"(
    SELECT
        camera_file.id AS file_id,
        camera_file.transaction_id,
        camera_file.file_name,
        camera_file.cam_id,
        camera_file.path,
        camera_event.time,
        camera.description,
        camera.ip_addr
    FROM
        camera_file
    LEFT JOIN
        camera_event
    ON
        camera_file.transaction_id = camera_event.transaction_id
    LEFT JOIN
        camera
    ON
        camera_file.cam_id = camera.id
)";

transcation->execSqlAsync(query, [callback](const drogon::orm::Result &result) {
    Json::Value json;
    Json::Value filesArray(Json::arrayValue);

    // 쿼리 결과를 JSON으로 변환
    for (const auto &row : result) {
        Json::Value fileJson;

        // NULL 값을 확인하며 처리
        fileJson["description"] = row["description"].isNull() ? "" : row["description"].as<std::string>();
        fileJson["ipAddr"] = row["ip_addr"].isNull() ? "" : row["ip_addr"].as<std::string>();
        fileJson["transactionId"] = row["transaction_id"].isNull() ? "" : row["transaction_id"].as<std::string>();
        fileJson["time"] = row["time"].isNull() ? "" : row["time"].as<std::string>();
        fileJson["fileName"] = row["file_name"].isNull() ? "" : row["file_name"].as<std::string>();
        fileJson["path"] = row["path"].as<std::string>();



        filesArray.append(fileJson);
    }

    json["files"] = filesArray;

    // JSON 응답 반환
    auto response = HttpResponse::newHttpJsonResponse(json);
    callback(response);
}, [callback](const orm::DrogonDbException  &e) {
    // 쿼리 실행 실패 시 에러 반환
    auto response = HttpResponse::newHttpJsonResponse(Json::Value{
        {"error", e.base().what()}
    });
    response->setStatusCode(drogon::k500InternalServerError);
    callback(response);
});

}
