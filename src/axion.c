#include "axion.h"
#include "cJSON.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define BASE_URL "https://api.axionquant.com"

// Opaque struct defined in the header
struct AxionClient {
    char *api_key;
    CURL *curl_handle;
};

// Response struct
struct AxionResponse {
    int http_status;
    char *data;
    cJSON *json;
    char *error;
};

// Struct to hold the response from curl
typedef struct {
    char *memory;
    size_t size;
} MemoryStruct;

// Callback function for curl to write received data into a buffer
static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        fprintf(stderr, "error: not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Internal function to perform requests
static AxionResponse* _axion_request(AxionClient *client, const char *path, const char *query_params) {
    if (!client || !client->curl_handle) {
        fprintf(stderr, "error: client not initialized.\n");
        return NULL;
    }

    CURL *curl = client->curl_handle;
    CURLcode res;

    MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    // Construct full URL
    char full_url[2048];
    if (query_params && strlen(query_params) > 0) {
        snprintf(full_url, sizeof(full_url), "%s/%s?%s", BASE_URL, path, query_params);
    } else {
        snprintf(full_url, sizeof(full_url), "%s/%s", BASE_URL, path);
    }

    AxionResponse *response = malloc(sizeof(AxionResponse));
    if (!response) return NULL;
    response->http_status = 0;
    response->data = NULL;
    response->json = NULL;
    response->error = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, full_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "axion-c-client/1.0");

    struct curl_slist *headers = NULL;
    if (client->api_key) {
        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", client->api_key);
        headers = curl_slist_append(headers, auth_header);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        response->error = strdup(curl_easy_strerror(res));
    } else {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        response->http_status = (int)http_code;
        response->data = chunk.memory;

        if (http_code >= 400) {
            cJSON *error_json = cJSON_Parse(response->data);
            if (error_json) {
                cJSON *message = cJSON_GetObjectItemCaseSensitive(error_json, "message");
                if (cJSON_IsString(message) && (message->valuestring != NULL)) {
                    response->error = strdup(message->valuestring);
                } else {
                    response->error = strdup("An unknown HTTP error occurred.");
                }
                cJSON_Delete(error_json);
            } else {
                 response->error = strdup("An unknown HTTP error occurred (failed to parse error response).");
            }
        } else {
             response->json = cJSON_Parse(response->data);
             if (!response->json && response->data && strlen(response->data) > 0) {
                 response->error = strdup("Failed to parse JSON response.");
             }
        }
    }

    if (headers) curl_slist_free_all(headers);
    if (response->error && response->data) {
        free(chunk.memory);
        response->data = NULL;
    } else if (!response->data) {
        free(chunk.memory);
    }

    return response;
}

// ---------------------------------------------------------------------
// Improved query builder - dynamically allocates exact needed memory
// ---------------------------------------------------------------------
static char* _build_query(const char **keys, const char **values, int count) {
    if (count == 0) return NULL;

    size_t needed = 1; // null terminator
    int i;
    for (i = 0; i < count; i++) {
        if (values[i] != NULL) {
            needed += strlen(keys[i]) + strlen(values[i]) + 2; // '=' and '&'
        }
    }

    char *query = malloc(needed);
    if (!query) return NULL;

    size_t offset = 0;
    for (i = 0; i < count; i++) {
        if (values[i] != NULL) {
            offset += snprintf(query + offset, needed - offset, "%s=%s&", keys[i], values[i]);
        }
    }

    if (offset > 0) {
        query[offset - 1] = '\0'; // remove trailing '&'
    } else {
        query[0] = '\0';
    }
    return query;
}

// ---------------------------------------------------------------------
// Client lifecycle
// ---------------------------------------------------------------------
AxionClient* axion_init(const char *api_key) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "error: curl_easy_init() failed.\n");
        return NULL;
    }

    AxionClient *client = malloc(sizeof(AxionClient));
    if (!client) {
        fprintf(stderr, "error: failed to allocate memory for client.\n");
        curl_easy_cleanup(curl);
        return NULL;
    }

    client->api_key = api_key ? strdup(api_key) : NULL;
    client->curl_handle = curl;
    return client;
}

void axion_client(AxionClient *client) {
    if (!client) return;
    if (client->api_key) free(client->api_key);
    if (client->curl_handle) curl_easy_cleanup(client->curl_handle);
    free(client);
    curl_global_cleanup();
}

void axion_response(AxionResponse *response) {
    if (!response) return;
    if (response->data) free(response->data);
    if (response->json) cJSON_Delete(response->json);
    if (response->error) free(response->error);
    free(response);
}

