#include "cameraProgramController.h"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <drogon/HttpClient.h>

#include "Camera.h"
#include "CameraProcess.h"

void cameraProgramController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here

    try{
        MultiPartParser fileParser;
        fileParser.parse(req);
        std::string fps;
        std::string name;
        std::string description;
        auto transaction = app().getDbClient()->newTransaction();
        auto pairs = fileParser.getParameters();

        if(pairs.find("description") != pairs.end()) {
            description = pairs["description"];
        }
        if(pairs.find("name") != pairs.end()) {
            name = pairs["name"];
        }

        if(pairs.find("fps") != pairs.end()) {
            fps = pairs["fps"];
        }

        if (fps.empty()) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("fps is missing.");
            callback(response);
            return;
        }

        if (name.empty()) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("name is missing.");
            callback(response);
            return;
        }

        if (description.empty()) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("name is missing.");
            callback(response);
            return;
        }

        const auto& files = fileParser.getFiles();
        if (files.empty()) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("At least one MP4 file must be uploaded.");
            callback(response);
            return;
        }

        bool sofileFound = false;
        orm::Mapper<drogon_model::veda4::Camera> cameraMapper(transaction);
        orm::Mapper<drogon_model::veda4::CameraProcess> cameraProcessMapper(transaction);
        std::vector<drogon_model::veda4::Camera> foundCamera = cameraMapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description, orm::CompareOperator::EQ, description));
        if(foundCamera.size() != 1) throw std::runtime_error("No camera found.");
        drogon_model::veda4::Camera camera = foundCamera[0];

        auto client = HttpClient::newHttpClient("https://85736fea-88ea-40bc-bfdc-b04627023da8.mock.pstmn.io");
       // auto client = HttpClient::newHttpClient("http://snackticon.iptime.org");
        // auto client = HttpClient::newHttpClient(camera.getIpAddr(),8000,false);

        std::vector<UploadFile> uploadFiles;
        for (const auto& file : files) {
            // 파일 확장자 검사 (MP4만 허용)
            std::string fileName = file.getFileName();
            std::cout << fileName << std::endl;
            std::string fileExtension = std::filesystem::path(fileName).extension().string();  // 파일 확장자 추출

            // 확장자에서 점(.)을 제거 (예: ".mp4" -> "mp4")
            if (!fileExtension.empty() && fileExtension[0] == '.') {
                fileExtension = fileExtension.substr(1);  // 첫 번째 점을 제거
            }

            if (fileExtension == "so") {
                sofileFound = true;  // MP4 파일이 있음을 표시
            } else {
                // MP4 파일이 아니라면 무시
                continue;
            }

            std::string  filePath = "./tmp/" + file.getFileName();
            std::string savedFileName = file.getFileName();
            file.saveAs(filePath);

            uploadFiles.push_back(UploadFile(filePath,savedFileName));


            // 파일 정보 출력 (디버그용)
            LOG_INFO << "File received: " << file.getFileName();
            LOG_INFO << "File size: " << file.fileLength();
            LOG_INFO << "File extension: " << fileExtension;
            LOG_INFO << "MD5: " << file.getMd5();

        }

        // MP4 파일이 하나 이상 업로드 되었는지 확인
        if (!sofileFound) {
            auto response = HttpResponse::newHttpResponse();
            response->setStatusCode(k400BadRequest);
            response->setBody("At least one so file must be uploaded.");
            callback(response);
            return;
        }

        HttpRequestPtr request = HttpRequest::newFileUploadRequest(uploadFiles);
        std::cout <<"uploadFiles size: " << uploadFiles.size() << std::endl;
        request->setParameter("fps",fps);
        request->setParameter("name",name);
        request->setPath("/program");
        std::pair<drogon::ReqResult, HttpResponsePtr> re = client->sendRequest(request);
        std::cout << re.second->getBody() << std::endl;
        // 성공 응답
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k200OK);
        response->setBody("MP4 files uploaded successfully with UUID.");
        callback(response);


    }
    catch(const std::exception &e){
        // 에러 처리
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k500InternalServerError);
        response->setBody(std::string("Error: ") + e.what());
        callback(response);
    }
}
