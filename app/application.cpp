#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "AppSettings.h"
#include "app_task.h"

DS18S20 TempSensor;
HttpServer server;

void onLedStatus(HttpRequest &request, HttpResponse &response)
{
	String responseString;

    if( digitalRead(GPIO_BUTTON_LED) == 0x01 )
    {
    	responseString = "{\"led\": 1}";
    }
    else
    {
    	responseString = "{\"led\": 0}";
    }

	response.sendString(responseString); // will be automatically deleted
}

void onLedOff(HttpRequest &request, HttpResponse &response)
{
	String responseString;

    digitalWrite(GPIO_BUTTON_LED, LOW);
    responseString = "{\"led\": 0}";

	response.sendString(responseString); // will be automatically deleted
}

void onLedOn(HttpRequest &request, HttpResponse &response)
{
	String responseString;

    digitalWrite(GPIO_BUTTON_LED, HIGH);
    responseString = "{\"led\": 1}";

	response.sendString(responseString); // will be automatically deleted
}

void onAdc(HttpRequest &request, HttpResponse &response)
{
	String adcString;
	String responseString;


	adcString = String(system_adc_read(), 10);
	responseString = "{\"adc\": ";
	responseString.concat(adcString);
	responseString.concat("\"}");

	response.sendString(responseString); // will be automatically deleted

}

void onRelayStatus(HttpRequest &request, HttpResponse &response)
{
	String responseString;

    if( digitalRead(GPIO_RELAY_CTRL) == 0x01 )
    {
    	responseString = "{\"relay\": 1}";
    }
    else
    {
    	responseString = "{\"relay\": 0}";
    }

	response.sendString(responseString); // will be automatically deleted
}

void onTmp(HttpRequest &request, HttpResponse &response)
{
	float tempDataFloat;
	int16_t tempDataInt;
	String responseString;
	String tempSensorString;

	tempDataFloat = TempSensor.GetCelsius(0);
	tempDataInt = (int16_t)(tempDataFloat);

	/* Rounding up */
	if( (tempDataFloat - (float)(tempDataInt)) > 0.5 )
	{
		tempDataInt = tempDataInt + 1;
	}

	tempSensorString = String(tempDataInt, 10);
	responseString = "{\"tmp\": ";
	responseString.concat(tempSensorString);
	responseString.concat("\"}");

	response.sendString(responseString); // will be automatically deleted

}

void onTmpF(HttpRequest &request, HttpResponse &response)
{
	String responseString;
	String tempSensorString;

	tempSensorString = String(TempSensor.GetFahrenheit(0), 4);
	responseString = "{\"tmp_f\": ";
	responseString.concat(tempSensorString);
	responseString.concat("\"}");

	response.sendString(responseString); // will be automatically deleted

}

void onTmpC(HttpRequest &request, HttpResponse &response)
{
	String responseString;
	String tempSensorString;

	tempSensorString = String(TempSensor.GetCelsius(0), 4);
	responseString = "{\"tmp_c\": ";
	responseString.concat(tempSensorString);
	responseString.concat("\"}");

	response.sendString(responseString); // will be automatically deleted

}

void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.getPath();
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
		response.forbidden();
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
}


void startAppServer()
{
	server.listen(80);
	server.addPath("/led", onLedStatus);
	server.addPath("/led/0", onLedOff);
	server.addPath("/led/1", onLedOn);
	server.addPath("/adc", onAdc);
	server.addPath("/relay", onRelayStatus);
	server.addPath("/tmp", onTmp);
	server.addPath("/tmp_c", onTmpC);
	server.addPath("/tmp_f", onTmpF);
	server.setDefaultHandler(onFile);
}

void init()
{
	softap_config config = {0};
	wifi_softap_get_config(&config);

	spiffs_mount(); // Mount file system, in order to work with files

	AppTask_Init();

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	//WifiStation.enable(true);

	if( WifiStation.isEnabled() == false )
	{
		// Start AP by default
		WifiAccessPoint.enable(true);
		WifiAccessPoint.config("FlipFlop_"+WifiStation.getMAC().substring(6), "", AUTH_OPEN);
		/* Optional */
		WifiAccessPoint.setIP(IPAddress(192, 168, 4, 1));

	}


	// Run WEB server on system ready
	System.onReady(startAppServer);
}
