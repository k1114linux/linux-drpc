#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <discord_rpc.h>

void _ready(const DiscordUser* connected_user_ptr) { return; }
void _disconnected(int errorc, const char* errorcontent_ptr) { return; }

#define _ldrpc_usage "Usage: %s --appid <id> [other options]\n"
#define _ldrpc_describe(type) \
    "\t%-10s %-20s %s\n", \
    ((type) == _ldrpc_ae_type_string ? "string" : \
     (type) == _ldrpc_ae_int         ? "int"    : \
     (type) == _ldrpc_ae_int64       ? "int64"  : \
                                      "unknown")

#define _ldrpc_appidrequired "Error: --appid is required\n"
#define _ldrpc_requiresvalue "Error: %s requires a value\n"

#define _ldrpc_unkownargument "Warning: Unknown argument '%s'\n"

enum _ldrpc_argt { _ldrpc_ae_type_string, _ldrpc_ae_int, _ldrpc_ae_int64 };
struct _ldrpc_argo {
    const char*      arg;
    const char*      help;
    enum _ldrpc_argt type;
    void*            field;
};

void _ldrpc_print_help(const char* prog, struct _ldrpc_argo* options_ptr, size_t options_len) {
    printf(_ldrpc_usage, prog);

    printf("\nRequired:\n");
    printf(_ldrpc_describe(_ldrpc_ae_int64), "--appid", "Discord application ID");

    printf("\nOptions:\n");
    for(size_t i = 0; i < options_len; i++) {
        printf(_ldrpc_describe(options_ptr[i].type), options_ptr[i].arg, options_ptr[i].help);
    }

    printf("\nMisc:\n");
    printf(_ldrpc_describe(-1), "--help", "Show this help and exit");
}


int _drp_constructor(int argc, char* argv_ptr[], DiscordRichPresence* dr_presence_ptr, const char** appid_ptr_ptr) {
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv_ptr[i], "--appid") == 0) {
            if((i+1) < argc) *appid_ptr_ptr = argv_ptr[++i];
            else {
                fprintf(stderr, _ldrpc_requiresvalue, "--appid"); return(1);
            } continue;
        }

        struct _ldrpc_argo options[] = {
            { "--state",            "Set presence state",       _ldrpc_ae_type_string,  &dr_presence_ptr->state },
            { "--details",          "Set presence details",     _ldrpc_ae_type_string,  &dr_presence_ptr->details },
            { "--start-timestamp",  "Unix start timestamp",     _ldrpc_ae_int64,        &dr_presence_ptr->startTimestamp },
            { "--end-timestamp",    "Unix end timestamp",       _ldrpc_ae_int64,        &dr_presence_ptr->endTimestamp },
            { "--large-image-key",  "Large image asset key",    _ldrpc_ae_type_string,  &dr_presence_ptr->largeImageKey },
            { "--large-image-text", "Large image hover text",   _ldrpc_ae_type_string,  &dr_presence_ptr->largeImageText },
            { "--small-image-key",  "Small image asset key",    _ldrpc_ae_type_string,  &dr_presence_ptr->smallImageKey },
            { "--small-image-text", "Small image hover text",   _ldrpc_ae_type_string,  &dr_presence_ptr->smallImageText },
            { "--party-size",       "Party current size",       _ldrpc_ae_int,          &dr_presence_ptr->partySize },
            { "--party-max",        "Party max size",           _ldrpc_ae_int,          &dr_presence_ptr->partyMax },
        };

        if(strcmp(argv_ptr[i], "--help") == 0 || strcmp(argv_ptr[i], "-h") == 0) {
            _ldrpc_print_help(argv_ptr[0], options, sizeof(options)/sizeof(options[0])); return(1);
        }

        int handled = 0;
        for(size_t j = 0; j < sizeof(options)/sizeof(options[0]); j++) {
            if(strcmp(argv_ptr[i], options[j].arg) == 0) {
                if((i+1) >= argc) {
                    fprintf(stderr, _ldrpc_requiresvalue, argv_ptr[i]);
                    return(1);
                } i++;
                switch(options[j].type) {
                    case _ldrpc_ae_type_string:{
                        *(const char**)options[j].field = argv_ptr[i]; break;
                    } case _ldrpc_ae_int:{
                        *(int*)options[j].field = atoi(argv_ptr[i]); break;
                    } case _ldrpc_ae_int64:{
                        *(int64_t*)options[j].field = atoll(argv_ptr[i]); break;
                    }
                } handled = 1; break;
            }
        }
        if(!handled) {
            fprintf(stderr, _ldrpc_unkownargument, argv_ptr[i]);
        }
    } return(0);
}

volatile sig_atomic_t _ldrpc_lexit = 1;
void _sigint(int sig) { (void)sig; _ldrpc_lexit = 0; }

int main(int argc, char* argv_ptr[]) {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask); 
    sa.sa_flags = 0; sa.sa_handler = _sigint;
    if(sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction"); return(1);
    }

    if(argc < 2) {
        printf(_ldrpc_usage, argv_ptr[0]);
        return(1);
    }

    const char* appid_ptr = NULL;
    DiscordRichPresence* dr_presence_ptr = (DiscordRichPresence*)calloc(1, sizeof(DiscordRichPresence));

    if(_drp_constructor(argc, argv_ptr, dr_presence_ptr, &appid_ptr)) { free(dr_presence_ptr); return(1); }

    if(!appid_ptr) {
        fprintf(stderr, _ldrpc_appidrequired);
        return(1);
    }

    DiscordEventHandlers* de_handlers_ptr = (DiscordEventHandlers*)calloc(1, sizeof(DiscordEventHandlers));
    de_handlers_ptr->ready = _ready; de_handlers_ptr->disconnected = _disconnected; de_handlers_ptr->errored = _disconnected;
    Discord_Initialize(appid_ptr, de_handlers_ptr, 1, NULL);

    Discord_UpdatePresence(dr_presence_ptr);

    while(_ldrpc_lexit) {
        Discord_RunCallbacks(); sleep(2);
    } Discord_Shutdown(); free(dr_presence_ptr); free(de_handlers_ptr);

    return(0);
}
