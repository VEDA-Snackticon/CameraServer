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

std::string cameraEventController::checkDescription(std::shared_ptr<Json::Value> values) {
    std::string description = (*values)["description"].asString();
    // description 자체가 없으면 예외반환
    if (description == "") { throw std::runtime_error("No description"); }
    return description;
}

trantor::Date cameraEventController::translateDate(std::shared_ptr<Json::Value> values) {
    std::string localtime_str =  (*values)["localtime"].asString();
    // 시간을 trantor::Date로 변환
    return string_to_trantor_date(localtime_str);
}

drogon_model::veda4::Camera cameraEventController::findCameraByDescription(
    std::string description, std::shared_ptr<drogon::orm::Transaction> transaction) {
    orm::Mapper<drogon_model::veda4::Camera> mapper(transaction);
    std::vector<drogon_model::veda4::Camera> cameras = mapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_description, orm::CompareOperator::EQ, description));
    if (cameras.size() == 0) { throw std::runtime_error("No cameras"); }
    return cameras[0];
}

void cameraEventController::checkSendingPolicy(drogon_model::veda4::Camera camera) {
    // 마스터 카메라가 아니면 처리하지 않음
    if (!(*camera.getIsMaster())) throw std::runtime_error("No camera is master");
    // 들어온 description 에 해당되는 카메라가 groupId설정이 되어있지 않아도 예외 반환
    if ((*camera.getGroupNumber()) ==0) { throw std::runtime_error("No camera group number");}
}

std::vector<drogon_model::veda4::Camera> cameraEventController::findCamerasByGroupNumber(
    drogon_model::veda4::Camera camera, std::shared_ptr<drogon::orm::Transaction> transaction) {
    orm::Mapper<drogon_model::veda4::Camera> mapper(transaction);
    // 해당하는 카메라의 groupid가 있다면 해당 groupid 카메라의 ip에 요청 전송
    std::vector<drogon_model::veda4::Camera> groupCameras = mapper.findBy(orm::Criteria(drogon_model::veda4::Camera::Cols::_group_number, orm::CompareOperator::EQ, (*camera.getGroupNumber())));
    if (groupCameras.size() == 0) { throw std::runtime_error("No cameras"); }
    return groupCameras;

}

std::string cameraEventController::saveCameraEvent(std::shared_ptr<orm::Transaction> db_client,
                                                   drogon_model::veda4::Camera camera, trantor::Date eventTime) {
    drogon_model::veda4::CameraEvent  camera_event;
    boost::uuids::random_generator gen;
    boost::uuids::uuid uuid = gen();  // 랜덤 UUID 생성
    std::string transaction_id = boost::uuids::to_string(boost::uuids::random_generator()());
    camera_event.setTransactionId(to_string(uuid));
    camera_event.setTime(eventTime);
    camera_event.setEventCameraId(camera.getValueOfId());
    orm::Mapper<drogon_model::veda4::CameraEvent> cameraEventMapper(db_client);
    cameraEventMapper.insert(camera_event);
    return transaction_id;
}

void cameraEventController::sendEventTo(std::shared_ptr<Json::Value> values, std::shared_ptr<drogon::HttpClient> client ,std::string transaction_id) {

    std::shared_ptr<drogon::HttpRequest> request = HttpRequest::newHttpRequest();
    request->addHeader("transactionid", transaction_id);
    request->addHeader("eventtime",(*values)["unixtime"].asString());
    std::cout << "unixtime : " << (*values)["unixtime"].asString() << std::endl;
    request->setPath("/event");
    request->setMethod(Post);
    std::cout << "prepare" << request->getHeader("transactionid") << " " << request->getHeader("eventtime") << ": end" << std::endl;

    client->sendRequest(
        request,
        [=](drogon::ReqResult result, const drogon::HttpResponsePtr &response) {
            if (result == drogon::ReqResult::Ok) {
                std::cout << "Response" << client->getHost() << "received: " << response->getBody() << std::endl;
            } else {
                std::cerr << "Request failed: " << drogon::to_string(result) << std::endl;
            }
        });
    std::cout << "success" << std::endl;
}

void cameraEventController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here

    std::shared_ptr<Json::Value> values = req->jsonObject();

    try {
        if (values == nullptr) {
            throw std::runtime_error("No data");
        }

        std::string localtime_str =  (*values)["localtime"].asString();
        trantor::Date trantor_date = string_to_trantor_date(localtime_str);
        // 시간을 trantor::Date로 변환
        std::cout << "initial Data : " << (*values)["unixtime"].asString() << std::endl;
        std::cout << "initial localTime" << trantor_date.toDbString() << std::endl;
        std::string description = checkDescription(values);
        trantor::Date eventTime = translateDate(values);
        std::vector<drogon_model::veda4::Camera> cameraSendList;
        std::string transaction_id ;
        {
            auto db_client = app().getDbClient()->newTransaction();
            // 들어온 description 에 해당하는 카메라가 없으면 예외 반환
            orm::Mapper<drogon_model::veda4::Camera> mapper(db_client);

            drogon_model::veda4::Camera eventCamera = findCameraByDescription(description, db_client);
            checkSendingPolicy(eventCamera);
            transaction_id = saveCameraEvent(db_client, eventCamera, eventTime);

            cameraSendList = findCamerasByGroupNumber(eventCamera, db_client);


        }

        for (auto camera : cameraSendList) {
            std::cout << "before SendEvent : " << (*values)["unixtime"].asString() << std::endl;
            sendEventTo(values,HttpClient::newHttpClient(camera.getValueOfIpAddr(),8000,false),transaction_id);
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
