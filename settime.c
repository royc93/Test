#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <curl/curl.h>
#include <curl/easy.h>

int dbg_flag = 1;

struct _TimeZone
{
    char *md_index;
    char *md_desc;
    char *nas_index;
    char *nas_desc;
};

static struct _TimeZone timezone_tab[] = {
    {"01", "(GMT-12:00) International Date Line West", "01", "(GMT-12:00) International Data Line West"},
    {"02", "(GMT-11:00) Midway Island, Samoa", "02", "(GMT-11:00) Midway Island, Samoa"},
    {"03", "(GMT-10:00) Hawaii", "03", "(GMT-10:00) Hawaii"},
    {"04", "(GMT-09:00) Alaska", "04", "(GMT-09:00) Alaska"},
    {"05", "(GMT-08:00) Pacific Time (US & Canada)", "05", "(GMT-08:00) Pacific Time (US & Canada);Tijuana"},
    {"06", "(GMT-08:00) Tijuana, Baja California", "05", "(GMT-08:00) Pacific Time (US & Canada);Tijuana"},
    {"07", "(GMT-07:00) Arizona", "06", "(GMT-07:00) Arizona"},
    {"08", "(GMT-07:00) Chihuahua, La Paz, Mazatlan", "07", "(GMT-07:00) Chihuahua, La Paz, Mazatlan"},
    {"09", "(GMT-07:00) Mountain Time (US & Canada)", "08", "(GMT-07:00) Mountain Time (US & Canada)"},
    {"10", "(GMT-06:00) Central America", "09", "(GMT-06:00) Central America, Saskatchewan"},
    //11
    {"11", "(GMT-06:00) Central Time (US & Canada)", "10", "(GMT-06:00) Central Time (US & Canada)"},
    {"12", "(GMT-06:00) Guadalajara, Mexico City, Monterrey", "11", "(GMT-06:00) Guadalajara, Mexico City, Monterrey"},
    {"13", "(GMT-06:00) Saskatchewan", "11", "(GMT-06:00) Guadalajara, Mexico City, Monterrey"},
    {"14", "(GMT-05:00) Bogota, Lima, Quito, Rio Branco", "12", "(GMT-05:00) Bogota, Lima, Quito"},
    {"15", "(GMT-05:00) Eastern Time (US & Canada)", "13", "(GMT-05:00) Eastern Time (US & Canada)"},
    {"16", "(GMT-05:00) Indiana (East)", "14", "(GMT-05:00) Indiana (East)"},
    {"17", "(GMT-04:30) Caracas", "16", "(GMT-04:00) Caracas, La Paz"},
    {"18", "(GMT-04:00) Atlantic Time (Canada)", "15", "(GMT-04:00) Atlantic Time (Canada)"},
    {"19", "(GMT-04:00) La Paz, Georgetown", "16", "(GMT-04:00) Caracas, La Paz"},
    {"20", "(GMT-04:00) Manaus", "16", "(GMT-04:00) Caracas, La Paz"}, //no this timezone

    {"21", "(GMT-04:00) Santiago", "17", "(GMT-04:00) Santiago"},
    {"22", "(GMT-03:30) Newfoundland", "18", "(GMT-03:30) Newfoundland"},
    {"23", "(GMT-03:00) Brasilia", "19", "(GMT-03:00) Brasilia, Buenos Aires, Georgetown"},
    {"24", "(GMT-03:00) Buenos Aires", "19", "(GMT-03:00) Brasilia, Buenos Aires, Georgetown"},
    {"25", "(GMT-03:00) Greenland", "20", "(GMT-03:00) Greenland"},
    {"26", "(GMT-03:00) Montevideo", "20", "(GMT-03:00) Greenland"}, //no this timezone
    {"27", "(GMT-02:00) Mid-Atlantic", "21", "(GMT-02:00) Mid-Atlantic"},
    {"28", "(GMT-01:00) Azores", "22", "(GMT-01:00) Azores"},
    {"29", "(GMT-01:00) Cape Verde Is.", "23", "(GMT-01:00) Cape Verde Is."},
    {"30", "(GMT+00:00) Casablanca, Monrovia, Reykjavik", "24", "GMT Casablanca, Monrovia"},

