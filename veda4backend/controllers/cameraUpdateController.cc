#include "cameraUpdateController.h"

#include <Camera.h>

void cameraUpdateController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
    try {

        std::shared_ptr<Json::Value> json = req->jsonObject();
        auto db_client = app().getDbClient();
        orm::Mapper<drogon_model::veda4::Camera> cameraMapper(db_client);
        std::string description = (*json)["description"].asString();
        std::vector<drogon_model::veda4::Camera> foundCamera = cameraMapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description, orm::CompareOperator::EQ, description));
        if (foundCamera.size() == 0) { throw std::runtime_error("No valid camera found"); }
        drogon_model::veda4::Camera updatedCamera = foundCamera[0];

        if (json->isMember("groupNumber")) {
            updatedCamera.setGroupNumber((*json)["groupNumber"].asInt());
        }
        if (json->isMember("isMaster")) {
            updatedCamera.setIsMaster((*json)["location"].asBool());
        }
        if (json->isMember("isActive")) {
            updatedCamera.setGroupNumber((*json)["groupNumber"].asInt());
        }
        cameraMapper.update(updatedCamera,[](long result){},[](const orm::DrogonDbException){std::cout << "error updating"<< std::endl;});

        auto http_response = HttpResponse::newHttpResponse();
        http_response->setBody("succes ");
        callback(http_response);

    }
    catch (std::exception &e) {
        std::shared_ptr<drogon::HttpResponse> response = HttpResponse::newHttpResponse();
        response->setBody(e.what());
        callback(response);
    }
}
