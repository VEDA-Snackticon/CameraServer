#include "cameraInfoController.h"
#include "Camera.h"

void cameraInfoController::check_json_data(std::shared_ptr<Json::Value> values) {
    if (values == nullptr) {
        throw std::runtime_error("No data");
    }
}

drogon_model::veda4::Camera & cameraInfoController::setUpEntity(std::string ip, std::string description,
                                                              drogon_model::veda4::Camera & camera) {
    camera.setDescription(description);
    camera.setIpAddr(ip);
    camera.setGroupNumber(0);
    camera.setIsAlive(true);
    camera.setIsMaster(false);

    return camera;
}

void cameraInfoController::updateCameraEntity(std::string ip, orm::Mapper<drogon_model::veda4::Camera> & mapper, std::string description,drogon_model::veda4::Camera &camera) {
    setUpEntity(ip, description, camera);
    mapper.update(camera,[](const int num){},[](const orm::DrogonDbException &exception){});
}

void cameraInfoController::insertCameraEntity(std::string ip, orm::Mapper<drogon_model::veda4::Camera> &mapper, std::string description, drogon_model::veda4::Camera& camera) {
    setUpEntity(ip,description,camera);
    mapper.insert(camera,[](const drogon_model::veda4::Camera & result){}, [](const orm::DrogonDbException &exception){});
}

bool cameraInfoController::isAlreadyInserted(std::__1::vector<drogon_model::veda4::Camera> foundCamera) {
    return foundCamera.size() != 0;
}

void cameraInfoController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) {
    try {
        auto dbClient = app().getDbClient();
        orm::Mapper<drogon_model::veda4::Camera> mapper(dbClient);

        auto values = req->jsonObject();
        check_json_data(values);
        std::string description =  (*values)["description"].asString();

        if (description.empty()){ throw std::runtime_error("No description"); }


        std::vector<drogon_model::veda4::Camera> foundCamera = mapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description,orm::CompareOperator::EQ,description));

        drogon_model::veda4::Camera camera;
        if(isAlreadyInserted(foundCamera)) {
            camera = foundCamera[0];
            updateCameraEntity(req->peerAddr().toIp(), mapper, description, camera);
        }
        else {
            insertCameraEntity(req->peerAddr().toIp(), mapper, description, camera);
        }

        auto response = HttpResponse::newHttpResponse();
        response->setBody("register success");
        callback(response);
    }
    catch (std::exception &e) {
        auto response_ptr = HttpResponse::newHttpResponse();
        response_ptr->addHeader("Content-Type", "application/json");
        response_ptr->addHeader("Access-Control-Allow-Origin", "*");
        response_ptr->setBody(" Error Occured");
        callback(response_ptr);
    }
    // write your application logic here

    std::shared_ptr<Json::Value> values = req->jsonObject();
}

