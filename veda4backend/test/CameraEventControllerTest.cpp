#include <drogon/drogon_test.h>
#include <controllers/TestController.h>
#include <drogon/HttpClient.h>

#include "Camera.h"
//
// Created by lee on 2024. 12. 5
//

drogon_model::veda4::Camera setupEventCamera() {
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlSync("DELETE FROM camera_event");
    dbClient->execSqlSync("DELETE FROM camera_process");
    dbClient->execSqlSync("DELETE FROM camera_file");
    dbClient->execSqlSync("DELETE FROM camera");

    drogon_model::veda4::Camera camera;
    camera.setGroupNumber(1);
    camera.setDescription("test");
    camera.setIsMaster(true);
    camera.setIpAddr("127.0.0.1");
    camera.setIsAlive(true);

    drogon::orm::Mapper<drogon_model::veda4::Camera> cameraMapper(dbClient);
    cameraMapper.insert(camera);
    return camera;


}
void cleanEvent() {
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlSync("DELETE FROM camera_event");
    dbClient->execSqlSync("DELETE FROM camera_process");
    dbClient->execSqlSync("DELETE FROM camera_file");
    dbClient->execSqlSync("DELETE FROM camera");
}


DROGON_TEST(EventReceiveTest)
 {
    setupEventCamera();
    Json::Value json;
    json["description"]= "test";
    json["localtime"] = "2024-10-22 18:27:12";
    json["unixtime"] = 1701577082;

    auto request = drogon::HttpRequest::newHttpJsonRequest(json);
    request->setMethod(Post);
    request->setPath("/event");
    std::shared_ptr<drogon::HttpClient> client = drogon::HttpClient::newHttpClient("127.0.0.1", 5555, false);
    std::pair<drogon::ReqResult, HttpResponsePtr> response = client->sendRequest(request);
    CHECK(response.first == drogon::ReqResult::Ok);
    CHECK(response.second->getBody() == "success");
    cleanEvent();
 }
