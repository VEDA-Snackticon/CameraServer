#pragma once

#include <drogon/HttpClient.h>
#include <drogon/HttpSimpleController.h>

#include "Camera.h"

using namespace drogon;

class cameraEventController : public drogon::HttpSimpleController<cameraEventController>
{
  public:
    std::string checkDescription(std::shared_ptr<Json::Value> values);

    trantor::Date translateDate(std::shared_ptr<Json::Value> values);

    template<class>
    void findCameraByDescription(std::string description, class transaction);

    drogon_model::veda4::Camera findCameraByDescription(std::string description, std::shared_ptr<drogon::orm::Transaction> transaction);

    void checkSendingPolicy(drogon_model::veda4::Camera camera);

    std::vector<drogon_model::veda4::Camera> findCamerasByGroupNumber(drogon_model::veda4::Camera camera, std::shared_ptr<drogon::orm::Transaction>
                                                                      transaction);

    std::string saveCameraEvent(std::shared_ptr<orm::Transaction> db_client, drogon_model::veda4::Camera camera, trantor::Date eventTime);

    void sendEventTo(std::shared_ptr<Json::Value> values, std::shared_ptr<drogon::HttpClient> client, std::string transaction_id);

    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    // list path definitions here;
    // PATH_ADD("/path", "filter1", "filter2", HttpMethod1, HttpMethod2...);
    PATH_ADD("/event",Post);
    PATH_LIST_END
};
