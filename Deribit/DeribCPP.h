#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <exception>

#include <curl/curl.h>
#include <json/json.h>



#define BINANCE_HOST "https://api.binance.com"


using namespace std;

class DeribCPP {

    static CURL* curl;

	public:

		static void curl_api( string &url, string &result_json );
		static void curl_api_with_header( string &url, string &result_json , vector <string> &extra_http_header, string &post_data, string &action );
		static size_t curl_cb( void *content, size_t size, size_t nmemb, string *buffer ) ;

		static void init();
        static void cleanup();


		// Public API
		static void get_time( Json::Value &json_result);
		static void get_orderBook( const char *symbol, Json::Value &json_result );

		static void write_log( const char *fmt, ... );

};

