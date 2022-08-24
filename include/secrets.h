#include <pgmspace.h>

#define SECRET

const char WIFI_SSID [] = "Incognito";
const char WIFI_PASSWORD[] = "biscuits";

#define THINGNAME "ESP8266_sensor"

int8_t TIME_ZONE = -5;

//AWS ENDPOINT
const char MQTT_HOST[] = "a5iu2b8gpc01m-ats.iot.us-east-1.amazonaws.com";

static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

//from the certificate.pem.crt
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAIH9NToa8RXo/BiQ1rdUsQSWhYbkMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjA4MjMwODQ1
NTRaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCsiQCWB6I0Owvx9akX
E22c9fah9F17DU/47cABbHeVHRd9zWYaGiU5/unkytnpXc1SPkhlaZ2WJcbeCFTr
6tKGmvUw3eS6iLkDEVjA3uSInBTGYvZ610nHDwYz1VRXyS+tQykSh1YaDv+tIEdL
gpMMRcKVmihLe2Ib+yu1GAe4rLEyrkI6BIpxsvGOh0W62CCK6it5uBXyt8Who4nP
TzCZeJnwgQyeiz3k8b80/PWbKtRttKZ5ytfgr0YRidj6LJGsaYxBkqSNWi/MfFw4
U6cOUeL9Kzkw5GJUTTQoDbW9suHGKfl8FHK7Z3t5o5RWetObnie3dCs5ltVRf23M
TQeTAgMBAAGjYDBeMB8GA1UdIwQYMBaAFH1InpVvzNsdw8T+L+mkLE+WVyrqMB0G
A1UdDgQWBBRHeWetUcq9LVPS1XsLKcvTZO3I/TAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEATo2YUUs97L40FCc8KUDVEpVD
UxOmNEdSDo8eErN7JdkkwuysIPr0H+tEvG6ENeVwofHbxnPR3sKQCy+PJkKl9AZa
GEa9W+YPuEDreq5cHcmp/EcPpvLd7tKvKyQCfmaURVWCfogpR87BzEUXNc/QN01c
eRzTd0w61iWs+lveeq1Gg4lbEOOQLytSPkjvY2CEyX4gq7y6n1gbaN0Oob5otdBZ
u2CpBMuoKQ5yV6czmUXgomb/M66SadmUcqKvRCF11HxFj4qwD6/uNG8n5lrBMg5s
EgKvagkDk7moBUwA02taXXSkIIeHP4bEdYnQTY7mdU3TZG3Bvser1Qv0gD0RNg==
-----END CERTIFICATE-----
)KEY";

static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEArIkAlgeiNDsL8fWpFxNtnPX2ofRdew1P+O3AAWx3lR0Xfc1m
GholOf7p5MrZ6V3NUj5IZWmdliXG3ghU6+rShpr1MN3kuoi5AxFYwN7kiJwUxmL2
etdJxw8GM9VUV8kvrUMpEodWGg7/rSBHS4KTDEXClZooS3tiG/srtRgHuKyxMq5C
OgSKcbLxjodFutggiuorebgV8rfFoaOJz08wmXiZ8IEMnos95PG/NPz1myrUbbSm
ecrX4K9GEYnY+iyRrGmMQZKkjVovzHxcOFOnDlHi/Ss5MORiVE00KA21vbLhxin5
fBRyu2d7eaOUVnrTm54nt3QrOZbVUX9tzE0HkwIDAQABAoIBAH69jS6ooOCZV2cA
PJv065pXnDrQFTNlTg/gJiFlmlpxKmaBUbVu61W1Cf/Fe1nQTBlUIz5L3TUEvxcv
K6K5trimb+LbvT/BW5cDUR0tHtezQBeTbthB+bI+OSIuXWy3H0y5fxxbSkaOKgb1
JSo02uwy+oOG4H7CPgmr8ilvY4ymKiCBsdMPkDCBCFVDBQYeidExYPfqC+YruRXq
rUrTPpOMys4+4KIXpIbJtKYO40Ien9u9fbHqNj9AJ6rYmbmdWKvk+uk8URuaxank
0q9LYEq1tQ853ZUyp1zFPO06ILGFbnYbQDzJuzqKrsQabs0aj0ly0hjz+iYIS8l0
as07lZECgYEA1RRqC1VmBOMhgYYTgmpS6Kx3Vh4VPZ7GLsUliGp8oW9Bc0Z79Zz+
jVUgzBdBlzHkq9xEQ0cKYyaGDyez1N45lmiih/vi/UdAUE8UcnSGHdPD0rq90rp+
4xSy/C4o/ofdN/Yfhdi3s03nKtqC70ZipoKIq8tHAm1gwIMMH+GEG0cCgYEAz0nh
4+RTH0GrSZBc6AgmcAjuCpn+3J+VUn1O8ulNaN+61ZCwQkEX7Ci/zE89wpF3EeRV
UDJAnrULFznPshmX012R02RENvolI7uAlXAM4auufdGCKp+IPUKTu5hPUA+yNKWm
gshPJS8VbJaHJF5k6u+zpzr2K2SEAaHrcus6v1UCgYAdPE/QfkchJIjUXLEQk8YM
Zp15E2eWrNJEWIhtzy6Kd9ljF6iqaoACpEt7tTUqqY3WhsJW+MhUElUSf0ANVPBR
0sl6HvVmCwCoBDGqApLkRMn5f+XLpAsE3TIxp6O4SKvxobNxieqqj2XIW3Lboayn
bAGUkK+NhbRgtZpURXz6wQKBgQDNYaDFEaciIwi5vKWw6NJc0xw3uNkCc8OZlbqx
h2NBc9tEWzWoj6JKRR20KxdAPqzBN4EoEoJgPXUdsBQPnLgHnoAr+P51kmdV/iOq
7Ph2aIpc80aLQMHWIK8Lgh+jbcndy/stPZbdesc5bfO8nyJSfOl92jeqSs9YZrxI
grUUeQKBgQDRtUjGwlQ3zZHfuRSWmZCs5JuqSv5IMuxGN8Q7IixB4mGXYrLulWDi
oIce3ddciXgCncm4UmTxIup6OU5jgCUKP4GV1Ub6EXxyRdPmE1aLa5FJbAcVO/0U
Gfjovh1JHG8oIEVI9ewl98SWGeG1JuCijzzskMogzQaV3HeRPXUHzQ==
-----END RSA PRIVATE KEY-----

)KEY";