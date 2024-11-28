#include "cameraEventController.h"

#include <drogon/HttpClient.h>

#include "Camera.h"

void cameraEventController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here





    //
    std::shared_ptr<Json::Value> values = req->jsonObject();

    try {
        if (values == nullptr) {
            throw std::runtime_error("No data");
        }

        std::string description = (*values)["description"].asString();
        // description 자체가 없으면 예외반환
        if (description == "") { throw std::runtime_error("No description"); }

        auto db_client = app().getDbClient();
        // 들어온 description 에 해당하는 카메라가 없으면 예외 반환
        orm::Mapper<drogon_model::veda4::Camera> mapper(db_client);

        std::__1::vector<drogon_model::veda4::Camera> cameras = mapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description, orm::CompareOperator::EQ, description));
        if (cameras.size() == 0) { throw std::runtime_error("No cameras"); }
        drogon_model::veda4::Camera camera = cameras[0];

        // 마스터 카메라가 아니면 처리하지 않음
        if (!(*camera.getIsMaster())) throw std::runtime_error("No camera is master");
        // 들어온 description 에 해당되는 카메라가 groupId설정이 되어있지 않아도 예외 반환
        if ((*camera.getGroupNumber()) == NULL) throw std::runtime_error("No camera group number");

        // 해당하는 카메라의 groupid가 있다면 해당 groupid 카메라의 ip에 요청 전송
        std::vector<drogon_model::veda4::Camera> groupCameras = mapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_group_number, orm::CompareOperator::EQ, (*camera.getGroupNumber())));
        if (cameras.size() == 0) { throw std::runtime_error("No cameras"); }

        for (int i =0; i<cameras.size(); i++) {
            drogon_model::veda4::Camera camera = cameras[i];

            std::shared_ptr<drogon::HttpClient> client = HttpClient::newHttpClient(*camera.getIpAddr(),5555,false);
            std:: cout << *camera.getIpAddr()+"/helloWorld" << std::endl;
            std::shared_ptr<drogon::HttpRequest> request = HttpRequest::newHttpRequest();
            request->setBody("testMessage");
            request->setPath("/helloWorld");
            std::cout << "전송 전 "<< std::endl;
            client->sendRequest(request,[](ReqResult,std::shared_ptr<drogon::HttpResponse> response) {
                std::cout << response->body()<< std::endl;;
            });

        }

        auto response = HttpResponse::newHttpResponse();
        response->setBody("success");
        callback(response);

    }
    catch (std::exception &e) {
        auto response = HttpResponse::newHttpResponse();
        response->setBody(e.what());
        response->setStatusCode(k404NotFound);
        callback(response);
    }




}
