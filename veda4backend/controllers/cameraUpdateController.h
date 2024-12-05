#pragma once

#include <drogon/HttpSimpleController.h>

#include "Camera.h"

using namespace drogon;

class cameraUpdateController : public drogon::HttpSimpleController<cameraUpdateController>
{
  public:
    drogon_model::veda4::Camera findCameraByDescription(std::shared_ptr<Json::Value> &json);

    drogon_model::veda4::Camera &setGroupNumber(std::shared_ptr<Json::Value> json,
                                                drogon_model::veda4::Camera &updatedCamera);

    drogon_model::veda4::Camera setMasterSlave(std::shared_ptr<Json::Value> json,
                                               drogon_model::veda4::Camera updatedCamera);

    void handleCameraProcess(std::shared_ptr<Json::Value> json, drogon_model::veda4::Camera updatedCamera);

    void updateCamera(drogon_model::veda4::Camera updatedCamera, std::shared_ptr<drogon::orm::Transaction> transaction);

    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    // list path definitions here;
    // PATH_ADD("/path", "filter1", "filter2", HttpMethod1, HttpMethod2...);
    PATH_ADD("/cameraInfo",Patch);
    PATH_LIST_END
};
