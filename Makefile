enc_client: enc_client.c
	gcc -pthread -std=c99 -o enc_client enc_client.c

enc_server: enc_server.c
	gcc -pthread -std=c99 -o enc_server enc_server.c

dec_client: dec_client.c
	gcc -pthread -std=c99 -o dec_client dec_client.c

dec_server: dec_server.c
	gcc -pthread -std=c99 -o dec_server dec_server.c