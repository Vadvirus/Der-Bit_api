#include "Derib_websocket.h"
#include "DeribCPP.h"

struct lws_context *DeribCPP_websocket::context = NULL;
struct lws_protocols DeribCPP_websocket::protocols[] =
{
	{
		"example-protocol",
		DeribCPP_websocket::event_cb,
		0,
		65536,
	},
	{ NULL, NULL, 0, 0 }
};

map <struct lws *,CB> DeribCPP_websocket::handles ;



//--------------------------
int
DeribCPP_websocket::event_cb( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{

	switch( reason )
	{
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			lws_callback_on_writable( wsi );
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:

			try {

				DeribCPP::write_log("%p %s",  wsi, (char *)in );

				string str_result = string( (char*)in );
				Json::Reader reader;
				Json::Value json_result;
				reader.parse( str_result , json_result );

				if ( handles.find( wsi ) != handles.end() ) {
					handles[wsi]( json_result );
				}

			} catch ( exception &e ) {
		 		DeribCPP::write_log( "<DeribCPP_websocket::event_cb> Error ! %s", e.what() );
			}
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			break;
		}

		case LWS_CALLBACK_CLOSED:
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			if ( handles.find( wsi ) != handles.end() ) {
				handles.erase(wsi);
			}
			break;

		default:
			break;
	}

	return 0;
}


//-------------------
void
DeribCPP_websocket::init( )
{
	struct lws_context_creation_info info;
	memset( &info, 0, sizeof(info) );

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;
	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

	context = lws_create_context( &info );
}


//----------------------------
// Register call backs
void
DeribCPP_websocket::connect_endpoint (
		CB cb
	)
{
	/* Connect if we are not connected to the server. */
	struct lws_client_connect_info ccinfo = {0};
ccinfo.context 	= context;
	ccinfo.address 	= BINANCE_WS_HOST;
	ccinfo.port 	= BINANCE_WS_PORT;
	ccinfo.path 	= BINANCE_WS_PATH;
	ccinfo.host 	= lws_canonical_hostname( context );
	ccinfo.origin 	= "origin";
	ccinfo.protocol = protocols[0].name;
	ccinfo.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;

	struct lws* conn = lws_client_connect_via_info(&ccinfo);
	handles[conn] = cb;
}


//----------------------------
// Entering event loop
void
DeribCPP_websocket::enter_event_loop()
{
	while( 1 )
	{
		try {
			lws_service( context, 500 );
		} catch ( exception &e ) {
		 	DeribCPP::write_log( "<DeribCPP_websocket::enter_event_loop> Error ! %s", e.what() );
		 	break;
		}
	}
	lws_context_destroy( context );
}

