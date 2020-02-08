#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "../lib/PubSubClient/PubSubClient.h"

typedef enum {
	NET_PENDING = -1,
	NET_SUCCESS = 0,
	NET_WIFI_TIMEOUT = 1,
	NET_WIFI_CONNECT_FAIL = 2,
	NET_WIFI_AUTH_FAIL = 3,
	NET_WIFI_UNAVAILABLE = 4,
	NET_MQTT_TIMEOUT = 5
} net_status_t;

class Network {
	
	public:
		Network(String mqttId);

		void setupWifi(String ssid, String password);
		void setupMqtt(String host, uint16_t port, String username, String password);

		void connect(unsigned int retries, unsigned long timeout);
		net_status_t status();
		net_status_t waitConnected();

		void submitBattery(int level);
		void submitWeight(String user, float weight);

	private:
		String _wifiNetwork;
		String _wifiPassword;

		String _mqttId;
		String _mqttUsername;
		String _mqttPassword;

		WiFiClient _netclient;
		PubSubClient _client;
		unsigned long _start;
		unsigned long _timeout;

		unsigned int _retryCount;
		unsigned int _retryLimit;
		net_status_t _wifiStatus();

		void _publish(const char* topic, String payload, bool retained);
};