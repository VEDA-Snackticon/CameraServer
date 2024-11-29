#include "cameraEventController.h"

#include <CameraEvent.h>
#include <drogon/HttpClient.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "Camera.h"

// JSON에서 받은 시간 문자열을 std::tm으로 변환하는 함수
trantor::Date string_to_trantor_date(const std::string &time_str)
{
    // trantor::Date는 문자열 형식으로 시간을 파싱할 수 있음
    // 날짜 형식은 "YYYY-MM-DD HH:MM:SS"
    // time_str이 빈 문자열인 경우 처리
    if (time_str.empty())
    {
        throw std::invalid_argument("Received an empty time string");
    }

    return trantor::Date::fromDbString(time_str);
}

void cameraEventController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here

    std::shared_ptr<Json::Value> values = req->jsonObject();

    try {
        if (values == nullptr) {
            throw std::runtime_error("No data");
        }

        std::string description = (*values)["description"].asString();
        // description 자체가 없으면 예외반환
        if (description == "") { throw std::runtime_error("No description"); }


        std::string localtime_str =  (*values)["localtime"].asString();

        // 시간을 trantor::Date로 변환
        trantor::Date eventTime = string_to_trantor_date(localtime_str);


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



        drogon_model::veda4::CameraEvent  camera_event;
        boost::uuids::random_generator gen;
        boost::uuids::uuid uuid = gen();  // 랜덤 UUID 생성
        std::string transaction_id = boost::uuids::to_string(boost::uuids::random_generator()());
        camera_event.setTransactionId(to_string(uuid));
        camera_event.setTime(eventTime);
        camera_event.setEventCameraId(camera.getValueOfId());
        orm::Mapper<drogon_model::veda4::CameraEvent> cameraEventMapper(db_client);

        cameraEventMapper.insert(camera_event,[](const drogon_model::veda4::CameraEvent& event) {}, [](const orm::DrogonDbException){});



        for (int i =0; i<cameras.size(); i++) {
            drogon_model::veda4::Camera camera = cameras[i];

            std::shared_ptr<drogon::HttpClient> client = HttpClient::newHttpClient(*camera.getIpAddr(),8000,false);
            std:: cout << *camera.getIpAddr()+"/helloWorld" << std::endl;
            Json::Value requestJson;
            requestJson["transcationId"] = transaction_id;
            requestJson["localtime"] = (*values)["localtime"];
            requestJson["unixTime"] = (*values)["unixTime"];
            std::shared_ptr<drogon::HttpRequest> request = HttpRequest::newHttpJsonRequest(requestJson);
            request->setBody("testMessage");
            request->setPath("/event");
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
