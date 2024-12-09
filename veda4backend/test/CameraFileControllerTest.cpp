#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <drogon/drogon_test.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpClient.h>
#include <drogon/orm/Mapper.h>

#include "Camera.h"
#include "CameraEvent.h"
#include "CameraFile.h"
//
// Created by lee on 2024. 12. 9..
//

std::string setupFileTest() {
    auto dbClient = drogon::app().getDbClient()->newTransaction();

    dbClient->execSqlSync("DELETE FROM camera_file");
    dbClient->execSqlSync("DELETE FROM camera_event");
    dbClient->execSqlSync("DELETE FROM camera_process");
    dbClient->execSqlSync("DELETE FROM camera");

    drogon_model::veda4::Camera camera;
    camera.setGroupNumber(1);
    camera.setDescription("test");
    camera.setIsMaster(true);
    camera.setIpAddr("127.0.0.1");
    camera.setIsAlive(true);

    drogon::orm::Mapper<drogon_model::veda4::Camera> cameraMapper(dbClient);
    drogon::orm::Mapper<drogon_model::veda4::CameraEvent> cameraEventMapper(dbClient);
    cameraMapper.insert(camera);
    auto foundCamera = cameraMapper.findBy(drogon::orm::Criteria(drogon_model::veda4::Camera::Cols::_description,drogon::orm::CompareOperator::EQ,"test"));
    drogon_model::veda4::Camera resCamera = foundCamera[0];
    drogon_model::veda4::CameraEvent cameraEvent;
    cameraEvent.setTime(trantor::Date::now());

    boost::uuids::random_generator gen;
    boost::uuids::uuid uuid = gen();  // 랜덤 UUID 생성
    std::string transaction_id = boost::uuids::to_string(boost::uuids::random_generator()());
    cameraEvent.setTransactionId(transaction_id);
    cameraEvent.setEventCameraId(resCamera.getValueOfId());
    cameraEventMapper.insert(cameraEvent);

    return  transaction_id;

}

void cleanupFileTest() {
    auto dbClient = drogon::app().getDbClient()->newTransaction();

    dbClient->execSqlSync("DELETE FROM camera_file");
    dbClient->execSqlSync("DELETE FROM camera_event");
    dbClient->execSqlSync("DELETE FROM camera_process");
    dbClient->execSqlSync("DELETE FROM camera");
}

DROGON_TEST(fileIntegrationTest) {
    // 요청시 파일이 올바르게 업로드 된다.
    char cwd[PATH_MAX];  // 현재 작업 디렉토리를 저장할 배열
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "Current working directory: " << cwd << std::endl;
    } else {
        perror("getcwd() error");  // 에러 발생 시 출력
    }
    std::string transactionId= setupFileTest();
    auto client  = drogon::HttpClient::newHttpClient("127.0.0.1",5555,false);

    std::vector<drogon::UploadFile> uploadVector;
    std::string filePath = "/Users/lee/Documents/veda/teamproject/VEDA4TeamProject/CameraServer/veda4backend/cmake-build-debug/test/tmp/test.mp4";
    uploadVector.push_back(drogon::UploadFile(filePath,"test.mp4"));
    std::cout <<uploadVector[0].path() << std::endl;
    drogon::HttpRequestPtr request = drogon::HttpRequest::newFileUploadRequest(uploadVector);
    request->setPath("/file");
    request->setMethod(drogon::Post);
    request->setParameter("description", "test");
    request->setParameter("transactionId", transactionId);
    auto response = client->sendRequest(request);

    std::cout << "response :" << response.second->body() << std::endl;

    cleanupFileTest();
}

DROGON_TEST(transactionIdCheckTest) {
    //동일한 transactionId에 대해서 처리됨을 확인한다.
}