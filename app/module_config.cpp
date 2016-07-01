#include <user_config.h>
#include <SmingCore/SmingCore.h>
//#include <AppSettings.h>
#include "app_task.h"
#include "module_config.h"

BssList networks;
String network, password, mode;
//Timer connectionTimer;
//Timer scanSsidTimer;

static String ApMode_SSID;

void onHome(HttpRequest &request, HttpResponse &response)
{

	//char section_data[500];

	Serial.println("On Home Web Page...");

	TemplateFileStream *tmpl = new TemplateFileStream("index_basic_frame.html");
	//TemplateFileStream *section = new TemplateFileStream("home_section.html");
	auto &vars = tmpl->variables();

#if(0)
	if( fileGetContent("section_home.html", section_data, sizeof(section_data) ) <= 0)
		vars["section_var"] = "<h2>Error section_home.html is too big...</h2>";
	else
		vars["section_var"] = section_data;
#endif

	response.sendTemplate(tmpl); // will be automatically deleted

}

void onSetup(HttpRequest &request, HttpResponse &response)
{
//	char section_data[2000];
//	char section_wifi_table[1000];
	String str;
//	String wifi_scan;

	TemplateFileStream *tmpl = new TemplateFileStream("index_frame.html");
	//TemplateFileStream *section = new TemplateFileStream("home_section.html");
	auto &vars = tmpl->variables();
#if(0)
	if( fileGetContent("wifi_scan.html", section_wifi_table, sizeof(section_wifi_table) ) <= 0)
		vars["section_var"] = "<h2>Error wifi_scan.html is too big...</h2>";
	else
#endif

	//if( fileGetContent("section_setup.html", section_data, sizeof(section_data) ) <= 0)
	str = fileGetContent("section_setup.html");
	if( str == "")
		vars["section_var"] = "<h2>Error section_setup.html is too big...</h2>";
	else
	{

		//str = section_data;
		//wifi_scan = section_wifi_table;
		str.replace("{ip_address_ap_var}", WifiAccessPoint.getIP().toString());

		if( mode == "ap_mode" )
		{
			str.replace("{ap_mode_select}", "checked=\"checked\"");
			str.replace("{station_mode_select}", "");
		}
		else
		{
			str.replace("{ap_mode_select}", "");
			str.replace("{station_mode_select}", "checked=\"checked\"");
		}

		//str.replace("{wifi_scan_table}", wifi_scan);


		vars["section_var"] = str;
	}


	response.sendTemplate(tmpl); // will be automatically deleted
}

void onInfo(HttpRequest &request, HttpResponse &response)
{
	char section_data[500];
	String str;
	String flashId;

	TemplateFileStream *tmpl = new TemplateFileStream("index_frame.html");
	//TemplateFileStream *section = new TemplateFileStream("home_section.html");
	auto &vars = tmpl->variables();


	if( fileGetContent("section_info.html", section_data, sizeof(section_data) ) <= 0)
		vars["section_var"] = "<h2>Error section_info.html is too big...</h2>";
	else
	{
		str = section_data;

		if( mode == "ap_mode")
		{
			str.replace("{module_mode_var}", "AP Mode");
			str.replace("{mac_address_var}", WifiAccessPoint.getMAC());
			str.replace("{ip_address_var}", WifiAccessPoint.getIP().toString());
			str.replace("{ssid_var}", ApMode_SSID);
			str.replace("{connection_status_var}", "Connected");
		}
		else
		{
			str.replace("{module_mode_var}", "Station Mode");
			str.replace("{mac_address_var}", WifiStation.getMAC());
			str.replace("{ip_address_var}", WifiStation.getIP().toString());
			str.replace("{ssid_var}", WifiStation.getSSID());

			if( WifiStation.getConnectionStatus() == eSCS_GotIP)
			{
				str.replace("{connection_status_var}", "Connected");
			}
			else
			{
				str.replace("{connection_status_var}", "Not Connected");
			}
		}

		str.replace("{sdk_version_var}", system_get_sdk_version());
		flashId = String(spi_flash_get_id(), 16);
		str.replace("{flash_type_var}", flashId);
		vars["section_var"] = str;
	}

	response.sendTemplate(tmpl); // will be automatically deleted
}

