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

// structures
typedef struct client {
	char username[50];
	char password[50];
} client;

typedef struct fileInfo {
	char path[500];
	char publ[50];
	char year[50];
} fileInfo;

//the thread function
void *connection_handler(void *);
//user id functions
bool login(char*, int); 
void get_client(char*);
bool regis(char*, int);
bool split_soal(int);
void update_tsv(char, char[]);
bool soal_c(int, char[]);
bool soal_d(int, char[]);
bool soal_e(int, char[]);
char* get_name(int);
char* get_ext(int);
bool soal_f(int);
bool soal_g(int, char[]);

//global variables
bool isOccupied;
char listofcreds[100][50];
char server_path[250] = "/home/jaglfr/Documents/SourceCodes/SisOp/Modul3/soal-shift-sisop-modul-3-C02-2021/soal1/server/";
char client_path[250] = "/home/jaglfr/Documents/SourceCodes/SisOp/Modul3/soal-shift-sisop-modul-3-C02-2021/soal1/client/";
char folderpath[250];
char logpath[250];
char tsvpath[250];
client curr_client;
fileInfo file_list[200];
int curr_numf = 0;

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
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("bind failed. Error");
		return 1;
	}
	puts("Bind done");
	//Listen
	listen(socket_desc , 10);
	
	// soalb
	strcpy(folderpath, server_path);
	mkdir(strcat(folderpath, "FOLDER/"), 0777);
	// tsv
	strcpy(tsvpath,server_path);
	strcat(tsvpath, "files.tsv");
	curr_numf = 0;
	update_tsv('r', "none");
	// soalh
	strcpy(logpath, server_path);
	strcat(logpath, "running.log");

	puts("Waiting for incoming connections...\n");
	c = sizeof(struct sockaddr_in);
	
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");
		
		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;
		
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}

		pthread_join( sniffer_thread , NULL);
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
	char client_message[2000]; memset(client_message,0,sizeof(client_message));
	char credentials[50]; memset(credentials,0,50);
	
	// Cek multiple clients
	while(isOccupied);
	isOccupied = true;

	// Kirim ke client
	send(sock , "free" , strlen("free") , 0);
	while(true) {
		bool status;
		// Looping input data
		memset(client_message,0,2000);
		bool incorrect = true;
		while( incorrect && (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
		{
			if(client_message[0] == 'l' || client_message[0] == 'r') {
				strcpy(credentials, client_message);
				incorrect = false;
			} else if (!strcmp(client_message, "exit")){
				puts("[client exit]\n");
				close(sock); free(socket_desc);
				isOccupied = false;
				return 0;
			}
			memset(client_message,0,2000);
		}

		if (credentials[0] == 'r') {
			status = regis(credentials, sock);
		} else {
			status = login(credentials, sock);
		}

		if (!status) {
			continue;
		} 
		else {
			if (credentials[0] == 'r')
				continue; 
			else {
				puts("[client logged in]");
				printf("curr usr: %s\ncurr pass: %s\n", curr_client.username, curr_client.password);
				send(sock , curr_client.username , strlen(curr_client.username), 0);
				// Setelah login
				while(true) 
				{
					char login_message[2000]; 
					memset(login_message,0,sizeof(login_message));
					recv(sock , login_message , 2000 , 0);
					// Jika logout kembali ke home menu
					if (!strcmp(login_message, "logout")) {
						puts("[client logged out]\n");
						break;
					}
					else {
						bool operation_res = split_soal(sock);
						if (!operation_res) {
							continue;
						}
					}
				}
				continue;
			}
		}

	}

	// Client terputus
	if(read_size == 0) {
		puts("Client disconnected\n");
		fflush(stdout);
	} else if(read_size == -1) {
		perror("recv failed");
	}
	close(sock);
	free(socket_desc);
	isOccupied = false;

	return 0;
}

bool login(char credentials[], int sock) {
	// menghapus huruf l
	char cleancreds[50]; memset(cleancreds,0,sizeof(cleancreds));
	for (int i = 1; i < strlen(credentials); ++i)
		cleancreds[i-1] = credentials[i];

	int counter = 0;
	FILE *fptr;
    fptr = fopen("akun.txt", "a+");
    while(fscanf(fptr, "%s\n", listofcreds[counter]) != EOF)
    	counter++;
    fclose(fptr);

    for (int i = 0; i < counter; ++i) {
    	if (!strcmp(listofcreds[i], cleancreds)) {
    		char *msg_success = "sukses";
    		get_client(cleancreds);
    		send(sock , msg_success , strlen(msg_success), 0);
    		return true;
    	}
    }
    char *msg_gagal = "gagal";
    send(sock , msg_gagal , strlen(msg_gagal), 0);
    return false;
}

void get_client(char credentials[]) {
	// memset
	memset(curr_client.username,0,sizeof(curr_client.username));
	memset(curr_client.password,0,sizeof(curr_client.password));
	// index pembagi
	int colon_idx = 0;
	for (int i = 0; i < strlen(credentials); ++i) {
		if (credentials[i] == ':')
			colon_idx = i;
	}
	// get username
	for (int i = 0; i < colon_idx; ++i)
		curr_client.username[i] = credentials[i];
	// get password
	for (int i = colon_idx + 1; i < strlen(credentials); ++i)
		curr_client.password[i-colon_idx-1] = credentials[i];
}

bool regis(char credentials[], int sock) {
	// Menghapus indikator 'r' dan parse username
	char usr_name[50]; memset(usr_name,0,sizeof(usr_name));
	int colon_idx = 0;
	char cleancreds[50]; memset(cleancreds,0,sizeof(cleancreds));
	for (int i = 1; i < strlen(credentials); ++i) {
		cleancreds[i-1] = credentials[i];
		if (credentials[i] == ':')
			colon_idx = i-1;
	}
	for (int i = 0; i < colon_idx; ++i)
		usr_name[i] = cleancreds[i];

	int counter = 0;
	FILE *fptr;
    fptr = fopen("akun.txt", "a+");
    while(fscanf(fptr, "%s\n", listofcreds[counter]) != EOF)
    	counter++;
    
    // Cek apakah sudah ada username
    for (int i = 0; i < counter; ++i) {
    	if (strstr(listofcreds[i], usr_name) != NULL) {
    		fclose(fptr);
    		char *msg_gagal = "gagal";
    		send(sock , msg_gagal , strlen(msg_gagal), 0);
    		return false;
    	}
    }

    fprintf(fptr, "%s\n", cleancreds);
    fclose(fptr);
    char *msg_success = "sukses";
    send(sock , msg_success , strlen(msg_success), 0);
    return true;
}

bool split_soal(int sock) {
	bool fin_val;
	// Menerima isi dari command
	char chg_tsv[150], newstring[150];
	memset(newstring, 0, sizeof(newstring));
	recv(sock , newstring , 150 , 0);
	// Menentukan alur
	char check_split = newstring[strlen(newstring) - 1];
	//rem_last
	for (int i = 0; i < strlen(newstring) - 1; ++i) {
		chg_tsv[i] = newstring[i];
	}
	chg_tsv[strlen(newstring) - 1] = '\0';
	
	if (check_split == 'a') {
		puts("[client add]");
		fin_val = soal_c(sock, chg_tsv);
	} else if (check_split == 'd') {
		puts("[client download]");
		fin_val = soal_d(sock, chg_tsv);
	} else if (check_split == 'e') {
		puts("[client delete]");
		fin_val = soal_e(sock, chg_tsv);
	} else if (check_split == 's') {
		puts("[client see]");
		fin_val = soal_f(sock);
	} else if (check_split == 'f') {
		puts("[client find]");
		fin_val = soal_g(sock, chg_tsv);
	}
	return fin_val;
}

void update_tsv(char command, char to_tsv[500]) {
	FILE *ftsv;

	if (command == 'r') {
		ftsv = fopen(tsvpath, "a+");
		char curr_path[500], curr_publ[50], curr_year[50];
		int counter = 0;
		while(fscanf(ftsv, "%s\t%s\t%s\n", curr_path, curr_publ, curr_year) != EOF) {
			strcpy(file_list[counter].path, curr_path);
		    strcpy(file_list[counter].publ, curr_publ);
		    strcpy(file_list[counter].year, curr_year);
		    counter++;
		}
		curr_numf = counter;
		fclose(ftsv);
	}
	else if (command == 'a') {
	    ftsv = fopen(tsvpath, "a+");
	    fprintf(ftsv, "%s\n", to_tsv);
	    fclose(ftsv);
	}
	else if (command == 'w') {
		ftsv = fopen(tsvpath, "w");
		for (int i = 0; i < curr_numf; ++i) {
			fprintf(ftsv, "%s\t%s\t%s\n", file_list[i].path, file_list[i].publ, file_list[i].year);
		}
	    fclose(ftsv);
	}
}

bool soal_c(int sock, char add_tsv[]) {
	//parsing
	char curr_publ[50] = {0}, curr_year[50] = {0}, curr_path[50] = {0};
	sscanf(add_tsv, "%s\t%s\t%s\n", curr_publ, curr_year, curr_path);

    // Client path
    char src_pth[350];
    strcpy(src_pth, client_path); strcat(src_pth, curr_path);
    printf("%s\n", src_pth);
    // Server path
    char dest_pth[350];
    strcpy(dest_pth, folderpath); strcat(dest_pth, curr_path);
    
    // Prepare files
    FILE *src_ptr, *dest_ptr;
    src_ptr = fopen(src_pth, "r");
    if (src_ptr == NULL) {
    	puts("[failed]");
    	send(sock , "op_gagal" , strlen("op_gagal"), 0);
    	return false;
    }
    dest_ptr = fopen(dest_pth,"w");
    printf("%s\n", dest_pth);
    
    // Copy file
    char temp = fgetc(src_ptr);
	while(temp != EOF){
		fputc(temp, dest_ptr);
		temp = fgetc(src_ptr);
	}
    // Close file pointers
    fclose(src_ptr); fclose(dest_ptr);
    puts("[success]");
    send(sock , "op_sukses" , strlen("op_sukses"), 0);

    // Membuat file.tsv
    char to_tsv[200];
    sprintf(to_tsv, "%s\t%s\t%s", src_pth, curr_publ,curr_year);
    update_tsv('a', to_tsv);

    // Mengisi array
    strcpy(file_list[curr_numf].path, src_pth);
    strcpy(file_list[curr_numf].publ, curr_publ);
    strcpy(file_list[curr_numf].year, curr_year);
    curr_numf++;

    //LOG
    FILE *log_ptr;
    log_ptr = fopen(logpath,"a+");
    char log_add[500];
    sprintf(log_add, "Tambah: %s (%s:%s)", curr_path, curr_client.username, curr_client.password);
    fprintf(log_ptr, "%s\n", log_add);
    fclose(log_ptr);

    return true;
}

bool soal_d(int sock, char dow_tsv[]) {
	// Menyimpan nama file
	char name[50]; strcpy(name, dow_tsv);
	// Mencari sesuai path
	char seek_name[300];
	strcpy(seek_name, client_path); strcat(seek_name, dow_tsv);
	// Mencari keberadaan
	bool found = false;
	for (int i = 0; i < curr_numf; ++i) {
		if (!strcmp(seek_name, file_list[i].path)) {
			found = true;
			break;
		}
	}
	// Action
	if (found) {
		// Prepare files
	    char temps[450]; 
	    strcpy(temps, folderpath); strcat(temps, name);
	    printf("%s\n", temps);
	    FILE *source, *destination;
	    source = fopen(temps, "r");
	    destination = fopen(seek_name,"w");
	    // Copy file
	    char tempc = fgetc(source);
		while(tempc != EOF){
			fputc(tempc, destination);
			tempc = fgetc(source);
		}
	    // Close file pointers
	    fclose(source); fclose(destination);
	    puts("[success]");
	    send(sock , "op_sukses" , strlen("op_sukses"), 0);
	    return true;

	} else {
		puts("[failed]");
		send(sock , "op_gagal" , strlen("op_gagal"), 0);
    	return false;
	}
}

bool soal_e(int sock, char del_tsv[]){
	char name[50]; strcpy(name, del_tsv);
	// Mencari sesuai path
	char seek_name[300];
	strcpy(seek_name, client_path); strcat(seek_name, del_tsv);
	// Mencari keberadaan
	bool found = false; int location = 0;
	for (int i = 0; i < curr_numf; ++i) {
		if (!strcmp(seek_name, file_list[i].path)) {
			found = true;
			location = i;
			break;
		}
	}
	// Action
	if (found) {
		// rename file
		char newname[300] = {0}, oldname[300] = {0};
		sprintf(newname, "%sold-%s", folderpath, name);
		sprintf(oldname, "%s%s", folderpath, name);
		rename(oldname, newname);

		// Menghapus isi array
		for (int i = location; i < curr_numf - 1; ++i) {
			strcpy(file_list[i].path, file_list[i+1].path);
			strcpy(file_list[i].publ, file_list[i+1].publ);
			strcpy(file_list[i].year, file_list[i+1].year);
		}
		curr_numf--;
		for (int i = 0; i < curr_numf; ++i)
		{
			printf("%s\t%s\t%s\n", file_list[i].path, file_list[i].publ, file_list[i].year);
		}
		
		update_tsv('w', "none");

		//LOG
	    FILE *log_ptr;
	    log_ptr = fopen(logpath,"a+");
	    char log_add[500];
	    sprintf(log_add, "Hapus: %s (%s:%s)", name, curr_client.username, curr_client.password);
	    fprintf(log_ptr, "%s\n", log_add);
	    fclose(log_ptr);
	    // End
	    puts("[success]");
	    send(sock , "op_sukses" , strlen("op_sukses"), 0);
	    return true;
	} 
	else {
		puts("[failed]");
		send(sock , "op_gagal" , strlen("op_gagal"), 0);
    	return false;
	}
}

char* get_name(int name_idx) {
	char *temp;
	char namecopy[500]; strcpy(namecopy,file_list[name_idx].path);
	temp = strrchr(namecopy, '/');
	for (int i = 0; i < strlen(temp); ++i)
		temp[i] = temp[i+1];
  	return temp;
}

char* get_ext(int ext_idx) {
	char *temp;
	char extcopy[500]; strcpy(extcopy,file_list[ext_idx].path);
	temp = strrchr(extcopy, '.');
	for (int i = 0; i < strlen(temp); ++i)
		temp[i] = temp[i+1];
  	return temp;
}

bool soal_f(int sock){
	char multi_string[1000] = {0};
	memset(multi_string,0,1000);memset(multi_string,0,1000);
	char name[50], ext[10];
	for (int i = 0; i < curr_numf; ++i) {
		// nama
		strcpy(multi_string, "Nama: ");
		strcpy(name, get_name(i));
		strcat(multi_string, name);
		// publisher
		strcat(multi_string, "\nPublisher: ");
		strcat(multi_string, file_list[i].publ);
		// tahun
		strcat(multi_string, "\nTahun publishing: ");
		strcat(multi_string, file_list[i].year);
		// extensi
		strcat(multi_string, "\nEkstensi File: ");
		strcpy(ext, get_ext(i));
		strcat(multi_string, ext);
		// filepath
		strcat(multi_string, "\nFilepath: ");
		strcat(multi_string, file_list[i].path);
		// kirim string
		send(sock , multi_string , strlen(multi_string), 0);
		send(sock , "\n" , strlen("\n"), 0);
		
		printf("\n%s\n", multi_string);
		puts("sent");
		memset(multi_string,0,1000);
	}
	send(sock , "done" , strlen("done"), 0);
	puts("[success]");
	return true;
}

bool soal_g(int sock, char find_tsv[]){
	// Menyimpan nama file
	char temp_name[50]; strcpy(temp_name, find_tsv);
	bool found = false;
	char multi_string[1000] = {0};memset(multi_string,0,1000);
	for (int i = 0; i < curr_numf; ++i) {
		char name[50], ext[10];
		strcpy(name, get_name(i));
		printf("t:%s n:%s\n", temp_name, name);
		if (strstr(name, temp_name) != NULL) {
			found = true;
			// nama
			strcpy(multi_string, "Nama: ");
			strcat(multi_string, name);
			// publisher
			strcat(multi_string, "\nPublisher: ");
			strcat(multi_string, file_list[i].publ);
			// tahun
			strcat(multi_string, "\nTahun publishing: ");
			strcat(multi_string, file_list[i].year);
			// extensi
			strcat(multi_string, "\nEkstensi File: ");
			strcpy(ext, get_ext(i));
			strcat(multi_string, ext);
			// filepath
			strcat(multi_string, "\nFilepath: ");
			strcat(multi_string, file_list[i].path);
			strcat(multi_string,"\n");
			// kirim string
			send(sock , multi_string , strlen(multi_string), 0);
			memset(multi_string,0,1000);
		} else {
			continue;
		}
	}
	if (found) {
		send(sock , "done" , strlen("done"), 0);
		puts("[success]");
		// send(sock , "op_sukses" , strlen("op_sukses"), 0);
		// send(sock , "op_sukses" , strlen("op_sukses"), 0);
		// puts("[success]");
		return true;
	} else {
		send(sock , "op_gagal" , strlen("op_gagal"), 0);
		send(sock , "op_gagal" , strlen("op_gagal"), 0);
		puts("[failed]");
		return false;
	}
}