// =====================================================================
// CREDIT API
// =====================================================================
AxionResponse* axion_credit_search(AxionClient *client, const char *query) {
    char *q = _build_query((const char*[]){"query"}, (const char*[]){query}, 1);
    AxionResponse *resp = _axion_request(client, "credit/search", q);
    free(q);
    return resp;
}

AxionResponse* axion_credit_ratings(AxionClient *client, const char *entity_id) {
    char path[256];
    snprintf(path, sizeof(path), "credit/ratings/%s", entity_id);
    return _axion_request(client, path, NULL);
}

// =====================================================================
// ESG API
// =====================================================================
AxionResponse* axion_esg_data(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "esg/%s", ticker);
    return _axion_request(client, path, NULL);
}

// =====================================================================
// ETF API
// =====================================================================
AxionResponse* axion_etfs_fund(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "etfs/%s/fund", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_etfs_holdings(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "etfs/%s/holdings", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_etfs_exposure(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "etfs/%s/exposure", ticker);
    return _axion_request(client, path, NULL);
}

// =====================================================================
// SUPPLY CHAIN API
// =====================================================================
AxionResponse* axion_supply_chain_customers(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "supply-chain/%s/customers", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_supply_chain_peers(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "supply-chain/%s/peers", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_supply_chain_suppliers(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "supply-chain/%s/suppliers", ticker);
    return _axion_request(client, path, NULL);
}

// =====================================================================
// STOCKS API
// =====================================================================
AxionResponse* axion_stocks_tickers(AxionClient *client, const char *country, const char *exchange) {
    const char *keys[] = {"country", "exchange"};
    const char *values[] = {country, exchange};
    char *query = _build_query(keys, values, 2);
    AxionResponse *resp = _axion_request(client, "stocks/tickers", query);
    free(query);
    return resp;
}

AxionResponse* axion_stocks_ticker(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "stocks/%s", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_stocks_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame) {
    const char *keys[] = {"from", "to", "frame"};
    const char *values[] = {from_date, to_date, frame};
    char *query = _build_query(keys, values, 3);
    char path[256];
    snprintf(path, sizeof(path), "stocks/%s/prices", ticker);
    AxionResponse *resp = _axion_request(client, path, query);
    free(query);
    return resp;
}

// =====================================================================
// CRYPTO API
// =====================================================================
AxionResponse* axion_crypto_tickers(AxionClient *client, const char *type) {
    const char *keys[] = {"type"};
    const char *values[] = {type};
    char *query = _build_query(keys, values, 1);
    AxionResponse *resp = _axion_request(client, "crypto/tickers", query);
    free(query);
    return resp;
}

AxionResponse* axion_crypto_ticker(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "crypto/%s", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_crypto_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame) {
    const char *keys[] = {"from", "to", "frame"};
    const char *values[] = {from_date, to_date, frame};
    char *query = _build_query(keys, values, 3);
    char path[256];
    snprintf(path, sizeof(path), "crypto/%s/prices", ticker);
    AxionResponse *resp = _axion_request(client, path, query);
    free(query);
    return resp;
}

// =====================================================================
// FOREX API
// =====================================================================
AxionResponse* axion_forex_tickers(AxionClient *client, const char *country, const char *exchange) {
    const char *keys[] = {"country", "exchange"};
    const char *values[] = {country, exchange};
    char *query = _build_query(keys, values, 2);
    AxionResponse *resp = _axion_request(client, "forex/tickers", query);
    free(query);
    return resp;
}

AxionResponse* axion_forex_ticker(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "forex/%s", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_forex_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame) {
    const char *keys[] = {"from", "to", "frame"};
    const char *values[] = {from_date, to_date, frame};
    char *query = _build_query(keys, values, 3);
    char path[256];
    snprintf(path, sizeof(path), "forex/%s/prices", ticker);
    AxionResponse *resp = _axion_request(client, path, query);
    free(query);
    return resp;
}

// =====================================================================
// FUTURES API
// =====================================================================
AxionResponse* axion_futures_tickers(AxionClient *client, const char *exchange) {
    const char *keys[] = {"exchange"};
    const char *values[] = {exchange};
    char *query = _build_query(keys, values, 1);
    AxionResponse *resp = _axion_request(client, "futures/tickers", query);
    free(query);
    return resp;
}

AxionResponse* axion_futures_ticker(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "futures/%s", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_futures_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame) {
    const char *keys[] = {"from", "to", "frame"};
    const char *values[] = {from_date, to_date, frame};
    char *query = _build_query(keys, values, 3);
    char path[256];
    snprintf(path, sizeof(path), "futures/%s/prices", ticker);
    AxionResponse *resp = _axion_request(client, path, query);
    free(query);
    return resp;
}

// =====================================================================
// INDICES API
// =====================================================================
AxionResponse* axion_indices_tickers(AxionClient *client, const char *exchange) {
    const char *keys[] = {"exchange"};
    const char *values[] = {exchange};
    char *query = _build_query(keys, values, 1);
    AxionResponse *resp = _axion_request(client, "indices/tickers", query);
    free(query);
    return resp;
}

AxionResponse* axion_indices_ticker(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "indices/%s", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_indices_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame) {
    const char *keys[] = {"from", "to", "frame"};
    const char *values[] = {from_date, to_date, frame};
    char *query = _build_query(keys, values, 3);
    char path[256];
    snprintf(path, sizeof(path), "indices/%s/prices", ticker);
    AxionResponse *resp = _axion_request(client, path, query);
    free(query);
    return resp;
}

// =====================================================================
// ECONOMIC API
// =====================================================================
AxionResponse* axion_econ_search(AxionClient *client, const char *query) {
    char *q = _build_query((const char*[]){"query"}, (const char*[]){query}, 1);
    AxionResponse *resp = _axion_request(client, "econ/search", q);
    free(q);
    return resp;
}

AxionResponse* axion_econ_dataset(AxionClient *client, const char *series_id) {
    char path[256];
    snprintf(path, sizeof(path), "econ/dataset/%s", series_id);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_econ_calendar(AxionClient *client,
                                   const char *from_date,
                                   const char *to_date,
                                   const char *country,
                                   int min_importance,
                                   const char *currency,
                                   const char *category) {
    const char *keys[6];
    const char *values[6];
    int count = 0;

    if (from_date) { keys[count] = "from"; values[count++] = from_date; }
    if (to_date)   { keys[count] = "to";   values[count++] = to_date;   }
    if (country)   { keys[count] = "country"; values[count++] = country; }
    if (min_importance >= 0) {
        char *imp_str = malloc(32);
        snprintf(imp_str, 32, "%d", min_importance);
        keys[count] = "minImportance";
        values[count++] = imp_str;  // will be freed later
    }
    if (currency)  { keys[count] = "currency"; values[count++] = currency; }
    if (category)  { keys[count] = "category"; values[count++] = category; }

    char *query = NULL;
    if (count > 0) {
        query = _build_query(keys, values, count);
    }
    // free the temporary string for min_importance
    if (min_importance >= 0) {
        free((char*)values[2]);
    }

    AxionResponse *resp = _axion_request(client, "econ/calendar", query);
    free(query);
    return resp;
}

// =====================================================================
// NEWS API
// =====================================================================
AxionResponse* axion_news_general(AxionClient *client) {
    return _axion_request(client, "news", NULL);
}

AxionResponse* axion_news_company(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "news/%s", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_news_country(AxionClient *client, const char *country) {
    char path[256];
    snprintf(path, sizeof(path), "news/country/%s", country);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_news_category(AxionClient *client, const char *category) {
    char path[256];
    snprintf(path, sizeof(path), "news/category/%s", category);
    return _axion_request(client, path, NULL);
}

// =====================================================================
// SENTIMENT API
// =====================================================================
AxionResponse* axion_sentiment_all(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "sentiment/%s/all", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_sentiment_social(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "sentiment/%s/social", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_sentiment_news(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "sentiment/%s/news", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_sentiment_analyst(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "sentiment/%s/analyst", ticker);
    return _axion_request(client, path, NULL);
}

// =====================================================================
// PROFILES API
// =====================================================================
AxionResponse* axion_profiles_profile(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_recommendation(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/recommendation", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_statistics(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/statistics", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_summary(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/summary", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_calendar(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/calendar", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_info(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/info", ticker);
    return _axion_request(client, path, NULL);
}

// =====================================================================
// EARNINGS API
// =====================================================================
AxionResponse* axion_earnings_history(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "earnings/%s/history", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_earnings_trend(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "earnings/%s/trend", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_earnings_index(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "earnings/%s/index", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_earnings_report(AxionClient *client, const char *ticker, const char *year, const char *quarter) {
    const char *keys[] = {"year", "quarter"};
    const char *values[] = {year, quarter};
    char *query = _build_query(keys, values, 2);
    char path[256];
    snprintf(path, sizeof(path), "earnings/%s/report", ticker);
    AxionResponse *resp = _axion_request(client, path, query);
    free(query);
    return resp;
}

// =====================================================================
// FILINGS API
// =====================================================================
AxionResponse* axion_filings_filings(AxionClient *client, const char *ticker, int limit, const char *form) {
    const char *keys[2];
    const char *values[2];
    int count = 0;
    char limit_str[32];
    if (limit > 0) {
        snprintf(limit_str, sizeof(limit_str), "%d", limit);
        keys[count] = "limit";
        values[count++] = limit_str;
    }
    if (form) {
        keys[count] = "form";
        values[count++] = form;
    }
    char *query = (count > 0) ? _build_query(keys, values, count) : NULL;
    char path[256];
    snprintf(path, sizeof(path), "filings/%s", ticker);
    AxionResponse *resp = _axion_request(client, path, query);
    free(query);
    return resp;
}

AxionResponse* axion_filings_forms(AxionClient *client, const char *ticker, const char *form_type,
                                    const char *year, const char *quarter, int limit) {
    const char *keys[3];
    const char *values[3];
    int count = 0;
    char limit_str[32];
    if (year)    { keys[count] = "year";   values[count++] = year; }
    if (quarter) { keys[count] = "quarter"; values[count++] = quarter; }
    if (limit > 0) {
        snprintf(limit_str, sizeof(limit_str), "%d", limit);
        keys[count] = "limit";
        values[count++] = limit_str;
    }
    char *query = (count > 0) ? _build_query(keys, values, count) : NULL;
    char path[256];
    snprintf(path, sizeof(path), "filings/%s/forms/%s", ticker, form_type);
    AxionResponse *resp = _axion_request(client, path, query);
    free(query);
    return resp;
}

AxionResponse* axion_filings_desc_forms(AxionClient *client) {
    return _axion_request(client, "filings/desc/forms", NULL);
}

AxionResponse* axion_filings_search(AxionClient *client,
                                    const char *year, const char *quarter,
                                    const char *form, const char *ticker) {
    const char *keys[4];
    const char *values[4];
    int count = 0;
    if (year)    { keys[count] = "year";    values[count++] = year; }
    if (quarter) { keys[count] = "quarter"; values[count++] = quarter; }
    if (form)    { keys[count] = "form";    values[count++] = form; }
    if (ticker)  { keys[count] = "ticker";  values[count++] = ticker; }
    char *query = (count > 0) ? _build_query(keys, values, count) : NULL;
    AxionResponse *resp = _axion_request(client, "filings/search", query);
    free(query);
    return resp;
}

// =====================================================================
// FINANCIALS API
// =====================================================================
static AxionResponse* _financials_request(AxionClient *client, const char *ticker, const char *subpath, int periods) {
    char path[512];
    if (periods > 0) {
        snprintf(path, sizeof(path), "financials/%s/%s?periods=%d", ticker, subpath, periods);
        return _axion_request(client, path, NULL);
    } else {
        snprintf(path, sizeof(path), "financials/%s/%s", ticker, subpath);
        return _axion_request(client, path, NULL);
    }
}

AxionResponse* axion_financials_revenue(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "revenue", periods);
}

AxionResponse* axion_financials_net_income(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "netincome", periods);
}

AxionResponse* axion_financials_total_assets(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "total/assets", periods);
}

AxionResponse* axion_financials_total_liabilities(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "total/liabilities", periods);
}

AxionResponse* axion_financials_stockholders_equity(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "stockholdersequity", periods);
}

AxionResponse* axion_financials_current_assets(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "current/assets", periods);
}

AxionResponse* axion_financials_current_liabilities(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "current/liabilities", periods);
}

AxionResponse* axion_financials_operating_cash_flow(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "cashflow/operating", periods);
}

AxionResponse* axion_financials_capital_expenditures(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "capitalexpenditures", periods);
}

AxionResponse* axion_financials_free_cash_flow(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "cashflow/free", periods);
}

AxionResponse* axion_financials_shares_outstanding_basic(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "sharesoutstanding/basic", periods);
}

AxionResponse* axion_financials_shares_outstanding_diluted(AxionClient *client, const char *ticker, int periods) {
    return _financials_request(client, ticker, "sharesoutstanding/diluted", periods);
}

AxionResponse* axion_financials_metrics(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "financials/%s/metrics", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_financials_snapshot(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "financials/%s/snapshot", ticker);
    return _axion_request(client, path, NULL);
}

// =====================================================================
// INSIDERS API
// =====================================================================
AxionResponse* axion_insiders_funds(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "insiders/%s/funds", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_insiders_individuals(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "insiders/%s/individuals", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_insiders_institutions(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "insiders/%s/institutions", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_insiders_ownership(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "insiders/%s/ownership", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_insiders_activity(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "insiders/%s/activity", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_insiders_transactions(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "insiders/%s/transactions", ticker);
    return _axion_request(client, path, NULL);
}

// =====================================================================
// WEB TRAFFIC API
// =====================================================================
AxionResponse* axion_webtraffic_traffic(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "webtraffic/%s/traffic", ticker);
    return _axion_request(client, path, NULL);
}
