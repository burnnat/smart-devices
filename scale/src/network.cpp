#include "network.h"
#include "user_interface.h"

#define MQTT_TOPIC_BATTERY "smart-scale/tele/BATTERY"
#define MQTT_TOPIC_WEIGHT "smart-scale/tele/WEIGHT"

Network::Network():
	_netclient(),
	_client(_netclient)
{}

void Network::connect(String ssid, String password) {
	WiFi.begin(ssid, password);

	// WiFi fix: https://github.com/esp8266/Arduino/issues/2186
	WiFi.persistent(false);
	WiFi.mode(WIFI_OFF);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
}

net_status_t Network::waitConnected(unsigned long timeout) {
	unsigned long start = millis();

	while (true) {
		station_status_t status = wifi_station_get_connect_status();

		switch(status) {
			case STATION_GOT_IP:
				return NET_SUCCESS;
			case STATION_NO_AP_FOUND:
				return NET_UNAVAILABLE;
			case STATION_CONNECT_FAIL:
				return NET_CONNECT_FAIL;
			case STATION_WRONG_PASSWORD:
				return NET_AUTH_FAIL;
			default:
				break;
		}

		delay(500);

		if (millis() - start > timeout) {
			// connection timeout
			return NET_TIMEOUT;
		}
	}

	return NET_SUCCESS;
}

net_status_t Network::connectMqtt(const char* host, uint16_t port, const char* username, const char* password, unsigned long timeout) {
	unsigned long start = millis();

	_client.setServer(host, port);

	while (!_client.connected()) {
		if (_client.connect("SmartScale", username, password)) {
			return NET_SUCCESS;
		}

		delay(500);

		if (millis() - start > timeout) {
			// connection timeout
			return NET_TIMEOUT;
		}
	}

	return NET_SUCCESS;
}

void Network::_publish(const char* topic, String payload, bool retained) {
	char formatted[payload.length() + 1];
	payload.toCharArray(formatted, payload.length());

	_client.publish(topic, formatted, retained);
}

void Network::submitBattery(int level) {
	_publish(MQTT_TOPIC_BATTERY, String(level), true);
}

void Network::submitWeight(String user, float value) {
	_publish(MQTT_TOPIC_WEIGHT, String(value, 1), false);
}