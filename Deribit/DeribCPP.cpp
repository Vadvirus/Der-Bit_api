#include "DeribCPP.h"

CURL* DeribCPP::curl = NULL;


//---------------------------------
void
DeribCPP::init()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	DeribCPP::curl = curl_easy_init();
}


void
DeribCPP::cleanup()
{
	curl_easy_cleanup(DeribCPP::curl);
	curl_global_cleanup();
}

void
DeribCPP::get_time( Json::Value &json_result)
{
	DeribCPP::write_log( "<DeribCPP::get_serverTime>" ) ;

	string url(BINANCE_HOST);
	url += "/api/v1/time?";

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {

		try {
			Json::Reader reader;
			json_result.clear();
			reader.parse( str_result , json_result );

		} catch ( exception &e ) {
		 	DeribCPP::write_log( "<DeribCPP::get_serverTime> Error ! %s", e.what() );
		}
		DeribCPP::write_log( "<DeribCPP::get_serverTime> Done." ) ;

	} else {
		DeribCPP::write_log( "<DeribCPP::get_serverTime> Failed to get anything." ) ;
	}
}

//--------------------
// Get Market Depth
/*
GET /api/v1/depth

Name	Type		Mandatory	Description
symbol	STRING		YES
limit	INT		NO		Default 100; max 100.

*/

void
DeribCPP::get_orderBook(
	const char *instrument_name,
	Json::Value &json_result )
{

	DeribCPP::write_log( "<DeribCPP::get_depth>" ) ;

	string url(BINANCE_HOST);
	url += "/api/v1/depth?";

	string querystring("instrument_name=");
	querystring.append( instrument_name );

	url.append( querystring );
	DeribCPP::write_log( "<DeribCPP::get_depth> url = |%s|" , url.c_str() ) ;

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {

		try {
			Json::Reader reader;
			json_result.clear();
	    		reader.parse( str_result , json_result );

		} catch ( exception &e ) {
		 	DeribCPP::write_log( "<DeribCPP::get_depth> Error ! %s", e.what() );
		}
		DeribCPP::write_log( "<DeribCPP::get_depth> Done." ) ;

	} else {
		DeribCPP::write_log( "<DeribCPP::get_depth> Failed to get anything." ) ;
	}
}

size_t
DeribCPP::curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer )
{
	DeribCPP::write_log( "<DeribCPP::curl_cb> " ) ;

	buffer->append((char*)content, size*nmemb);

	DeribCPP::write_log( "<DeribCPP::curl_cb> done" ) ;
	return size*nmemb;
}

void
DeribCPP::curl_api( string &url, string &result_json ) {
	vector <string> v;
	string action = "GET";
	string post_data = "";
	curl_api_with_header( url , result_json , v, post_data , action );
}

void
DeribCPP::curl_api_with_header( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action )
{

	DeribCPP::write_log( "<DeribCPP::curl_api>" ) ;

	CURLcode res;

	if( DeribCPP::curl ) {

		curl_easy_setopt(DeribCPP::curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(DeribCPP::curl, CURLOPT_WRITEFUNCTION, DeribCPP::curl_cb);
		curl_easy_setopt(DeribCPP::curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(DeribCPP::curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(DeribCPP::curl, CURLOPT_ENCODING, "gzip");

		if ( extra_http_header.size() > 0 ) {

			struct curl_slist *chunk = NULL;
			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
			}
			curl_easy_setopt(DeribCPP::curl, CURLOPT_HTTPHEADER, chunk);
		}

		if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

			if ( action == "PUT" || action == "DELETE" ) {
				curl_easy_setopt(DeribCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
			}
			curl_easy_setopt(DeribCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );
 		}

		res = curl_easy_perform(DeribCPP::curl);

		/* Check for errors */
		if ( res != CURLE_OK ) {
			DeribCPP::write_log( "<DeribCPP::curl_api> curl_easy_perform() failed: %s" , curl_easy_strerror(res) ) ;
		}

	}

	DeribCPP::write_log( "<DeribCPP::curl_api> done" ) ;

}

//-----------------------------------------------
void
DeribCPP::write_log( const char *fmt, ... )
{
   printf(fmt);
   printf("\n");
}
