Build str from console: 
g++ -L3rd_library/libcurl/lib -L3rd_library/libwebsockets-2.4.0/lib -o bin/Debug/Deribit obj/Debug/3rd_library/jsoncpp/src/jsoncpp.o obj/Debug/Derib_websocket.o obj/Debug/DeribCPP.o obj/Debug/main.o  -lcurl -lwebsockets  3rd_library/libcurl/lib/libcurl.so 3rd_library/libwebsockets-2.4.0/lib/libwebsockets.so
