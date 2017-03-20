#include <stdio.h>
#include <stdlib.h>
#include "json.h"

char *string_to_parse=
	 "{\"status\":\"OK\",\"result\":[{\"contestId\":791,"
	 "\"contestName\":\"Codeforces Round #405 "
	 "(rated, Div. 2, based on VK Cup 2017 Round 1)\","
	 "\"handle\":\"Ferris\",\"rank\":896,"
	 "\"ratingUpdateTimeSeconds\":1489858500,"
	 "\"oldRating\":1500,\"newRating\":1523}]}";

int main(int argc, char *argv[])
{
	struct json_object *new_obj, *status, *result, *field;
	struct array_list *result_array;
	new_obj = json_tokener_parse(string_to_parse);

	json_object_object_get_ex(new_obj, "status", &status);
	printf("%s\n", json_object_get_string(status));

	json_object_object_get_ex(new_obj, "result", &result);
	printf("%s\n", json_object_get_string(result));

	result_array = json_object_get_array(result);
	result = (struct json_object*)result_array->array[0];

	json_object_object_get_ex(result, "contestId", &field);
	printf("contestId = %d\n", json_object_get_int(field));

	json_object_object_get_ex(result, "contestName", &field);
	printf("contestName = %s\n", json_object_get_string(field));

	json_object_object_get_ex(result, "handle", &field);
	printf("handle = %s\n", json_object_get_string(field));

	json_object_object_get_ex(result, "rank", &field);
	printf("rank = %d\n", json_object_get_int(field));

	json_object_object_get_ex(result, "ratingUpdateTimeSeconds", &field);
	printf("ratingUpdateTimeSeconds = %d\n", json_object_get_int(field));

	json_object_object_get_ex(result, "oldRating", &field);
	printf("oldRating = %d\n", json_object_get_int(field));

	json_object_object_get_ex(result, "newRating", &field);
	printf("newRating = %d\n", json_object_get_int(field));

	json_object_put(new_obj);
}
