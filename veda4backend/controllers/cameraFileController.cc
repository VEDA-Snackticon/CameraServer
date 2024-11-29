#include "cameraFileController.h"

#include <Camera.h>
#include <CameraEvent.h>
#include <CameraFile.h>
#include <filesystem>       // 디렉토리 생성용
#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>  // HttpRequest 헤더 추가
#include <drogon/HttpResponse.h> // HttpResponse 헤더 추가

void cameraFileController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{

    try {
        // 요청을 파싱
        MultiPartParser fileParser;
        fileParser.parse(req);
        std::string transactionId;
        std::string description;
        auto pairs = fileParser.getParameters();

        if(pairs.find("transactionId") != pairs.end()) {
            transactionId = pairs["transactionId"];
        }

        if(pairs.find("description") != pairs.end()) {
            description = pairs["description"];
        }

        if (transactionId.empty()) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("transactionId is missing.");
            callback(response);
            return;
        }

        if (description.empty()) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("Description is missing.");
            callback(response);
            return;
        }

        // 업로드된 파일 확인
        const auto& files = fileParser.getFiles();
        if (files.empty()) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("At least one MP4 file must be uploaded.");
            callback(response);
            return;
        }

        // config.yaml에서 upload_path 가져오기
        std::string uploadPath = drogon::app().getCustomConfig()["upload_path"].asString();
        if (uploadPath.empty()) {
            uploadPath = "./uploads";  // 기본값 설정 (config에 없을 경우)
        }

        // 디렉토리 존재 여부 확인 및 생성
        if (!std::filesystem::exists(uploadPath)) {
            std::filesystem::create_directories(uploadPath);
        }

        std::string savedFilename;
        // 업로드된 파일 처리
        bool mp4Found = false;
        for (const auto& file : files) {
            // 파일 확장자 검사 (MP4만 허용)
            std::string fileName = file.getFileName();
            std::string fileExtension = std::filesystem::path(fileName).extension().string();  // 파일 확장자 추출

            // 확장자에서 점(.)을 제거 (예: ".mp4" -> "mp4")
            if (!fileExtension.empty() && fileExtension[0] == '.') {
                fileExtension = fileExtension.substr(1);  // 첫 번째 점을 제거
            }

            if (fileExtension == "mp4") {
                mp4Found = true;  // MP4 파일이 있음을 표시
            } else {
                // MP4 파일이 아니라면 무시
                continue;
            }

            // 파일 정보 출력 (디버그용)
            LOG_INFO << "File received: " << file.getFileName();
            LOG_INFO << "File size: " << file.fileLength();
            LOG_INFO << "File extension: " << fileExtension;
            LOG_INFO << "MD5: " << file.getMd5();

            // 파일 저장 경로 설정
            std::string savePath = uploadPath + "/" + transactionId + "_" + file.getFileName();  // UUID를 파일 이름에 포함
            savedFilename = transactionId + "_" + file.getFileName();
            // 파일 저장
            file.saveAs(savePath);
            LOG_INFO << "File saved to: " << savePath;
        }

        // MP4 파일이 하나 이상 업로드 되었는지 확인
        if (!mp4Found) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("At least one MP4 file must be uploaded.");
            callback(response);
            return;
        }


        auto client = app().getDbClient();
        //실제 transcation이 있는 건지 확인한다.
        orm::Mapper<drogon_model::veda4::CameraEvent> cameraEventMapper(client);

        std::vector<drogon_model::veda4::CameraEvent> camera_events = cameraEventMapper.findBy(orm::Criteria(drogon_model::veda4::CameraEvent::Cols::_transaction_id,orm::CompareOperator::EQ, transactionId));

        if (camera_events.empty()) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("No cameraEvents found.");
            callback(response);
        }
        // transaction이 실제로 있다면 어떤 카메라가 보낸 요청인지 찾는다.
        orm::Mapper<drogon_model::veda4::Camera> cameraMapper(client);
        std::vector<drogon_model::veda4::Camera> foundCameras = cameraMapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description,orm::CompareOperator::EQ,description));
        if(foundCameras.empty()) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("cannot find event Camera.");
            callback(response);
        }

        // 찾아낸 카메라 번호를 가져온다.
        int64_t foundCameraId = foundCameras[0].getValueOfId();


        // 카메라 파일 이름과 함께 파일에 저장한다.

        drogon_model::veda4::CameraFile cameraFile;

        cameraFile.setCamId(foundCameraId);
        cameraFile.setTransactionId(transactionId);
        cameraFile.setPath(uploadPath);
        cameraFile.setFileName(savedFilename);

        orm::Mapper<drogon_model::veda4::CameraFile> cameraFileMApper(client);
        cameraFileMApper.insert(cameraFile,[](const drogon_model::veda4::CameraFile& cameraFile){ std::cout <<"success" << std::endl;}, [](const orm::DrogonDbException){std::cout << "fail" << std::endl;});


        // 성공 응답
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k200OK);
        response->setBody("MP4 files uploaded successfully with UUID.");
        callback(response);
    } catch (const std::exception& e) {
        // 에러 처리
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k500InternalServerError);
        response->setBody(std::string("Error: ") + e.what());
        callback(response);
    }
}
