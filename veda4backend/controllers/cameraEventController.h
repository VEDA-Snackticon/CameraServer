#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

class cameraEventController : public drogon::HttpSimpleController<cameraEventController>
{
  public:
    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    // list path definitions here;
    // PATH_ADD("/path", "filter1", "filter2", HttpMethod1, HttpMethod2...);
    PATH_ADD("/event",Post);
    PATH_LIST_END
};
