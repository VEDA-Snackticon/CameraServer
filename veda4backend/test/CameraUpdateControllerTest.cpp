#include <unistd.h>
#include <drogon/drogon_test.h>
#include <drogon/HttpClient.h>
#include <controllers/TestController.h>

#include "Camera.h"
#include "CameraProcess.h"
#include "controllers/cameraUpdateController.h"

void clean() {
    auto dbClient = app().getDbClient();
    dbClient->execSqlSync("DELETE FROM camera_process");
    dbClient->execSqlSync("DELETE FROM camera_file");
    dbClient->execSqlSync("DELETE FROM camera_event");
    dbClient->execSqlSync("DELETE FROM camera");

}

void clean(std::shared_ptr<drogon::orm::Transaction> dbClient) {
    dbClient->execSqlSync("DELETE FROM camera_process");
    dbClient->execSqlSync("DELETE FROM camera_file");
    dbClient->execSqlSync("DELETE FROM camera_event");
    dbClient->execSqlSync("DELETE FROM camera");

}

drogon_model::veda4::Camera setupCamera() {
    clean();
    drogon_model::veda4::Camera camera;
    camera.setGroupNumber(1);
    camera.setDescription("test");
    camera.setIsMaster(true);
    camera.setIpAddr("127.0.0.1");
    camera.setIsAlive(true);

    orm::Mapper<drogon_model::veda4::Camera> cameraMapper(app().getDbClient());
    cameraMapper.insert(camera);
    return camera;
}

DROGON_TEST(updateTest) {

    setupCamera();

    Json::Value json;
    json["description"]= "test";
    json["isMaster"] = false;
    json["groupNumber"] = 88;
    json["processes"] = Json::Value(Json::arrayValue);  // 배열 타입 선언
    json["processes"].append("process1");
    json["processes"].append("process2");

    auto request = HttpRequest::newHttpJsonRequest(json);
    request->setMethod(Patch);
    request->setPath("/cameraInfo");
    std::shared_ptr<drogon::HttpClient> client = drogon::HttpClient::newHttpClient("127.0.0.1", 5555, false);
    client->sendRequest(request,[TEST_CTX](const drogon::ReqResult& result, const drogon::HttpResponsePtr& resp) {
        REQUIRE(result == drogon::ReqResult::Ok);
        CHECK(resp->getBody() == "success");
        clean();
    });


}

DROGON_TEST(updateUnitTest) {
    clean();


    drogon_model::veda4::Camera camera;
    camera.setGroupNumber(1);
    camera.setDescription("test");
    camera.setIsMaster(true);
    camera.setIpAddr("127.0.0.1");
    camera.setIsAlive(true);
    auto client = app().getDbClient();
    std::shared_ptr<drogon::orm::Transaction> transaction = client->newTransaction();
    orm::Mapper<drogon_model::veda4::Camera> cameraMapper(transaction);

    cameraMapper.insert(camera);
    CHECK(cameraMapper.findAll().size() == 1);
     auto foundCamera = cameraMapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description,orm::CompareOperator::EQ,"test"));
    CHECK(foundCamera.size() ==1);
    drogon_model::veda4::Camera updateCamera = foundCamera[0];
    cameraUpdateController updateController;
    updateCamera.setGroupNumber(88);
    updateCamera.setIsMaster(false);
    updateController.updateCamera(updateCamera,transaction);

    auto foundUpdateCamera = cameraMapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description,orm::CompareOperator::EQ,"test"));
    CHECK(foundUpdateCamera.size() ==1);
    drogon_model::veda4::Camera tempCamera = foundUpdateCamera[0];

    CHECK(*tempCamera.getGroupNumber() == 88);
    CHECK(*tempCamera.getIsMaster() == false);

}

DROGON_TEST(groupNumberUnitTest) {

    cameraUpdateController updateController;
    std::shared_ptr<Json::Value> json = std::make_shared<Json::Value>();
    (*json)["groupNumber"] = 99;

    drogon_model::veda4::Camera camera;
    camera.setGroupNumber(1);
    drogon_model::veda4::Camera group_number = updateController.setGroupNumber(json,camera);
    CHECK(*group_number.getGroupNumber() == 99);
}

DROGON_TEST(processTest) {
    auto transaction = app().getDbClient()->newTransaction();
    drogon_model::veda4::Camera  updatedCamera = setupCamera();

    drogon_model::veda4::CameraProcess newProcess;
    newProcess.setCameraId(updatedCamera.getValueOfId());
    std::shared_ptr<Json::Value> json = std::make_shared<Json::Value>();
    (*json)["description"]= "test";
    (*json)["isMaster"] = false;
    (*json)["groupNumber"] = 88;
    (*json)["processes"] = Json::Value(Json::arrayValue);  // 배열 타입 선언
    (*json)["processes"].append("process1");
    (*json)["processes"].append("process2");
    (*json)["processes"].append("process3");
    orm::Mapper<drogon_model::veda4::CameraProcess> cameraProcessMapper(transaction);
    const auto &processes = (*json)["processes"];
    if (processes.empty()) return;
    for (const auto &processName : processes)
    {
        if (!processName.isString())
        {
            throw std::runtime_error("Invalid process name");
        }
        drogon_model::veda4::CameraProcess newProcess;
        newProcess.setCameraId(updatedCamera.getValueOfId());
        newProcess.setProcessName(processName.asString());
        cameraProcessMapper.insert(newProcess);
    }

    orm::Mapper<drogon_model::veda4::Camera> cameraMapper(transaction);
    std::vector<drogon_model::veda4::Camera> cameras = cameraMapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description,orm::CompareOperator::EQ,"test"));
    CHECK(cameras.size() == 1 );
    drogon_model::veda4::Camera camera = cameras[0];

    std::shared_ptr<Json::Value> json2 = std::make_shared<Json::Value>();
    (*json2)["description"]= "test";
    (*json2)["isMaster"] = false;
    (*json2)["groupNumber"] = 88;
    (*json2)["processes"] = Json::Value(Json::arrayValue);  // 배열 타입 선언
    (*json2)["processes"].append("process1");

    cameraUpdateController updateController;
    updateController.handleCameraProcess(json2,camera,transaction);

    std::vector<drogon_model::veda4::Camera> checkCameras = cameraMapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description,orm::CompareOperator::EQ,"test"));
    CHECK(checkCameras.size() == 1 );
    clean(transaction);

}