#include "network.h"
#include "user_interface.h"

#define MQTT_TOPIC_BATTERY "smart-scale/tele/BATTERY"
#define MQTT_TOPIC_WEIGHT "smart-scale/tele/WEIGHT"

Network::Network(String mqttId):
	_mqttId(mqttId),
	_netclient(),
	_client(_netclient)
{}

void Network::setupWifi(String ssid, String password) {
	_wifiNetwork = ssid;
	_wifiPassword = password;
}

void Network::setupMqtt(String host, uint16_t port, String username, String password) {
	_client.setServer(host.c_str(), port);
	_mqttUsername = username;
	_mqttPassword = password;
}

void Network::connect(unsigned int retries, unsigned long timeout) {
	_retryCount = 0;
	_retryLimit = retries;
	_timeout = timeout;
	_start = millis();

	WiFi.begin(_wifiNetwork, _wifiPassword);

	// // WiFi fix: https://github.com/esp8266/Arduino/issues/2186
	// WiFi.persistent(false);
	// WiFi.mode(WIFI_OFF);
	// WiFi.mode(WIFI_STA);
	// WiFi.begin(_wifiNetwork, _wifiPassword);
}

net_status_t Network::_wifiStatus() {
	station_status_t status = wifi_station_get_connect_status();

	switch(status) {
		case STATION_GOT_IP:
			return NET_SUCCESS;
		case STATION_NO_AP_FOUND:
			return NET_WIFI_UNAVAILABLE;
		case STATION_CONNECT_FAIL:
			return NET_WIFI_CONNECT_FAIL;
		case STATION_WRONG_PASSWORD:
			return NET_WIFI_AUTH_FAIL;
		case STATION_IDLE:
		case STATION_CONNECTING:
		default:
			return NET_PENDING;
	}
}

net_status_t Network::status() {
	net_status_t status = _wifiStatus();

	if (status == NET_PENDING) {
		return status;
	}
	else if (status != NET_SUCCESS && _retryCount < _retryLimit) {
		// Retry connecting to wifi in case the network is flaky.
		WiFi.begin(_wifiNetwork, _wifiPassword);
		_retryCount++;
		return NET_PENDING;
	}

	if (_client.connected()) {
		return NET_SUCCESS;
	}
	else {
		bool success = _client.connect(_mqttId.c_str(), _mqttUsername.c_str(), _mqttPassword.c_str());
		return success ? NET_SUCCESS : NET_PENDING;
	}
}

net_status_t Network::waitConnected() {
	while (true) {
		if (millis() - _start > _timeout) {
			return NET_WIFI_TIMEOUT;
		}

		net_status_t result = status();

		if (result != NET_PENDING) {
			return result;
		}

		delay(500);
	}
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