void onWifiInfo(HttpRequest &request, HttpResponse &response)
{
	String str;
	String tempTableRows;
	String wifiTableRowTemplate;
	uint32_t heapSizeInt;
	String heapSizeString;
	int i;
	int ssidCount;

	if (request.getRequestMethod() == RequestMethod::POST)
	{
		mode = request.getPostParameter("mode");
		network = request.getPostParameter("ssid");
		password = request.getPostParameter("pwd");

		if( mode == "ap_mode")
		{
			WifiAccessPoint.enable(true);
			//WifiAccessPoint.config(ApMode_SSID, "", AUTH_OPEN);
			// Optional: Change IP addresses (and disable DHCP)
			//WifiAccessPoint.setIP(IPAddress(192, 168, 4, 1));
		}
		else
		{
			WifiStation.enable(true);
			//WifiStation.config(network, password);
		}

		//ConfigStationModeCheckStatusTmrCntr = 0;
		//connectionTimer.initializeMs(1000, makeConnection).start();

		//StationModeConnectionResponse = &response;
	}



	TemplateFileStream *tmpl = new TemplateFileStream("index_frame.html");
	//TemplateFileStream *section = new TemplateFileStream("home_section.html");
	auto &vars = tmpl->variables();

	if( mode == "ap_mode" )
	{
		str = fileGetContent("section_station_status.html");

		if( str == "" )
			str = "<h2>Error section_station_status.html is too big...</h2>";
		else
		{
			str.replace("{ssid_var}", ApMode_SSID);
			str.replace("{ip_address_var}", WifiAccessPoint.getIP().toString());
			if( WifiAccessPoint.isEnabled() == true )
				str.replace("{connection_status_var}", "<b><font color=green> WiFi AP Connection - Success </font></b>");
			else
				str.replace("{connection_status_var}", "<b><font color=orange> WiFi AP Connection - Failed... </font></b>");
		}
	}
	else /* Need to connect to the WiFi network so give the list of Available Network */
	{

		  tempTableRows = "";
		  ssidCount = networks.count();
		  /* Configure each row for the scan wifi table */
		  for( i=0; i <= ssidCount; i++)
		  //for( i=0; i <= 3; i++)
		  {
			  Serial.println(String(i, 10));

			  wifiTableRowTemplate = "";
			  /* Read the template file for the rows */
			  wifiTableRowTemplate = fileGetContent("wifi_scan_table_row.html");

			  if( wifiTableRowTemplate == "" )
			  {
					str = "<h2>Error wifi_scan_table_row.html is too big...</h2>";
					break;
			  }
			  else
			  { /* Fill the table with the right variable */

				//if (networks[i].hidden && (i < networks.count()) ) {continue;}

				wifiTableRowTemplate.replace("{wifi_select_id_var}", String(i, 10) );
				//wifiTableRowTemplate.replace("{ssid_id_var}", String(i, 10) );
				wifiTableRowTemplate.replace("{pwd_id_var}", String(i, 10) );
				if( i == ssidCount )
				//if( i >= 3 )
				{
					wifiTableRowTemplate.replace("{scan_ssid_name_var}", "<input type=\"text\" name=\"custom_ssid\" value=\"\">" );
					wifiTableRowTemplate.replace("{signal_quality_var}", "---" );
				}
				else
				{
					wifiTableRowTemplate.replace("{scan_ssid_name_var}", networks[i].ssid );
					wifiTableRowTemplate.replace("{signal_quality_var}", String(networks[i].rssi, 10) );
					//wifiTableRowTemplate.replace("{scan_ssid_name_var}", String(i, 10) );
					//wifiTableRowTemplate.replace("{signal_quality_var}", String(i, 10) );
				}
			  }
			  /* Add a custom field also */

			  /* append the result to the temp String variable */
			  //tempTableRows.concat(wifiTableRowTemplate);
			  tempTableRows = tempTableRows + wifiTableRowTemplate;

			  heapSizeInt = system_get_free_heap_size();
			  heapSizeString = String(heapSizeInt, 10);
			  Serial.println("Heap Free Size: " + heapSizeString);

		  }

		  Serial.println(String(i, 10));

		  str = fileGetContent("wifi_scan.html");

		if( str == "" )
		{
			str = "<h2>Error wifi_scan.html is too big...</h2>";
		}
		else
		{
			str.replace("{wifi_scan_table_rows_var}", tempTableRows);
		}
		i++;
		Serial.println(String(i, 10));

	}

	  heapSizeInt = system_get_free_heap_size();
	  heapSizeString = String(heapSizeInt, 10);
	  Serial.println("Heap Free Size: " + heapSizeString);

	vars["section_var"] = str;
	i++;
	Serial.println(String(i, 10));

	  heapSizeInt = system_get_free_heap_size();
	  heapSizeString = String(heapSizeInt, 10);
	  Serial.println("Heap Free Size (Last): " + heapSizeString);

	response.sendTemplate(tmpl); // will be automatically deleted


}

