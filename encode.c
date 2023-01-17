#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strcmp(strstr(argv[2],"."), ".bmp")==0)
    {
	encInfo -> src_image_fname = argv[2];
    }
    else
    {
	return e_failure;
    }
    if(strcmp(strstr(argv[3],"."), ".txt") == 0)
    {
	encInfo -> secret_fname = argv[3];
    }
    else
    {
	return e_failure;
    }
    if(argv[4]!=NULL)
    {
	encInfo -> stego_image_fname = argv[4];
    }
    else
    {
        encInfo -> stego_image_fname = "stego.bmp";
    }
    return e_success;
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);


    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    if (encInfo -> image_capacity > (16+32+32+32+(encInfo -> size_secret_file*8)))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }

}

Status copy_bmp_header(FILE *fptr_src_image, FILE     *fptr_dest_image)
{
    char str[54];
    rewind(fptr_src_image);
    fread(str,sizeof(char),54,fptr_src_image);
    fwrite(str,sizeof(char),54,fptr_dest_image);
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string,strlen(magic_string),encInfo -> fptr_src_image,encInfo -> fptr_stego_image,encInfo);
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image,EncodeInfo *encInfo)
{
    for(int i=0;i<size;i++)
    {
	fread(encInfo -> image_data,sizeof(char),8,fptr_src_image);
	encode_byte_to_lsb(data[i],encInfo -> image_data);
	fwrite(encInfo -> image_data,sizeof(char),8,fptr_stego_image);
    }
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    uint mask = 1<<7;
    for( int i=0;i<8;i++)
    {
	image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask ) >> (7-i));
	mask=mask >> 1;
    }
}

Status encode_secret_file_extn_size(int size,FILE *fptr_src_image,FILE *fptr_dest_image )
{
    char str[32];
    fread(str,sizeof(char),32,fptr_src_image);
    encode_size_to_lsb(str,size);
    fwrite(str,sizeof(char),32,fptr_dest_image);
    return e_success;
}


Status encode_size_to_lsb(char *buffer,int size)
{
    uint mask = 1 << 31;
    for(int i=0;i<32;i++)
    {
	buffer[i]=(buffer[i] & 0xFE) | ((size & mask) >> (31-i) );
	mask=mask>>1;
    }
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo -> fptr_src_image,encInfo -> fptr_stego_image,encInfo);
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str,sizeof(char),32,encInfo -> fptr_src_image);
    encode_size_to_lsb( str,file_size);
    fwrite(str,sizeof(char),32,encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_buff[encInfo -> size_secret_file];
    char str[encInfo -> size_secret_file * 8];

   encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");  
    
   fread(secret_buff,sizeof(char),encInfo -> size_secret_file,encInfo ->fptr_secret);
  
   encode_data_to_image(secret_buff,encInfo -> size_secret_file,encInfo -> fptr_src_image,encInfo ->  fptr_stego_image, encInfo);

   return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,sizeof(char),1,fptr_src)>0)
    {
	fwrite(&ch,sizeof(char),1, fptr_dest);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if( open_files (encInfo) == e_success)
    {
	printf("Files are opened successfully\n");
    }
    else
    {
	printf("open file function is a failure\n");
    }

    if (check_capacity (encInfo) == e_success)
    {
	printf("Check capacity is successfully executed\n");
    }
    else
    {
	printf("check capacity is a failure\n");
    }

   if ( copy_bmp_header (encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
    {
	printf("Copy bmp is successful\n");
    }
    else
    {
	printf("Copy bmp is a failure\n");
    }
    if( encode_magic_string(MAGIC_STRING,encInfo) == e_success)
    {
	printf("encode magic string is successful\n");
	strcpy(encInfo -> extn_secret_file,strstr (encInfo -> secret_fname, "."));
    }
    else
    {
	printf("encode magic string is a failure\n");
    }
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo->fptr_src_image, encInfo ->fptr_stego_image) == e_success)
    {
	printf("Encode secret file extn size is successfully completed\n");
    }
    else
    {
	printf("Encode secret file extn size is failure\n");
    }
    if( encode_secret_file_extn(encInfo -> extn_secret_file,encInfo) == e_success)
    {
	printf("Encode secret file extn is successfully completed\n");
    }
    else
    {
	printf("Encode secret file extn is a failure\n");
    }

    if ( encode_secret_file_size(encInfo -> size_secret_file,encInfo )== e_success)
    {
	printf("Encode secret file size is successful\n");
    }
    else
    {
	printf("Encode secret file size is a failure\n");
    }
    if (encode_secret_file_data(encInfo) == e_success)
    {
	printf("Encode secret file data is successful\n");
    }
    else
    {
	printf("Encode secret file data is a failure\n");
    }
    if(copy_remaining_img_data(encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
    {
	printf("Remaining data is successfully copied\n");
    }
    else
    {
	printf("Copying remaining data function is failure\n");
    }
  
    return e_success;
}
