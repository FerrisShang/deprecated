#include <stdio.h>
#include <unistd.h>


#include "attribute_data.h"
#include "notif_value.h"



unsigned char data1 [] = {0, 0, 0, 0, 0, 1, 2, 0, 81, 81, 2, 0, 0, 3, 16, 0, -24, -65, -121, 91, -27, -89, -100, 93, -23, -66, -103, 58, 108, 105, 107, 101, 4, 2, 0, 49, 54, 10, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 5, 15, 0, 50, 48, 49, 53, 49, 50, 48, 50, 84, 49, 54, 50, 53, 53, 49, 10, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 100, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113};




unsigned char data2[] = {0, 5, 0, 0, 0, 1, 2, 0, 81, 81, 2, 0, 0, 3, 102, 0, -24, -65, -121, 91, -27, -89, -100, 93, -23, -66, -103, 58, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 106, 4, 3, 0, 49, 48, 50, 10, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 5, 15, 0, 50, 48, 49};




unsigned char data21[] = {53, 49, 50, 48, 50, 84, 49, 55, 52, 49, 53, 51, 10, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 100, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113}; 





unsigned char data3[] = {0, 7, 0, 0, 0, 4, 2, 0, 51, 53, 5, 15, 0, 50, 48, 49, 53, 49, 50, 48, 50, 84, 49, 55, 49, 56, 50, 53, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113};



unsigned char data4[] = {0, 3, 0, 0, 0, 1, 2, 0, 81, 81, 2, 0, 0, 3, 28, 0, -24, -65, -121, 91, -27, -89, -100, 93, -23, -66, -103, 58, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 4, 2, 0, 50, 56, 10, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 5, 15, 0, 50, 48, 49, 53, 49, 50, 48, 50, 84, 49, 55, 49, 52, 49, 49, 10, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 100, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110};
unsigned char data41[] = {116, 46, 109, 105, 112, 97, 100, 113, 113};




unsigned char data5[] = {0, 4, 0, 0, 0, 1, 2, 0, 81, 81, 2, 0, 0, 3, 26, 0, -24, -65, -121, 91, -27, -89, -100, 93, -23, -66, -103, 58, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 4, 2, 0, 50, 54, 10, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 5, 15, 0, 50, 48, 49, 53, 49, 50, 48, 50, 84, 49, 55, 49, 55, 50, 54, 10, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46, 109, 105, 112, 97, 100, 113, 113, 100, 0, 0, 0, 19, 0, 99, 111, 109, 46, 116, 101, 110, 99, 101, 110, 116, 46};
unsigned char data51[] = {109, 105, 112, 97, 100, 113, 113};



unsigned char command[] = {0, 21, 4, 1, 0, 0, 0, 0};


static void my_callback(void* arg);


int main(int argc, char* argv[])
{
	int i = 1;
	start_resp_assembler_session(my_callback);	
	unsigned char* req_tmp = NULL;
	unsigned char* req_buf = NULL;
	int req_len = 0;

	while(i--){
		resp_data_assembling(data1,sizeof(data1));
		usleep(2000000);

		resp_data_assembling(data2,sizeof(data2));
		usleep(50000);
		resp_data_assembling(data21,sizeof(data21));
		usleep(150000);

		resp_data_assembling(data3,sizeof(data3));
		usleep(120000);

		resp_data_assembling(data4,sizeof(data4));
		usleep(80000);
		resp_data_assembling(data41,sizeof(data41));
		usleep(120000);

		resp_data_assembling(data5,sizeof(data5));
		usleep(80000);
		resp_data_assembling(data51,sizeof(data51));
		usleep(120000);
	}

	sleep(2);
	end_resp_assembler_session();

	print_character(command);

	notif_req_assembling(&req_tmp,&req_len,command,0x3F);	

	DEBUG("%s, %d\n",__func__,__LINE__);
	for(i = 0;i < req_len;i++){
		printf("%d ",req_tmp[i]);
	}

	printf("\n");

	return 0;
}


void print_data(void* arg)
{
	Resp_data *data = arg;
	
	DEBUG("#######################################################\n");

	DEBUG("commandid = %d\n",data->commandid);

	DEBUG("notif_uid = %lx\n", *(unsigned long*) data->notif_uid);

	if(data->app_id_str != NULL)
		DEBUG("app_id_str = %s\n",data->app_id_str);

	if(data->app_identifier != NULL)
		DEBUG("app_identifier = %s\n",data->app_identifier);

	if(data->title != NULL)
		DEBUG("title = %s\n", data->title);

	if(data->subtitle != NULL)
		DEBUG("subtitle = %s\n",data->subtitle);

	if(data->message != NULL)
		DEBUG("message = %s\n",data->message);

	if(data->message_size != NULL)
		DEBUG("message_size = %s\n",data->message_size);

	if(data->date != NULL)
		DEBUG("data = %s\n",data->date);

	if(data->positive_actionlabel != NULL)
		DEBUG("positive_actionlabel = %s\n",data->positive_actionlabel);

	if(data->negative_actionlabel != NULL)
		DEBUG("negative_actionlabel = %s\n",data->negative_actionlabel);


	DEBUG("***********************************************************\n");
	return;
}


void my_callback(void* arg)
{
	if(arg == NULL){
		DEBUG("%s: data is NULL\n",__func__);
		return;
	}

	print_data(arg);

}
