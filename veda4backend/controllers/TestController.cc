#include "TestController.h"

class Test {

    int id;
    std::string message;
public:

    Test(std::shared_ptr<Json::Value> jsonPtr) :id(stoi((*jsonPtr)["id"].asString())), message((*jsonPtr)["message"].asString()) {
    }
    virtual ~Test() = default;





};


void TestController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
    std::shared_ptr<Json::Value> values = req->getJsonObject();


    try {
        std::cout << "values" <<  (*values)["id"].asString()<< std::endl;
        Test t(values);
        Json::Value response;
        response["code"] = 200;
        response["message"] = "OK";
        response["hello"] = "nice to meetyou";

        auto resp=HttpResponse::newHttpJsonResponse(response);
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_TEXT_HTML);
        resp->setBody("An Error Occured");
        callback(resp);
    } catch (std::exception exception) {
        auto resp=HttpResponse::newHttpResponse();
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_TEXT_HTML);
        resp->setBody("An Error Occured");
        callback(resp);
    }




}
