#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

class RestClient {
public:
    RestClient(const char *host, const int port);
    int begin(const char *ssid, const char *pass);

    int get(const char *path, const bool insecure);
    int post(const char *path, const char *body, const bool insecure);

    void setTimeout(const int seconds);
    void setHeader(const char *header);
    void setContentType(const char *contentTypeValue);
    void write(const char *string);
    int request(const char *method, const char *path, const char *body, const bool insecure);
    int getResponseStatus();

private:
    int port;
    int num_headers;
    const char *host;
    const char *headers[10];
    const char *contentType;
    WiFiClient client;
    WiFiClientSecure client_s;

    void writeHeaders();
    void writeBody(const char *body);
};