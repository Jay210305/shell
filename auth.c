#include "auth.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define USER_FILE "users.txt" // username:hex(iv):hex(ciphertext)
#define KEY_FILE "aes.key"    // binary key file
#define MAX_LINE 1024

// Convert hex to bytes
static unsigned char *hex_to_bytes(const char *hex, int *out_len)
{
    int len = strlen(hex) / 2;
    *out_len = len;
    unsigned char *buf = malloc(len);
    for (int i = 0; i < len; i++)
        sscanf(hex + 2 * i, "%2hhx", &buf[i]);
    return buf;
}

// Convert bytes to hex
static char *bytes_to_hex(const unsigned char *buf, int len)
{
    char *hex = malloc(len * 2 + 1);
    for (int i = 0; i < len; i++)
        sprintf(hex + 2 * i, "%02x", buf[i]);
    hex[len * 2] = '\0';
    return hex;
}

bool authenticate()
{
    char user[128], pass[128];
    printf("Usuario: ");
    if (!fgets(user, sizeof(user), stdin))
        return false;
    user[strcspn(user, "\n")] = '\0';
    printf("Contraseña: ");
    if (!fgets(pass, sizeof(pass), stdin))
        return false;
    pass[strcspn(pass, "\n")] = '\0';

    FILE *kf = fopen(KEY_FILE, "rb");
    if (!kf)
    {
        perror("fopen aes.key");
        return false;
    }
    unsigned char key[32];
    if (fread(key, 1, 32, kf) != 32)
    {
        fclose(kf);
        return false;
    }
    fclose(kf);

    FILE *f = fopen(USER_FILE, "r");
    if (!f)
    {
        perror("fopen users.txt");
        return false;
    }
    char line[MAX_LINE];
    bool ok = false;
    while (fgets(line, sizeof(line), f))
    {
        char *u = strtok(line, ":");
        char *hex_iv = strtok(NULL, ":");
        char *hex_ct = strtok(NULL, "\n");
        if (!u || !hex_iv || !hex_ct)
            continue;
        if (strcmp(u, user) != 0)
            continue;
        int iv_len, ct_len;
        unsigned char *iv = hex_to_bytes(hex_iv, &iv_len);
        unsigned char *ct = hex_to_bytes(hex_ct, &ct_len);

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
            goto finish;
        unsigned char pt[256];
        int len, plaintext_len;
        if (!EVP_DecryptUpdate(ctx, pt, &len, ct, ct_len))
            goto finish;
        plaintext_len = len;
        if (!EVP_DecryptFinal_ex(ctx, pt + len, &len))
            goto finish;
        plaintext_len += len;
        pt[plaintext_len] = '\0';
        if (strcmp((char *)pt, pass) == 0)
            ok = true;
    finish:
        EVP_CIPHER_CTX_free(ctx);
        free(iv);
        free(ct);
        break;
    }
    fclose(f);
    if (ok)
        printf("Autenticación exitosa.\n");
    else
        printf("Acceso denegado.\n");
    return ok;
}

void list_users()
{
    FILE *f = fopen(USER_FILE, "r");
    if (!f)
    {
        perror("fopen users.txt");
        return;
    }
    printf("Usuarios registrados:\n");
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = '\0';
        char *u = strtok(line, ":");
        if (u)
            printf("  %s\n", u);
    }
    fclose(f);
}