void onWifiStationConnect(HttpRequest &request, HttpResponse &response)
{
	char section_data[1000];
	String str;
	String flashId;
	EStationConnectionStatus connectionStatus;
	String wifi_table_id;
	int scanTableIndex;

	if (request.getRequestMethod() == RequestMethod::POST)
	{
		if( request.getPostParameter("submit") == "WiFi_Connect")
		{

			mode = request.getPostParameter("mode");
			network = request.getPostParameter("custom_ssid");
			password = request.getPostParameter("custom_pwd");

			if( mode == "ap_mode") // AP Mode
			{
				WifiAccessPoint.enable(true);
				WifiAccessPoint.config(ApMode_SSID, "", AUTH_OPEN);
				// Optional: Change IP addresses (and disable DHCP)
				WifiAccessPoint.setIP(IPAddress(192, 168, 4, 1));
			}
			else // Station Mode
			{
				WifiStation.enable(true);
				WifiStation.config(network, password);
			}

			Serial.println(network);
			Serial.println(password);
			//WifiStation.config(network, password);
		}
	}



	TemplateFileStream *tmpl = new TemplateFileStream("index_frame.html");
	//TemplateFileStream *section = new TemplateFileStream("home_section.html");
	auto &vars = tmpl->variables();


	if( fileGetContent("section_station_status.html", section_data, sizeof(section_data) ) <= 0)
		vars["section_var"] = "<h2>Error section_station_status.html is too big...</h2>";
	else
	{
		str = section_data;

		if( mode == "ap_mode" )
		{

			str.replace("{ssid_var}", ApMode_SSID);
			str.replace("{ip_address_var}", WifiAccessPoint.getIP().toString());
			if( WifiAccessPoint.isEnabled() == true )
				str.replace("{connection_status_var}", "<b><font color=green> WiFi AP Connection - Success </font></b>");
			else
				str.replace("{connection_status_var}", "<b><font color=orange> WiFi Station Connecting... </font></b>");
		}
		else
		{
			connectionStatus = WifiStation.getConnectionStatus();


			str.replace("{ssid_var}", WifiStation.getSSID());
			str.replace("{ip_address_var}", WifiStation.getIP().toString());

			switch (connectionStatus)
			{
			case eSCS_Idle:
				str.replace("{connection_status_var}", "<b><font color=orange> WiFi Station Connection - Idle </font></b>");
				break;
			case eSCS_Connecting:
				str.replace("{connection_status_var}", "<b><font color=orange> WiFi Station Connecting... </font></b>");
				break;
			case eSCS_WrongPassword:
				str.replace("{connection_status_var}", "<b><font color=red> WiFi Station - Wrong Password </font></b>");
				break;
			case eSCS_AccessPointNotFound:
				str.replace("{connection_status_var}", "<b><font color=red> WiFi Station - Wrong SSID </font></b>");
				break;
			case eSCS_ConnectionFailed:
				str.replace("{connection_status_var}", "<b><font color=red> WiFi Station Connection - Failed </font></b>");
				break;
			case eSCS_GotIP:
				str.replace("{connection_status_var}", "<b><font color=green> WiFi Station Connection - Success </font></b>");
				break;
			default:
				break;
			}
		}


	}
	vars["section_var"] = str;

	response.sendTemplate(tmpl); // will be automatically deleted


}


