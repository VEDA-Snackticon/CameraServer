#pragma once

#include <Camera.h>
#include <drogon/HttpSimpleController.h>

using namespace drogon;

class cameraInfoController : public drogon::HttpSimpleController<cameraInfoController>
{
  public:
    void check_json_data(std::shared_ptr<Json::Value> values);

    drogon_model::veda4::Camera &setUpEntity(std::string ip,
                                             std::string description, drogon_model::veda4::Camera &camera);

    void updateCameraEntity(std::string ip,
                            orm::Mapper<drogon_model::veda4::Camera> &mapper,
                            std::string description, drogon_model::veda4::Camera &camera);

    void insertCameraEntity(std::string ip,
                            orm::Mapper<drogon_model::veda4::Camera> &mapper, std::string description, drogon_model::veda4::Camera &camera);

    bool isAlreadyInserted(std::__1::vector<drogon_model::veda4::Camera> foundCamera);

    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    // list path definitions here;
    // PATH_ADD("/path", "filter1", "filter2", HttpMethod1, HttpMethod2...);
    PATH_ADD("/setup",Get);
    PATH_LIST_END
};
