#include "sun_data.h"

#define APPMSG_INBOX_SIZE   256
#define APPMSG_OUTBOX_SIZE   64

#define DEFAULT_SUNRISE  (6 * 60)
#define DEFAULT_SUNSET  (18 * 60)

static int             s_sunrise_minutes = DEFAULT_SUNRISE;
static int             s_sunset_minutes  = DEFAULT_SUNSET;
static SunDataCallback s_callback        = NULL;

static void clamp_sun_times(void) {
    if (s_sunrise_minutes < 0)
        s_sunrise_minutes = 0;
    if (s_sunset_minutes > MINUTES_PER_DAY - 1)
        s_sunset_minutes = MINUTES_PER_DAY - 1;
    if (s_sunset_minutes <= s_sunrise_minutes)
        s_sunset_minutes = s_sunrise_minutes + 60;
}

static void inbox_received(DictionaryIterator *iterator, void *context) {
    Tuple *rh = dict_find(iterator, MESSAGE_KEY_SUNRISE_HOUR);
    Tuple *rm = dict_find(iterator, MESSAGE_KEY_SUNRISE_MIN);
    Tuple *sh = dict_find(iterator, MESSAGE_KEY_SUNSET_HOUR);
    Tuple *sm = dict_find(iterator, MESSAGE_KEY_SUNSET_MIN);

    if (!(rh && rm && sh && sm)) return;

    s_sunrise_minutes = (int)rh->value->int32 * 60 + (int)rm->value->int32;
    s_sunset_minutes  = (int)sh->value->int32 * 60 + (int)sm->value->int32;
    clamp_sun_times();

    APP_LOG(APP_LOG_LEVEL_INFO, "Sun: rise=%d:%02d set=%d:%02d",
            s_sunrise_minutes / 60, s_sunrise_minutes % 60,
            s_sunset_minutes  / 60, s_sunset_minutes  % 60);

    if (s_callback) {
        s_callback(s_sunrise_minutes, s_sunset_minutes);
    }
}

static void inbox_dropped(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Inbox dropped: %d", reason);
}

static void outbox_failed(DictionaryIterator *iter,
                          AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox failed: %d", reason);
}

static void outbox_sent(DictionaryIterator *iter, void *context) {
    (void)iter; (void)context;
}

void sun_data_init(SunDataCallback on_update) {
    s_callback = on_update;

    app_message_register_inbox_received(inbox_received);
    app_message_register_inbox_dropped(inbox_dropped);
    app_message_register_outbox_failed(outbox_failed);
    app_message_register_outbox_sent(outbox_sent);
    app_message_open(APPMSG_INBOX_SIZE, APPMSG_OUTBOX_SIZE);
}

void sun_data_deinit(void) {
    s_callback = NULL;
}

int sun_data_get_sunrise(void) {
    return s_sunrise_minutes;
}

int sun_data_get_sunset(void) {
    return s_sunset_minutes;
}

void sun_data_request_update(void) {
    DictionaryIterator *iter;
    if (app_message_outbox_begin(&iter) == APP_MSG_OK) {
        dict_write_uint8(iter, MESSAGE_KEY_REQUEST_SUN_DATA, 1);
        app_message_outbox_send();
    }
}
