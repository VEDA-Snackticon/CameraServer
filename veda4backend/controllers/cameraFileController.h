#pragma once

#include <drogon/HttpSimpleController.h>

#include "Camera.h"
#include "CameraEvent.h"
#include "CameraFile.h"

using namespace drogon;

class cameraFileController : public drogon::HttpSimpleController<cameraFileController>
{
  public:
    std::string getTransactionId(SafeStringMap<std::string> pairs);

    std::string getDescription(SafeStringMap<std::string> pairs);

    bool checkParameter(std::function<void(const HttpResponsePtr &)> &&callback, std::string transactionId,
                        std::string description, std::vector<HttpFile> files);

    std::string setUploadPath();

    bool checkSaveFiles(std::function<void(const HttpResponsePtr &)> &&callback, std::vector<std::string> savedFiles);

    bool checkCameraEvent(std::function<void(const HttpResponsePtr &)> &&callback,
                          std::vector<drogon_model::veda4::CameraEvent> camera_events, std::string transactionId);

    void logFile(const HttpFile &file, std::string fileExtension);

    std::string getFileExtension(std::string fileExtension);

    bool checkCamera(std::function<void(const HttpResponsePtr &)> &&callback,
                     std::vector<drogon_model::veda4::Camera> foundCameras);

    drogon_model::veda4::CameraFile setCameraFile(std::string transactionId, std::string uploadPath, std::string savedFilename,
                                                  int64_t foundCameraId);

    void sendSuccessResponse(std::function<void(const HttpResponsePtr &)> &&callback);

    std::string saveFile(std::string transactionId, std::string uploadPath, const HttpFile &file);

    bool checkTransactionEventReallyExist(std::function<void(const HttpResponsePtr &)> &&callback,
                                          std::string transactionId, std::shared_ptr<orm::Transaction> transaction);

    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    // list path definitions here;
    // PATH_ADD("/path", "filter1", "filter2", HttpMethod1, HttpMethod2...);
    PATH_ADD("/file",Post);
    PATH_LIST_END
};