    {"31", "(GMT+00:00) Greenwich Mean Time : Dublin, Edinburgh, Lisbon, London", "25", "GMT Greenwich Mean Time : Dublin, Edinburgh, Lisbon, London"},
    {"32", "(GMT+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna", "26", "(GMT+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna"},
    {"33", "(GMT+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague", "27", "(GMT+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague"},
    {"34", "(GMT+01:00) Brussels, Copenhagen, Madrid, Paris", "28", "(GMT+01:00) Brussels, Copenhagen, Madrid, Paris"},
    {"35", "(GMT+01:00) Sarajevo, Skopje, Warsaw, Zagreb", "29", "(GMT+01:00) Sarajevo, Skopje, Warsaw, Zagreb"},

    {"36", "(GMT+01:00) West Central Africa", "30", "(GMT+01:00) West Central Africa"},
    {"37", "(GMT+02:00) Amman", "31", "(GMT+02:00) Athens, Istanbul, Minsk, Bucharest"}, //not this timezone
    {"38", "(GMT+02:00) Athens, Bucharest, Istanbul", "31", "(GMT+02:00) Athens, Istanbul, Minsk, Bucharest"},
    {"39", "(GMT+02:00) Beirut", "32", "(GMT+02:00) Cairo"}, //no this timezone 4:30
    {"40", "(GMT+02:00) Cairo", "32", "(GMT+02:00) Cairo"},

    {"41", "(GMT+02:00) Harare, Pretoria", "33", "(GMT+02:00) Harare, Pretoria, Jerusalem"},
    {"42", "(GMT+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius", "34", "(GMT+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius"},
    {"43", "(GMT+02:00) Jerusalem", "33", "(GMT+02:00) Harare, Pretoria, Jerusalem"},
    {"44", "(GMT+02:00) Minsk", "31", "(GMT+02:00) Athens, Istanbul, Minsk, Bucharest"},
    {"45", "(GMT+02:00) Windhoek", "34", "(GMT+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius"},

    {"46", "(GMT+03:00) Baghdad", "35", "(GMT+03:00) Baghdad, Kuwait, Riyadh"},
    {"47", "(GMT+03:00) Kuwait, Riyadh", "35", "(GMT+03:00) Baghdad, Kuwait, Riyadh"},
    {"48", "(GMT+03:00) Moscow, St. Petersburg, Volgograd", "36", "(GMT+03:00) Moscow, St. Petersburg, Volgograd"},
    {"49", "(GMT+03:00) Nairobi", "37", "(GMT+03:00) Nairobi, Tehran"},
    {"50", "(GMT+03:00) Tbilisi", "39", "(GMT+04:00) Baku, Tbilisi, Yerevan"},

    {"51", "(GMT+03:30) Tehran", "37", "(GMT+03:00) Nairobi, Tehran"},
    {"52", "(GMT+04:00) Abu Dhabi, Muscat", "38", "GMT+04:00) Abu Dhabi, Muscat, Kabul"},
    {"53", "(GMT+04:00) Baku", "39", "(GMT+04:00) Baku, Tbilisi, Yerevan"},
    {"54", "(GMT+04:00) Yerevan", "39", "(GMT+04:00) Baku, Tbilisi, Yerevan"},
    {"55", "(GMT+04:30) Kabul", "39", "(GMT+04:00) Baku, Tbilisi, Yerevan"}, //no this timezone 4:30

    {"56", "(GMT+05:00) Ekaterinburg", "40", "(GMT+05:00) Ekaterinburg"},
    {"57", "(GMT+05:00) Islamabad, Karachi, Tashkent", "41", "(GMT+05:00) Islamabad, Karachi, Tashkent"},
    {"58", "(GMT+05:30) Chennai, Kolkata, Mumbai, New Delhi", "42", "(GMT+05:30) Chennai, Kolkata, Mumbai, New Delhi"},
    {"59", "(GMT+05:30) Sri Jayawardenepura", "45", "GMT+06:00) Astana, Dhaka, Sri Jayawardenepura"},
    {"60", "(GMT+05:45) Kathmandu", "43", "(GMT+05:45) Kathmandu"},

    {"61", "(GMT+06:00) Almaty, Novosibirsk", "44", "(GMT+06:00) Almaty, Novosibirsk"},
    {"62", "(GMT+06:00) Astana, Dhaka", "45", "(GMT+06:00) Astana, Dhaka, Sri Jayawardenepura"},
    {"63", "(GMT+06:30) Yangon (Rangoon)", "46", "(GMT+06:30) Rangoon"},
    {"64", "(GMT+07:00) Bangkok, Hanoi, Jakarta", "47", "(GMT+07:00) Bangkok, Hanoi, Jakarta"},
    {"65", "(GMT+07:00) Krasnoyarsk", "48", "GMT+07:00) Krasnoyarsk"},

