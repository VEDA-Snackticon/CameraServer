#include "cameraUpdateController.h"

#include <Camera.h>
#include <CameraProcess.h>

drogon_model::veda4::Camera cameraUpdateController::findCameraByDescription(std::shared_ptr<Json::Value> &json){

    auto db_client = app().getDbClient();
    orm::Mapper<drogon_model::veda4::Camera> cameraMapper(db_client);
    std::string description = (*json)["description"].asString();
    std::vector<drogon_model::veda4::Camera> foundCamera = cameraMapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description, orm::CompareOperator::EQ, description));
    if (foundCamera.size() == 0) { throw std::runtime_error("No valid camera found"); }
    return foundCamera[0];
}

drogon_model::veda4::Camera &cameraUpdateController::setGroupNumber(std::shared_ptr<Json::Value> json,
                                                                    drogon_model::veda4::Camera &updatedCamera) {
    if (json->isMember("groupNumber")) {
        updatedCamera.setGroupNumber((*json)["groupNumber"].asInt());
        return updatedCamera;
    }
    return updatedCamera;
}

drogon_model::veda4::Camera cameraUpdateController::setMasterSlave(std::shared_ptr<Json::Value> json,
                                                                   drogon_model::veda4::Camera updatedCamera) {
    if (json->isMember("isMaster")) {
        updatedCamera.setIsMaster((*json)["isMaster"].asBool());
        return updatedCamera;
    }
    return updatedCamera;
}

void cameraUpdateController::handleCameraProcess(std::shared_ptr<Json::Value> json, drogon_model::veda4::Camera updatedCamera, std::shared_ptr<drogon::orm::Transaction> transaction ) {

    if (json->isMember("processes"))
    {
        const auto &processes = (*json)["processes"];
        if (!processes.isArray())
        {
            throw std::runtime_error("Processes must be an array");
        }

        orm::Mapper<drogon_model::veda4::CameraProcess> cameraProcessMapper(transaction);
        transaction->execSqlSync("DELETE FROM camera_process WHERE camera_id = ?", updatedCamera.getValueOfId());
        if (!processes.empty())
        {
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
        }

    }
}

void cameraUpdateController::updateCamera(drogon_model::veda4::Camera updatedCamera,std::shared_ptr<drogon::orm::Transaction> transaction ) {
    orm::Mapper<drogon_model::veda4::Camera> cameraMapper(transaction);
    cameraMapper.update(updatedCamera);
}

void cameraUpdateController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
    try {
        std::shared_ptr<Json::Value> json = req->jsonObject();
        auto db_client = app().getDbClient();
        std::shared_ptr<drogon::orm::Transaction> transaction = db_client->newTransaction();
        drogon_model::veda4::Camera updatedCamera = findCameraByDescription(json);

        updatedCamera =setGroupNumber(json, updatedCamera);
        updatedCamera = setMasterSlave(json, updatedCamera);
        handleCameraProcess(json, updatedCamera, transaction);
        updateCamera(updatedCamera, transaction);

        auto http_response = HttpResponse::newHttpResponse();
        http_response->setBody("success");
        callback(http_response);

    }
    catch (std::exception &e) {
        std::shared_ptr<drogon::HttpResponse> response = HttpResponse::newHttpResponse();
        response->setStatusCode(k404NotFound);
        response->setBody(e.what());
        callback(response);
    }
}
