#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>	
#include <stdlib.h>	
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <unistd.h>	
#include <pthread.h> 
#include <stdbool.h>

//the thread function
void *connection_handler(void *);

//user id functions
void login(char*, int); 
void regis(char*, int);
void soal_b();
void soal_c(int);

//global variables
bool isOccupied = false;
char listofcreds[100][50];
char dir_path[250] = "/home/jaglfr/Documents/Source Codes/SisOp/Modul3/soal-shift-sisop-modul-3-C02-2021/soal1/server/";
char folderpath[250]; 

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	
	//Listen
	listen(socket_desc , 3);
	
	soal_b();

	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");
		
		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;
		
		// SOAL A
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}
		
		//Now join the thread , so that we dont terminate before the thread
		pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
	}
	
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	
	return 0;
}


void *connection_handler(void *socket_desc)
{
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	char *message , client_message[2000];
	memset(client_message,0,2000);

	if (!isOccupied) {
		// isOccupied = true;
		// send(sock, &isOccupied, sizeof(isOccupied), 0);
		message = "Selamat datang, silahkan pilih antara [login] atau [register]\n";
		write(sock , message , strlen(message)); 
		//Receive a message from client
		bool incorrect = true;
		// read_size = recv(sock , client_message , 2000 , 0);
		while( incorrect && (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
		{
			char chosen[4] = "err";
			if(!strcmp(client_message,"login")) {
				strcpy(chosen, "log");
				incorrect = false;
			} else if (!strcmp(client_message,"register")) {
				strcpy(chosen, "reg");
				incorrect = false;
			} else {
				// read_size = recv(sock , client_message , 2000 , 0);
			}
			//Send the message back to client
			write(sock , chosen , strlen(chosen));
			memset(client_message,0,2000);
		}
		if(read_size == 0) {
			puts("Client disconnected");
			fflush(stdout);
		} else if(read_size == -1) {
			perror("recv failed");
		}
		char credentials[50]; memset(credentials,0,50);
		recv(sock , credentials , 50 , 0);

		if (credentials[0] == 'r') {
			regis(credentials, sock);
		} else {
			login(credentials, sock);
		}
		close(sock);
		free(socket_desc);
	}
	return 0;
}

void login(char credentials[], int sock) {
	char cleancreds[50]; memset(cleancreds,0,50);
	for (int i = 1; i < strlen(credentials); ++i)
		cleancreds[i-1] = credentials[i];
	printf("%s\n", cleancreds);
	
	int counter = 0;
	FILE *fptr;
    fptr = fopen("akun.txt", "a+");
    while(fscanf(fptr, "%s\n", listofcreds[counter]) != EOF)
    	counter++;

    for (int i = 0; i < counter; ++i) {
    	if (!strcmp(listofcreds[i], cleancreds)) {
    		char *msg_success = "login sukses";
    		write(sock , msg_success , strlen(msg_success));
    		soal_c(sock);
    		return;
    	}
    }
    char *msg_gagal = "gagal";
    fclose(fptr);
    write(sock , msg_gagal , strlen(msg_gagal));
}


void regis(char credentials[], int sock) {
	char cleancreds[50]; memset(cleancreds,0,50);
	for (int i = 1; i < strlen(credentials); ++i)
		cleancreds[i-1] = credentials[i];
	
	int counter = 0;
	FILE *fptr;
    fptr = fopen("akun.txt", "a+");
    while(fscanf(fptr, "%s\n", listofcreds[counter]) != EOF)
    	counter++;

    for (int i = 0; i < counter; ++i) {
    	if (strcmp(listofcreds[i], cleancreds)) {
    		char *msg_gagal = "gagal";
    		write(sock , msg_gagal , strlen(msg_gagal));
    		return;
    	}
    }

    fprintf(fptr, "%s\n", cleancreds);
    fclose(fptr);
    char *msg_success = "register sukses";
    write(sock , msg_success , strlen(msg_success));
}


void soal_b() {
	// Membuat folder
	strcpy(folderpath, dir_path);
	mkdir(strcat(folderpath, "FOLDER/"), 0777);
}

void soal_c(int sock) {
	// Menerima isi dari command add
	char add_tsv[150];
	recv(sock , add_tsv , 150 , 0);
	
	// Membuat file.tsv
	char filespath[250]; strcpy(filespath, dir_path);
	FILE *cptr;
    cptr = fopen(strcat(filespath, "files.tsv"), "a+");
    fprintf(cptr, "%s\n", add_tsv);
    fclose(cptr);

    // parse sourse path
    char copy_path[150]; int tab_count = 0, tab_idx = 0;
    for (int i = 0; i < strlen(add_tsv); ++i) {
    	if (add_tsv[i] == '\t')
    		tab_count++;
    	if (tab_count == 2) {
    		tab_idx = i;
    		break;
    	}
    }
    for (int i = tab_idx++; i < strlen(add_tsv); ++i) {
    	copy_path[i - tab_idx] = add_tsv[i];
    }
    copy_path[strlen(add_tsv) - tab_idx + 1] = '\0';
    // printf("tsv: %s\n", add_tsv);
    // printf("%s\n", copy_path);

    //parse filename
    char filename[150]; int last_slash = 0, exten_point = 0;
    for (int i = 0; i < strlen(copy_path); ++i) {
    	if (copy_path[i] == '/')
    		last_slash = i;
    	if (copy_path[i] == '.') {
    		exten_point = i;
    		break;
    	}
    }
    for (int i = last_slash++; i < strlen(copy_path); ++i) {
    	filename[i - last_slash] = copy_path[i];
    }
    filename[strlen(copy_path) - last_slash + 1] = '\0';
    // printf("filename\n%s\n", filename);
    
    // copy file
    char temp[320], temp_path[250];
    strcpy(temp_path, folderpath);
    
    FILE *target_ptr, *source_ptr;
    source_ptr = fopen(copy_path, "r+");
    target_ptr = fopen(strcat(temp_path, filename),"a+");
    
    while(fscanf(source_ptr, "%s\n", temp) != EOF) {
    	fprintf(target_ptr, "%s\n", temp);
    	printf("%s\n", temp);
    }
    fclose(source_ptr); fclose(target_ptr);
}