void onRestart(HttpRequest &request, HttpResponse &response)
{
	char section_data[500];
	bool ifRestart;

	ifRestart = false;

	if (request.getRequestMethod() == RequestMethod::POST)
	{
		if( request.getPostParameter("apply") == "Apply Changes")
		{
			if( mode == "ap_mode" )
			{
				WifiAccessPoint.enable(true);
				WifiStation.enable(false);
				/* Wait for changes to occur */
				while( WifiAccessPoint.isEnabled() == false){};
				while( WifiStation.isEnabled() == true){};

			}
			else
			{
				WifiAccessPoint.enable(false);
				WifiStation.enable(true);
				/* Wait for changes to occur */
				while( WifiAccessPoint.isEnabled() == true){};
				while( WifiStation.isEnabled() == false){};

			}

			ifRestart = true;
		}

		if(ifRestart == true)
		{
			System.restart();
		}

	}
	else
	{

	TemplateFileStream *tmpl = new TemplateFileStream("index_frame.html");
	//TemplateFileStream *section = new TemplateFileStream("home_section.html");
	auto &vars = tmpl->variables();


	if( fileGetContent("section_restart.html", section_data, sizeof(section_data) ) <= 0)
		vars["section_var"] = "<h2>Error section_restart.html is too big...</h2>";
	else
		vars["section_var"] = section_data;

	response.sendTemplate(tmpl); // will be automatically deleted
	}
}

void onTest(HttpRequest &request, HttpResponse &response)
{

	TemplateFileStream *tmpl = new TemplateFileStream("test.html");
	//TemplateFileStream *section = new TemplateFileStream("home_section.html");

	response.sendTemplate(tmpl); // will be automatically deleted
}

void startWebServer()
{
	Serial.println("Start Server Config");

	server.addPath("/", onHome);
	server.addPath("/module_setup", onSetup);
	server.addPath("/module_info", onInfo);
	server.addPath("/wifi_mode", onWifiInfo);
	server.addPath("/wifi_connect", onWifiStationConnect);
	server.addPath("/restart", onRestart);
	server.addPath("/test", onTest);

}

void networkScanCompleted(bool succeeded, BssList list)
{
	if (succeeded)
	{
		for (int i = 0; i < list.count(); i++)
			if (!list[i].hidden && list[i].ssid.length() > 0)
				networks.add(list[i]);
	}
	networks.sort([](const BssInfo& a, const BssInfo& b){ return b.rssi - a.rssi; } );
}

void ModuleConfig_init()
{

	//AppSettings.load();


	/* Which mode are we in right now */
	if( WifiStation.isEnabled() == true )
	{
		mode = "station_mode";
	}
	else
	{
		mode = "ap_mode";
	}

#if(0)
	if (AppSettings.exist())
	{
		WifiStation.config(AppSettings.ssid, AppSettings.password);
		if (!AppSettings.dhcp && !AppSettings.ip.isNull())
			WifiStation.setIP(AppSettings.ip, AppSettings.netmask, AppSettings.gateway);
	}
#endif

	/* Enable the Station mode to allow the scan of SSID network */
	if( mode != "station_mode" )
	{
		WifiStation.enable(true);
	}

	/* Disconnect to stop trying to auto connect to the WiFi network */
	//WifiStation.disconnect();

	/* Force a first WiFi network scan */
	WifiStation.startScan(networkScanCompleted);

	/* Start a periodic scan of WiFi networks */
	//scanSsidTimer.initializeMs(10000, networkScanStart).start();

	/* Get module AP Mode SSID */
	ApMode_SSID = "FlipFlop_"+WifiStation.getMAC().substring(6);

	if( WifiAccessPoint.isEnabled() == false )
	{
		//WifiStation.disconnect();
		// Start AP for configuration



		WifiAccessPoint.enable(true);

		if(WifiAccessPoint.isEnabled() == true)
			Serial.println(ApMode_SSID);

		//WifiAccessPoint.config(ApMode_SSID, "", AUTH_OPEN, false, 6, 100);
		/* Wait for AP to be enable */

	}


	/* Add required listening path to server */
	startWebServer();

}
