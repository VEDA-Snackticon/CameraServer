#include "cameraMovieGetController.h"
#include <fstream>
#include <filesystem>
#include <drogon/drogon.h>

ContentType getContentTypeByExtension(const std::string &filename)  {
    // 파일 확장자를 기반으로 MIME 타입 코드 설정
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".mp4") {
        return CT_VIDEO_MP4;
    } else if (filename.size() >= 5 && filename.substr(filename.size() - 5) == ".webm") {
        return CT_VIDEO_WEBM;
    } else if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".png") {
        return CT_IMAGE_PNG;
    } else if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".txt") {
        return CT_TEXT_PLAIN;
    } else if (filename.size() >= 5 && filename.substr(filename.size() - 5) == ".html") {
        return CT_TEXT_HTML;
    } else {
        return CT_APPLICATION_OCTET_STREAM; // 기본값
    }
}

void cameraMovieGetController::asyncHandleHttpRequest(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    // URL 경로에서 파일명 추출
    auto filename = req->getParameter("filename");
    std::cout << "filename: " << filename << std::endl;
    std::cout << "Current directory: " << std::filesystem::current_path() << std::endl;
    if (filename.empty()) {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k400BadRequest);
        response->setBody("Filename is required");
        callback(response);
        return;
    }

    // 파일 경로 고정: ./upload
    std::string filePath = "./uploads/" + filename;

    // 파일 존재 여부 확인
    if (!std::filesystem::exists(filePath)) {
        auto response = HttpResponse::newHttpResponse();
        response->setStatusCode(k404NotFound);
        response->setBody("File not found");
        callback(response);
        return;
    }

    // MIME 타입 결정 (확장자 기반)
    ContentType contentType = getContentTypeByExtension(filename);

    // 파일 응답 생성
    auto response = HttpResponse::newFileResponse(filePath, filename, contentType);

    // Content-Disposition 헤더 설정: 브라우저에서 바로 열리도록
    response->addHeader("Content-Disposition", "inline; filename=\"" + filename + "\"");

    // 응답 전달
    callback(response);
}
