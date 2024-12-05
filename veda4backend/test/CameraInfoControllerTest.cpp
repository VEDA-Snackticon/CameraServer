#include <drogon/drogon_test.h>
#include <drogon/HttpClient.h>
#include <controllers/TestController.h>

#include "controllers/cameraUpdateController.h"
//
// Created by lee on 2024. 12. 3
//


DROGON_TEST(DescriptionTest) {

    Json::Value json;
    json["description"]= "testDescription";
    auto request = HttpRequest::newHttpJsonRequest(json);
    request->setMethod(drogon::Post);
    request->setPath("/setup");
    std::shared_ptr<drogon::HttpClient> client = drogon::HttpClient::newHttpClient("127.0.0.1", 5555, false);
    std::pair<drogon::ReqResult, HttpResponsePtr> response = client->sendRequest(request);
    CHECK(response.first == drogon::ReqResult::Ok);
    CHECK(response.second->getBody() == "register success");
}






