#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define USER_FILE "users.txt"
#define KEY_FILE  "aes.key"

static char *bytes_to_hex(const unsigned char *buf, int len) {
    char *hex=malloc(len*2+1);
    for(int i=0;i<len;i++) sprintf(hex+2*i,"%02x",buf[i]);
    hex[len*2]='\0'; return hex;
}

int main(int argc,char**argv){
    if(argc!=3){fprintf(stderr,"Uso: %s usuario password\n",argv[0]); return 1;} char *user=argv[1], *pass=argv[2];
    unsigned char key[32];
    // Load or generate key
    FILE *kf=fopen(KEY_FILE,"rb");
    if(kf){ fread(key,1,32,kf); fclose(kf);} else {
        RAND_bytes(key,32);
        kf=fopen(KEY_FILE,"wb"); fwrite(key,1,32,kf); fclose(kf);
    }
    unsigned char iv[16]; RAND_bytes(iv,16);
    EVP_CIPHER_CTX *ctx=EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx,EVP_aes_256_cbc(),NULL,key,iv);
    int len, ct_len;
    unsigned char ct[256];
    EVP_EncryptUpdate(ctx,ct,&len,(unsigned char*)pass,strlen(pass)); ct_len=len;
    EVP_EncryptFinal_ex(ctx,ct+len,&len); ct_len+=len;
    EVP_CIPHER_CTX_free(ctx);

    char *hex_iv=bytes_to_hex(iv,16);
    char *hex_ct=bytes_to_hex(ct,ct_len);
    FILE *f=fopen(USER_FILE,"a");
    fprintf(f,"%s:%s:%s\n",user,hex_iv,hex_ct);
    fclose(f);
    free(hex_iv); free(hex_ct);
    printf("Usuario %s agregado.\n",user);
    return 0;
}
