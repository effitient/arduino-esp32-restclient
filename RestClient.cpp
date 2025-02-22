#include "RestClient.h"

// Uncomment to debug
// #define ESP32_RESTCLIENT_DEBUG

#ifdef ESP32_RESTCLIENT_DEBUG
#define DEBUG_PRINT(x) Serial.print(x);
#else
#define DEBUG_PRINT(x)
#endif

const char *contentType = "text/plain";

RestClient::RestClient(const char *_host, const int _port) {
    port = _port;
    host = _host;
    num_headers = 0;
}

int RestClient::begin(const char *ssid, const char *pass) {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, pass);
    DEBUG_PRINT("\n[Connecting] [");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        DEBUG_PRINT("-");
    }

    DEBUG_PRINT("]\n");
    DEBUG_PRINT("[Connected to: ");
    DEBUG_PRINT(ssid);
    DEBUG_PRINT("]\n");
    DEBUG_PRINT("[IP address: ");
    DEBUG_PRINT(WiFi.localIP());
    DEBUG_PRINT("]\n");
    return WiFi.status();
}

int RestClient::get(const char *path, const bool insecure) {
    return request("GET", path, NULL, insecure);
}

int RestClient::post(const char *path, const char *body, const bool insecure) {
    return request("POST", path, body, insecure);
}

void RestClient::setTimeout(const int seconds) {
    client_s.setTimeout(seconds);
}

void RestClient::setHeader(const char *header) {
    headers[num_headers] = header;
    num_headers++;
}

void RestClient::setContentType(const char *contentTypeValue) {
    contentType = contentTypeValue;
}

void RestClient::write(const char *string) {
    DEBUG_PRINT(string);
    client_s.print(string);
}

void RestClient::writeHeaders() {
    for (int i = 0; i < num_headers; i++) {
        write(headers[i]);
        write("\r\n");
    }
}

void RestClient::writeBody(const char *body) {
    if (body != NULL) {
        char contentLength[30];
        sprintf(contentLength, "Content-Length: %d\r\n", strlen(body));
        write(contentLength);

        write("Content-Type: ");
        write(contentType);
        write("\r\n");
    }

    write("\r\n");

    if (body != NULL) {
        write(body);
        write("\r\n");
        write("\r\n");
    }
}

int RestClient::request(const char *method, const char *path, const char *body, const bool insecure) {
    int statusCode = -1;
    if (insecure) {
        client_s.setInsecure();
    }
    if (!client_s.connect(host, port)) {
        DEBUG_PRINT("[Connection failed]\n");
        return 0;
    }
    DEBUG_PRINT("[Connected to: ");
    DEBUG_PRINT(host);
    DEBUG_PRINT("]\n");
    DEBUG_PRINT("[Request: \n");

    write(method);
    write(" ");
    write(path);
    write(" ");
    write("HTTP/1.1\r\n");
    writeHeaders();
    write("Host: ");
    write(host);
    write("\r\n");
    write("Connection: close\r\n");
    writeBody(body);

    DEBUG_PRINT("][End Request]\n");
    DEBUG_PRINT("[Reading Response Status]\n");
    statusCode = getResponseStatus();
    DEBUG_PRINT("[End Read Response Status]\n");
    DEBUG_PRINT("[Stoping client]\n");
    num_headers = 0;
    client_s.stop();
    DEBUG_PRINT("[Client stopped]\n");
    return statusCode;
}

int RestClient::getResponseStatus() {
    int code = 0;

    if (client_s.connected()) {
        String headerLine = client_s.readStringUntil('\n');
        int indexFirstBlank = headerLine.indexOf(' ');
        int indexSecondBlank = headerLine.indexOf(' ', indexFirstBlank + 1);
        String statusCode = headerLine.substring(indexFirstBlank + 1, indexSecondBlank);
        DEBUG_PRINT(statusCode);
        code = statusCode.toInt();
    }
    return code;
}
