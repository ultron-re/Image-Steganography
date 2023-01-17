#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strcmp(strstr(argv[2],"."), ".bmp")==0)
    {
	decInfo -> stego_image_fname = argv[2];
    }
    else
    {
	return e_failure;
    }
    if(argv[4]!=NULL)
    {
	decInfo -> decoded_fname = argv[4];
    }
    else
    {
        decInfo -> decoded_fname = "decoded.txt";
    }
    return e_success;
}

Status open_files_dec(DecodeInfo *decInfo)
{
    //Stego image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }

    //Decoded text
    decInfo->fptr_decoded = fopen(decInfo->decoded_fname, "w");
    // Do Error handling
    if (decInfo->fptr_decoded == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decoded_fname);

        return e_failure;
    }
    // No failure return e_success
    return e_success;
}

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    int flag=0;

    flag=decode_data_from_image(magic_string,strlen(magic_string),decInfo -> fptr_stego_image,decInfo -> fptr_decoded,decInfo);
    if(flag==1)
    {
        return e_success;
    }
    else
	return e_failure;
}

Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image, FILE *fptr_decoded,DecodeInfo *decInfo)
{
    char *data1=malloc(size);
    data1[0]=data1[0]&0x00;

    for(int i=0;i<size;i++)
    {
        fread(decInfo -> decoded_data,sizeof(char),8,fptr_stego_image);
       
	decode_byte_from_lsb(decInfo -> decoded_data);

	for(int k=0;k<8;k++)
	{
	    data1[i]= (data1[i] | decInfo -> decoded_data[k])<<1;
	}
	data1[i]=data1[i]>>1;
	data1[i]=data1[i]&0x7F;
    }

    if(size==decInfo-> size_secret_file)
    {
    fwrite(data1,sizeof(char),size,fptr_decoded);
    }

    if(strcmp(data,data1)==0)
    {
	return 1;
    }
}

Status decode_byte_from_lsb(char *decode_buffer)
{
    uint mask=1;
    for(int i=0;i<8;i++)
    {
      decode_buffer[i]=((decode_buffer[i]) & mask);
    }
   
}

Status decode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_dest_file)
{
    char str[32];
    char *data1=malloc(1);
    fread(str,sizeof(char),32,fptr_src_image);
    decode_size_from_lsb(str,size);
    for(int k=0;k<32;k++)
    {
	data1[0]= (data1[0] | str[k])<<1;
    }
    data1[0]=data1[0]>>1;

    if(data1[0]==4)
    {
    return e_success;
    }
    else
	return e_failure;
}

Status decode_size_from_lsb(char *buffer,int size)
{
    uint mask=1;
    for(int i=0;i<32;i++)
    {
        buffer[i]=((buffer[i]) & mask);
    }
}

Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo)
{
    int flag=0;
    flag=decode_data_from_image(file_extn, 4, decInfo -> fptr_stego_image,decInfo -> fptr_decoded,decInfo);
    if(flag==1)
    {
        return e_success;
    }
    else
	return e_failure;
}

Status decode_secret_file_size(long file_size, DecodeInfo *decInfo)
{
    char str[32];
    char *data1=malloc(1);
     
    fread(str,sizeof(char),32,decInfo -> fptr_stego_image);
    decode_size_from_lsb( str,file_size);
    for(int k=0;k<32;k++)
    {
        data1[0] = (data1[0] | str[k]) << 1;
    }
    data1[0] = data1[0] >> 1;

    decInfo -> size_secret_file=data1[0];
   
    return e_success;
    
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
   
    char secret_buff[decInfo -> size_secret_file];
    char str[ decInfo -> size_secret_file * 8 ];

    decode_data_from_image(secret_buff,decInfo -> size_secret_file,decInfo -> fptr_stego_image,decInfo ->  fptr_decoded, decInfo);
	
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if( open_files_dec (decInfo) == e_success)
    {
	printf("Files are opened successfully\n");
    }
    else
    {
	printf("Opening files was unsuccessful\n");
    }

    fseek(decInfo->fptr_stego_image,54,SEEK_SET);

    if( decode_magic_string(MAGIC_STRING,decInfo) == e_success)
    {

        printf("Decode magic string is successful\n");
        strcpy(decInfo -> extn_secret_file,strstr (decInfo -> decoded_fname, "."));
    }
    else
    {
        printf("Decode magic string was unsuccessful\n");
    }
    if(decode_secret_file_extn_size(strlen(decInfo->extn_secret_file),decInfo->fptr_stego_image, decInfo ->fptr_decoded) == e_success)
    {
	printf("Decode secret file extn size successful\n");
    }
    else
    {
	printf("Decode secret file extn size is a failure\n");
    }
    if(decode_secret_file_extn(decInfo -> extn_secret_file,decInfo) == e_success)
    {
	printf("Decoding secret file extension successful\n");
    }
    else
    {
	printf("Decoding secret file extn is a failure\n");
    }
    if ( decode_secret_file_size(decInfo -> size_secret_file,decInfo )== e_success)
    {
        printf("Decode secret file size is successful\n");
    }
    else
    {
        printf("Decode secret file size is a failure\n");
    }
    
    if (decode_secret_file_data(decInfo) == e_success)
    {
       printf("Decode secret file data is successful\n");
    }
    else
    {
        printf("Decode secret file data is a failure\n");
    }

}
