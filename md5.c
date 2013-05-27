#include <stdio.h>
#include <openssl/md5.h>
char result[16];

char* calc_md5(char* file)
{
    unsigned char c[MD5_DIGEST_LENGTH];
    FILE *inFile = fopen (file, "rb");
    MD5_CTX mdContext;
    int bytes;
    int i;
    
    unsigned char data[1024];

    if (inFile == NULL) {
        printf ("%s can't be opened.\n", file);
        return 0;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
    result[0]='\0';
    for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
    	sprintf(result,"%s%02x",result, c[i]);
    }
    fclose (inFile);
    return result;
}
