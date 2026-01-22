#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <discord_rpc.h>

void _ready(const DiscordUser* connected_user_ptr) { return; }
void _disconnected(int errorc, const char* errorcontent_ptr) { return; }

#define _ldrpc_usage "Usage: %s --appid <id> [other options]\n"
#define _ldrpc_appidrequired "Error: --appid is required\n"
#define _ldrpc_requiresvalue "Error: %s requires a value\n"

#define _ldrpc_unkownargument "Warning: Unkown argument '%s'\n"

enum _ldrpc_argt { _ldrpc_ae_type_string, _ldrpc_ae_int, _ldrpc_ae_int64 };
struct _ldrpc_argo {
    const char*      arg;
    enum _ldrpc_argt type;
    void*            field;
};

int _drp_constructor(int argc, char* argv_ptr[], DiscordRichPresence* dr_presence_ptr, const char** appid_ptr_ptr) {
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv_ptr[i], "--appid") == 0) {
            if((i+1) < argc) *appid_ptr_ptr = argv_ptr[++i];
            else {
                fprintf(stderr, _ldrpc_requiresvalue, "--appid"); return(1);
            } continue;
        }

        struct _ldrpc_argo options[] = {
            { "--state",            _ldrpc_ae_type_string,  &dr_presence_ptr->state },
            { "--details",          _ldrpc_ae_type_string,  &dr_presence_ptr->details },
            { "--start-timestamp",  _ldrpc_ae_int64,        &dr_presence_ptr->startTimestamp },
            { "--end-timestamp",    _ldrpc_ae_int64,        &dr_presence_ptr->endTimestamp },
            { "--large-image-key",  _ldrpc_ae_type_string,  &dr_presence_ptr->largeImageKey },
            { "--large-image-text", _ldrpc_ae_type_string,  &dr_presence_ptr->largeImageText },
            { "--small-image-key",  _ldrpc_ae_type_string,  &dr_presence_ptr->smallImageKey },
            { "--small-image-text", _ldrpc_ae_type_string,  &dr_presence_ptr->smallImageText },
            { "--party-size",       _ldrpc_ae_int,          &dr_presence_ptr->partySize },
            { "--party-max",        _ldrpc_ae_int,          &dr_presence_ptr->partyMax },
        };

        int status;
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
                } status = 1; break;
            }
        }
        if(!status) {
            fprintf(stderr, _ldrpc_unkownargument, argv_ptr[i]);
        }
    } return(0);
}

int main(int argc, char* argv_ptr[]) {
    if(argc < 3) {
        printf(_ldrpc_usage, argv_ptr[0]);
        return(1);
    }

    const char* appid_ptr = NULL;
    DiscordRichPresence* dr_presence_ptr = (DiscordRichPresence*)malloc(sizeof(DiscordRichPresence));

    if(_drp_constructor(argc, argv_ptr, dr_presence_ptr, &appid_ptr)) { return(1); }

    if(!appid_ptr) {
        fprintf(stderr, _ldrpc_appidrequired);
        return(1);
    }

    DiscordEventHandlers* de_handlers_ptr = (DiscordEventHandlers*)malloc(sizeof(DiscordEventHandlers));
    de_handlers_ptr->ready = _ready; // todo: _disconnected implement
    Discord_Initialize(appid_ptr, de_handlers_ptr, 1, NULL);

    Discord_UpdatePresence(dr_presence_ptr);

    while(1) {
        Discord_RunCallbacks(); sleep(2);
    } free(dr_presence_ptr); Discord_Shutdown(); free(de_handlers_ptr);

    return(0);
}
