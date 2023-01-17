#include<stdio.h> 
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char **argv)
{
    //validate the cla count

    int operation=check_operation_type(argv);
    if(operation == e_encode)
    {
	EncodeInfo encInfo;
	printf("Selected encoding\n");
	if (read_and_validate_encode_args(argv,&encInfo)==e_success)
	{
	    printf("read and validate is successfully executed\n");
	    if( do_encoding (&encInfo) == e_success)
	    {
		printf("Encoding successful\n");
	    }
	    else
	    {
		printf("Encoding unsuccessful\n");
	    }
	}
	else
	{
	    printf("read and validate is unsuccessful\n");
	    return -1;
	}
    }
    else if(operation == e_decode)
    {
	DecodeInfo decInfo;
	printf("Selected decoding\n");

	if (read_and_validate_decode_args(argv,&decInfo)==e_success)
	{
	    printf("read and validate is successfully executed\n");
	    if( do_decoding (&decInfo) == e_success)
	    {
	    }
	    else
	    {
	    }
	}
	else
	{
	    printf("read and validate is unsuccessful\n");
	    return -1;
	}

    }
    else
    {
	printf("Operation is invalid\n");
    }
    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
    {
	return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }

}

