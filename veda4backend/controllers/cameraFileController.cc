#include "cameraFileController.h"

#include <Camera.h>
#include <CameraEvent.h>
#include <CameraFile.h>
#include <filesystem>       // 디렉토리 생성용
#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>  // HttpRequest 헤더 추가
#include <drogon/HttpResponse.h> // HttpResponse 헤더 추가

std::string cameraFileController::getTransactionId(SafeStringMap<std::string> pairs) {
    if(pairs.find("transactionid") != pairs.end()){
        return pairs["transactionid"];
    }
    return "";
}

std::string cameraFileController::getDescription(SafeStringMap<std::string> pairs) {
    std::string description;
    if(pairs.find("description") != pairs.end()) {
        return pairs["description"];
    }
    return "";
}

bool cameraFileController::checkParameter(std::function<void(const HttpResponsePtr &)> &&callback, std::string transactionId, std::string description, const std::vector<HttpFile> files ) {
    if (transactionId.empty()) {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k400BadRequest);
        response->setBody("transactionId is missing.");
        callback(response);
        return true;
    }

    if (description.empty()) {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k400BadRequest);
        response->setBody("Description is missing.");
        callback(response);
        return true;
    }

    if (files.empty()) {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k400BadRequest);
        response->setBody("At least one MP4 file must be uploaded.");
        callback(response);
        return true;
    }
    return false;
}

std::string cameraFileController::setUploadPath() {
    std::string uploadPath = drogon::app().getCustomConfig()["upload_path"].asString();
    if (uploadPath.empty()) {
        uploadPath = "./uploads";  // 기본값 설정 (config에 없을 경우)
    }
    return uploadPath;
}

bool cameraFileController::checkSaveFiles(std::function<void(const HttpResponsePtr &)> &&callback, std::vector<std::string> savedFiles) {
    // MP4 파일이 하나 이상 업로드 되었는지 확인
    if (savedFiles.size() == 0) {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k400BadRequest);
        response->setBody("At least one MP4 file must be uploaded.");
        callback(response);
        return true;
    }
    return false;
}

bool cameraFileController::checkCameraEvent(std::function<void(const HttpResponsePtr &)> &&callback, std::vector<drogon_model::veda4::CameraEvent> camera_events, std::string transactionId) {
    if (camera_events.empty() ) {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k400BadRequest);
        response->setBody("No cameraEvents found.");
        callback(response);
        return true;
    }
    return false;
}

void cameraFileController::logFile(const HttpFile &file, std::string fileExtension) {
    // 파일 정보 출력 (디버그용)
    LOG_INFO << "File received: " << file.getFileName();
    LOG_INFO << "File size: " << file.fileLength();
    LOG_INFO << "File extension: " << fileExtension;
    LOG_INFO << "MD5: " << file.getMd5();
}

std::string cameraFileController::getFileExtension(std::string fileExtension) {
    // 확장자에서 점(.)을 제거 (예: ".mp4" -> "mp4")
    if (!fileExtension.empty() && fileExtension[0] == '.') {
        fileExtension = fileExtension.substr(1);  // 첫 번째 점을 제거
    }
    return fileExtension;
}

bool cameraFileController::checkCamera(std::function<void(const HttpResponsePtr &)> &&callback, std::vector<drogon_model::veda4::Camera> foundCameras) {
    if(foundCameras.empty()) {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k400BadRequest);
        response->setBody("cannot find event Camera.");
        callback(response);
        return true;
    }
    return false;
}

drogon_model::veda4::CameraFile cameraFileController::setCameraFile(std::string transactionId, std::string uploadPath, std::string savedFilename, int64_t foundCameraId) {
    drogon_model::veda4::CameraFile cameraFile;
    cameraFile.setCamId(foundCameraId);
    cameraFile.setTransactionId(transactionId);
    cameraFile.setPath(uploadPath);
    cameraFile.setFileName(savedFilename);
    return cameraFile;
}

void cameraFileController::sendSuccessResponse(std::function<void(const HttpResponsePtr &)> &&callback) {
    // 성공 응답
    auto response = HttpResponse::newHttpResponse();
    response->setStatusCode(k200OK);
    response->setBody("MP4 files uploaded successfully with UUID.");
    callback(response);
}

