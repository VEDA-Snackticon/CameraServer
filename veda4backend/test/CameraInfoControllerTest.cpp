#include <drogon/drogon_test.h>
#include <drogon/HttpClient.h>
#include <controllers/TestController.h>

#include "controllers/cameraUpdateController.h"
//
// Created by lee on 2024. 12. 3
//


DROGON_TEST(Description을_보내면_응답한다) {

    Json::Value json;
    json["description"]= "testDescription";
    auto request = HttpRequest::newHttpJsonRequest(json);
    request->setMethod(drogon::Post);
    request->setPath("/setup");
    std::shared_ptr<drogon::HttpClient> client = drogon::HttpClient::newHttpClient("127.0.0.1", 5555, false);
    client->sendRequest(request,[TEST_CTX](const drogon::ReqResult& result, const drogon::HttpResponsePtr& resp) {
        REQUIRE(result == drogon::ReqResult::Ok);
        CHECK(resp->getBody() == "register success");
    });
}






