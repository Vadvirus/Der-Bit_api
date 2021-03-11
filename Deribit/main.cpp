
#include <json/json.h>

#include "curl/curl.h"
#include "Derib_websocket.h"
#include "DeribCPP.h"

using namespace std;

map < string, map <double,double> >  depthCache;
int lastUpdateId = INT_MAX;
Json::Value result;
FILE *fptr = fopen("output.txt", "w");


void print_depthCache() {

	map < string, map <double,double> >::iterator it_i;

	for ( it_i = depthCache.begin() ; it_i != depthCache.end() ; it_i++ ) {

		string bid_or_ask = (*it_i).first ;
		fprintf(fptr, "%s\n", bid_or_ask.c_str());
		fprintf(fptr, "Price                  Qty\n");

		map <double,double>::reverse_iterator it_j;

		for ( it_j = depthCache[bid_or_ask].rbegin() ; it_j != depthCache[bid_or_ask].rend() ; it_j++ ) {

			double price = (*it_j).first;
			double qty   = (*it_j).second;
			fprintf(fptr, "%.08f          %.08f\n", price, qty );
		}
	}
	fprintf(fptr, "\n");
}

//-------------
int ws_depth_onData( Json::Value &json_result ) {

    int i;
    int new_updateId  = json_result["u"].asInt();
	if ( new_updateId > lastUpdateId ) {
        fprintf(fptr, "%s\n", "_________Update_________");
		for ( i = 0 ; i < json_result["b"].size() ; i++ ) {
			double price = atof( json_result["b"][i][0].asString().c_str());
			double qty 	 = atof( json_result["b"][i][1].asString().c_str());
			if ( qty == 0.0 ) {
				depthCache["bids"].erase(price);
			} else {
				depthCache["bids"][price] = qty;
			}
		}
		for ( i = 0 ; i < json_result["a"].size() ; i++ ) {
			double price = atof( json_result["a"][i][0].asString().c_str());
			double qty 	 = atof( json_result["a"][i][1].asString().c_str());
			if ( qty == 0.0 ) {
				depthCache["asks"].erase(price);
			} else {
				depthCache["asks"][price] = qty;
			}
		}
		lastUpdateId = new_updateId;
	} else {
        // Get order book snapshot
        fprintf(fptr, "%s\n", "_________Snapshot_________");
        string symbol = "BNBBTC";
        DeribCPP::get_orderBook( symbol.c_str(),result);

        // Initialize the lastUpdateId
        lastUpdateId = result["lastUpdateId"].asInt();

        for ( int i = 0 ; i < result["asks"].size(); i++ ) {

            double price = atof( result["asks"][i][0].asString().c_str() );
            double qty   = atof( result["asks"][i][1].asString().c_str() );
            depthCache["asks"][price] = qty;
        }
        for  ( int i = 0 ; i < result["bids"].size() ; i++ ) {

            double price = atof( result["bids"][i][0].asString().c_str() );
            double qty   = atof( result["bids"][i][1].asString().c_str() );
            depthCache["bids"][price] = qty;
        }
	}
	print_depthCache();
}


int main()
{

    DeribCPP::init();

    // Get server time
    DeribCPP::get_time(result);
    fprintf(fptr, "timestamp = %llu\n", result); // timestamp

    DeribCPP_websocket::init();
 	DeribCPP_websocket::connect_endpoint( ws_depth_onData );
 	DeribCPP_websocket::enter_event_loop();

    fclose(fptr);
}
