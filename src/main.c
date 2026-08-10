#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <animate.h>

/* Load the whole file into a freshly allocated buffer. */
static uint8_t *fload(const char *path) {
    FILE *f=fopen(path,"rb");
    if (f==NULL) return NULL;

    /* file size (CP/M rounds it up to the record size) */
    if (fseek(f,0,SEEK_END)!=0) { fclose(f); return NULL; }
    long flen=ftell(f);
    rewind(f);
    if (flen<=0) { fclose(f); return NULL; }

    uint8_t *buf=(uint8_t *)malloc((size_t)flen);
    if (buf==NULL) { fclose(f); return NULL; }

    if (fread(buf,1,(size_t)flen,f)!=(size_t)flen) {
        free(buf); fclose(f); return NULL;
    }

    fclose(f);
    return buf;
}

int main(int argc, char *argv[]) {

    /* must have exactly 1 argument */
    if (argc!=2) {
        printf("Invalid command line arguments.\n");
        printf("Usage: animate <path>\n");
        return 1;
    }

    /* load the file */
    uint8_t *ani=fload(argv[1]);
    if (ani==NULL) {
        printf("File %s could not be read.\n",argv[1]);
        return 2;
    }

    /* recognize file format */
    if (ani[0]!='A'||ani[1]!='N') {
        printf("File %s does not look like an animation file.\n",argv[1]);
        free(ani);
        return 3;
    }

    /* and, finally, animate... */
    animate((animation_t *)ani);

    /* release the file */
    free(ani);
    return 0;
}