    {"66", "(GMT+08:00) Beijing, Chongqing, Hong Kong, Urumqi", "47", "(GMT+08:00) Beijing, Chongqing, Hong Kong, Taipei"},
    {"67", "(GMT+08:00) Irkutsk, Ulaan Bataar", "50", "(GMT+08:00) Irkutsk, Ulaan Bataar"},
    {"68", "(GMT+08:00) Kuala Lumpur, Singapore", "51", "(GMT+08:00) Kuala Lumpur, Singapore, Perth"},
    {"69", "(GMT+08:00) Perth", "51", "(GMT+08:00) Kuala Lumpur, Singapore, Perth"},
    {"70", "(GMT+08:00) Taipei", "49", "(GMT+08:00) Beijing, Chongqing, Hong Kong, Taipei"},

    {"71", "(GMT+09:00) Osaka, Sapporo, Tokyo", "52", "GMT+09:00) Osaka, Sapporo, Tokyo, Seoul"},
    {"72", "(GMT+09:00) Seoul", "52", "(GMT+09:00) Osaka, Sapporo, Tokyo, Seoul"},
    {"73", "(GMT+09:00) Yakutsk", "53", "(GMT+09:00) Yakutsk"},
    {"74", "(GMT+09:30) Adelaide", "54", "(GMT+09:30) Adelaide"},
    {"75", "(GMT+09:30) Darwin", "55", "(GMT+09:30) Darwin"},

    {"76", "(GMT+10:00) Brisbane", "56", "(GMT+10:00) Brisbane, Guam, Port Moresby"},
    {"77", "(GMT+10:00) Canberra, Melbourne, Sydney", "57", "(GMT+10:00) Canberra, Melbourne, Sydney"},
    {"78", "(GMT+10:00) Guam, Port Moresby", "56", "(GMT+10:00) Brisbane, Guam, Port Moresby"},
    {"79", "(GMT+10:00) Hobart", "58", "(GMT+10:00) Hobart"},
    {"80", "(GMT+10:00) Vladivostok", "59", "(GMT+10:00) Vladivostok"},

    {"81", "(GMT+11:00) Magadan, Solomon Is., New Caledonia", "60", "(GMT+11:00) Magadan, Solomon Is., New Caledonia"},
    {"82", "(GMT+12:00) Auckland, Wellington", "61", "(GMT+12:00) Auckland, Wellington'"},
    {"83", "(GMT+12:00) Fiji, Kamchatka, Marshall Is.", "62", "GMT+12:00) Fiji, Kamchatka, Marshall Is."},
    {"84", "(GMT+13:00) Nuku'alofa", "63", "(GMT+13:00) Nuku'alofa"},

    {NULL, NULL}};

void cgi_curl_tx_rx(char *url, char *buf)
{
    CURL *curl;
    CURLcode res;

    //initalize
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl == NULL)
    {
        if (dbg_flag == 1)
            printf("curl == NULL\n");
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    res = curl_easy_perform(curl);

    //always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (res != CURLE_OK && res != CURLE_OPERATION_TIMEDOUT)
    {
        if (dbg_flag == 1)
            printf("###ERROR: curl send fail.\n");
        return;
    }
    else
    {
        if (dbg_flag == 1)
            printf("###ok [%s]\n", "abc");
        return;
    }
}

int main(int argc, char **argv)
{
    char buf[128] = {0};
    char url[64] = {0};
    int i;

    if (argc < 8)
        return 1;
    //timezone
    for (i = 0; timezone_tab[i].md_index != NULL; i++)
    {
        if (strcmp(argv[1], timezone_tab[i].md_index) == 0)
        {
            strcpy(url, "http://127.0.0.1/cgi-bin/system_mgr.cgi");
            sprintf(buf, "cmd=cgi_timezone&f_timezone=%s", timezone_tab[i].nas_index);
            cgi_curl_tx_rx(url, buf);
        }
    }

    //time
    strcpy(url, "http://127.0.0.1/cgi-bin/system_mgr.cgi");
    sprintf(buf, "cmd=cgi_manual_time&f_year=%s&f_month=%s&f_day=%s&f_hour=%s&f_min=%s&f_sec=%s", argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);

    cgi_curl_tx_rx(url, buf);

    return 0;
}
