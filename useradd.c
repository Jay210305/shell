#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

static char *bytes_to_hex(const unsigned char *in, int len) {
    char *hex = malloc(len*2 + 1), *p = hex;
    for(int i=0;i<len;i++) p += sprintf(p, "%02x", in[i]);
    *p = '\0';
    return hex;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s usuario password\n", argv[0]);
        return 1;
    }
    char *user = argv[1], *pass = argv[2];

    // Carga clave pública
    FILE *f = fopen("public.pem", "r");
    if (!f) { perror("public.pem"); return 1; }
    RSA *rsa = PEM_read_RSA_PUBKEY(f, NULL, NULL, NULL);
    fclose(f);
    if (!rsa) { fprintf(stderr, "Error leyendo public.pem\n"); return 1; }

    // Cifra
    unsigned char buf[256];
    int outlen = RSA_public_encrypt(strlen(pass), (unsigned char*)pass,
                                    buf, rsa, RSA_PKCS1_PADDING);
    RSA_free(rsa);
    if (outlen < 0) { fprintf(stderr, "Error en encrypt\n"); return 1; }

    // Hex y appends a users.txt
    char *hex = bytes_to_hex(buf, outlen);
    FILE *uf = fopen("users.txt", "a");
    fprintf(uf, "%s:%s\n", user, hex);
    fclose(uf);
    free(hex);
    printf("Usuario %s agregado.\n", user);
    return 0;
}