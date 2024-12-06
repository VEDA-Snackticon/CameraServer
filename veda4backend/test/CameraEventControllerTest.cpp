#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <drogon/drogon_test.h>
#include <controllers/TestController.h>
#include <drogon/HttpClient.h>

#include "Camera.h"
#include "controllers/cameraEventController.h"
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

DROGON_TEST(DateTimeTransiTionTest) {
    // 올바르게 date가 trantor::Date로 변환되는지 확인한다.
    std::shared_ptr<Json::Value> json = std::make_shared<Json::Value>();
    (*json)["description"]= "test";
    (*json)["localtime"] = "2024-10-22 18:27:12";
    (*json)["unixtime"] = 1701577082;

    cameraEventController eventController;
    trantor::Date translate_date = eventController.translateDate(json);
    CHECK(translate_date.toDbString() == "2024-10-22 18:27:12");
}

DROGON_TEST(DescriptionCheckTest) {
    // description 자체가 없으면 예외를 반환한다.

    std::shared_ptr<Json::Value> json = std::make_shared<Json::Value>();
    (*json)["description"]= "";
    (*json)["localtime"] = "2024-10-22 18:27:12";
    (*json)["unixtime"] = 1701577082;

    cameraEventController eventController;
    // 예외가 발생하는지 확인
    CHECK_THROWS_AS(eventController.checkDescription(json), std::runtime_error);
}
DROGON_TEST(eventSendTest) {
    // 다른 장비로 올바르게 요청이 나가는지 확인한다.
    setupEventCamera();
    boost::uuids::random_generator gen;
    boost::uuids::uuid uuid = gen();  // 랜덤 UUID 생성
    std::string transaction_id = boost::uuids::to_string(boost::uuids::random_generator()());

    std::shared_ptr<Json::Value> json = std::make_shared<Json::Value>();
    (*json)["description"]= "test";
    (*json)["localtime"] = "2024-10-22 18:27:12";
    (*json)["unixtime"] = 1701577082;

    cameraEventController eventController;
    auto transaction = app().getDbClient()->newTransaction();
    drogon_model::veda4::Camera camera = eventController.findCameraByDescription("test",transaction);
    camera.setIpAddr("https://85736fea-88ea-40bc-bfdc-b04627023da8.mock.pstmn.io");
    eventController.sendEventTo(json,HttpClient::newHttpClient(camera.getValueOfIpAddr()),transaction_id);


}


