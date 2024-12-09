//
// Created by lee on 2024. 12. 9..
//

#include <drogon/drogon_test.h>
#include <drogon/HttpClient.h>
DROGON_TEST(programTest) {
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1",5555,false);

    drogon::UploadFile upload_file("./tmp/test.so","test.so");
    std::vector<drogon::UploadFile> upload_files;
    upload_files.push_back(upload_file);
    drogon::HttpRequestPtr request = drogon::HttpRequest::newFileUploadRequest(upload_files);
    request->setMethod(drogon::Post);
    request->setPath("/program");
    request->setParameter("fps","5");
    request->setParameter("name","test.so");
    request->setParameter("description","test");

    std::pair<drogon::ReqResult, drogon::HttpResponsePtr> response = client->sendRequest(request);
    std::cout<< "response :: " << response.second->getBody() << std::endl;
}