std::string cameraFileController::saveFile(std::string transactionId, std::string uploadPath,const HttpFile &file) {
    std::string fileName = file.getFileName();
    std::cout << "File name: " << fileName << std::endl;
    std::cout << "Transaction ID: " << transactionId << std::endl;
    std::string fileExtension = getFileExtension(std::filesystem::path(fileName).extension().string());
    logFile(file, fileExtension);
    std::string savePath = uploadPath + "/" + transactionId + "_" + file.getFileName();
    std::string savedFilename = transactionId + "_" + file.getFileName();
    // 파일 저장
    file.saveAs(savePath);
    LOG_INFO << "File saved to: " << savePath;
    return fileName;
}

bool cameraFileController::checkTransactionEventReallyExist(std::function<void(const HttpResponsePtr &)> &&callback, std::string transactionId,std::shared_ptr<orm::Transaction> transaction) {
    //실제 transcation이 있는 건지 확인한다.
    orm::Mapper<drogon_model::veda4::CameraEvent> cameraEventMapper(transaction);
    std::cout << "before query : " << transactionId << std::endl;
    std::vector<drogon_model::veda4::CameraEvent> camera_events = cameraEventMapper.findBy(orm::Criteria(drogon_model::veda4::CameraEvent::Cols::_transaction_id,orm::CompareOperator::EQ, transactionId));
    if (checkCameraEvent(std::move(callback), camera_events, transactionId)) return true;
    return false;
}

void cameraFileController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    try {
        // 요청을 파싱
        auto transaction = app().getDbClient()->newTransaction();
        MultiPartParser fileParser;
        fileParser.parse(req);
        auto pairs = fileParser.getParameters();
        std::string transactionId = getTransactionId(pairs);
        std::string description = getDescription(pairs);
        const auto& files = fileParser.getFiles();
        if (checkParameter(std::move(callback), transactionId, description,files )) return;

        // config.yaml에서 upload_path 가져오기
        std::string uploadPath = setUploadPath();
        // 디렉토리 존재 여부 확인 및 생성
        if (!std::filesystem::exists(uploadPath)) {
            std::filesystem::create_directories(uploadPath);
        }

        // 업로드된 파일 처리
        std::vector<std::string> savedFiles;
        for (const auto& file : files) {
            // 파일 확장자 검사 (MP4만 허용)
            std::string savedFilename = saveFile(transactionId, uploadPath, file);
            savedFiles.push_back(savedFilename);
        }

        if (checkSaveFiles(std::move(callback), savedFiles)) return;
        LOG_INFO << "checkSaveFiles Completed";
        if (checkTransactionEventReallyExist(std::move(callback), transactionId, transaction)) return;
        LOG_INFO << "checkTransactionEventReallyExist Completed";
        // transaction이 실제로 있다면 어떤 카메라가 보낸 요청인지 찾는다.
        orm::Mapper<drogon_model::veda4::Camera> cameraMapper(transaction);
        LOG_INFO << "cameraMapper Completed";
        std::vector<drogon_model::veda4::Camera> foundCameras = cameraMapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description,orm::CompareOperator::EQ,description));
        LOG_INFO << "foundCameras Completed";
        if (checkCamera(std::move(callback), foundCameras)) return;
        // 찾아낸 카메라 번호를 가져온다.
        int64_t foundCameraId = foundCameras[0].getValueOfId();
        LOG_INFO << "foundCameraId Completed";
        for (std::string fileName : savedFiles) {
            // 카메라 파일 이름과 함께 파일에 저장한다.
            drogon_model::veda4::CameraFile cameraFile = setCameraFile(transactionId, uploadPath, fileName, foundCameraId);
            std::cout << "cameraFile is Set FileName:" << cameraFile.getValueOfFileName() << std::endl;
            std::cout << "cameraFile is Set Transaction ID: " << cameraFile.getValueOfTransactionId() << std::endl;
            std::cout << "cameraFile is Set Path: " << cameraFile.getValueOfPath() << std::endl;
            std::cout << "cameraFile is Set CamID: " << cameraFile.getValueOfCamId() << std::endl;
            orm::Mapper<drogon_model::veda4::CameraFile> cameraFileMapper(transaction);
            cameraFileMapper.insert(cameraFile,[](const drogon_model::veda4::CameraFile camera_file) {

                LOG_INFO << "@@@@saved: " << camera_file.getValueOfFileName();
                LOG_INFO << " @@@@camera file saved Successfully Id :  " << camera_file.getValueOfId();

            },[](const orm::DrogonDbException & exception) {
                LOG_INFO << "@@@@Failed to save CameraFile :" << exception.base().what() ;
            });
        }
        sendSuccessResponse(std::move(callback));
    } catch (const std::exception& e) {
        // 에러 처리
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k500InternalServerError);
        response->setBody(std::string("Error: ") + e.what());
        callback(response);
    }

}
