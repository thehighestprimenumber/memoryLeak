#include "common.h"

void free_memory(void **puntero){
	if((*puntero) != NULL){
		free(*puntero);
		*puntero = NULL;
	}
}

char* get_local_ip(){
    const char* google_dns_server = "8.8.8.8";
    int dns_port = 53;

    struct sockaddr_in serv;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    //Socket could not be created
    if(sock < 0)
    {
    	return "127.0.0.1";
    }

    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(google_dns_server);
    serv.sin_port = htons(dns_port);

    int err = connect(sock, (const struct sockaddr*)&serv, sizeof(serv));
    if (err < 0)
    {
        return "127.0.0.1";
    }

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*)&name, &namelen);

    char *buffer = calloc(1, 16);
    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 80);
    close(sock);
    if(p == NULL){
    	return "127.0.0.1";
    }
    return buffer;
}

int cantidad_lineas_script(char* script){

	int n_lineas = 0;

	for(int i = 0; script[i] != '\0'; i++){
	if (script[i] == '\n') n_lineas++;
	}

	return n_lineas + 1;

}
