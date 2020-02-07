#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "../lib/PubSubClient/PubSubClient.h"

typedef enum {
	NET_SUCCESS = 0,
	NET_TIMEOUT = 1,
	NET_CONNECT_FAIL = 2,
	NET_AUTH_FAIL = 3,
	NET_UNAVAILABLE = 4
} net_status_t;

class Network {
	
	public:
		Network();

		void connect(String ssid, String password);
		net_status_t waitConnected(unsigned long timeout);

		net_status_t connectMqtt(const char* host, uint16_t port, const char* username, const char* password, unsigned long timeout);

		void submitBattery(int level);
		void submitWeight(String user, float weight);

	private:
		WiFiClient _netclient;
		PubSubClient _client;

		void _publish(const char* topic, String payload, bool retained);
};