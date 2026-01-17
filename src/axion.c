#include "axion.h"
#include "cJSON.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

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
    chunk.memory = malloc(1); // will be grown by realloc
    chunk.size = 0;

    // Construct full URL
    char full_url[2048];
    if (query_params && strlen(query_params) > 0) {
        snprintf(full_url, sizeof(full_url), "%s/%s?%s", BASE_URL, path, query_params);
    } else {
        snprintf(full_url, sizeof(full_url), "%s/%s", BASE_URL, path);
    }

    // Prepare response struct
    AxionResponse *response = malloc(sizeof(AxionResponse));
    if (!response) return NULL;
    response->http_status = 0;
    response->data = NULL;
    response->json = NULL;
    response->error = NULL;

    // Set curl options
    curl_easy_setopt(curl, CURLOPT_URL, full_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "axion-c-client/1.0");

    // Set auth header
    struct curl_slist *headers = NULL;
    if (client->api_key) {
        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", client->api_key);
        headers = curl_slist_append(headers, auth_header);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Perform the request
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        response->error = strdup(curl_easy_strerror(res));
    } else {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        response->http_status = (int)http_code;
        response->data = chunk.memory; // Transfer ownership of the memory

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

    // Cleanup
    if (headers) {
        curl_slist_free_all(headers);
    }
    // If we are returning an error and have allocated data, free it
    if (response->error && response->data) {
        free(chunk.memory);
        response->data = NULL;
    } else if (!response->data) {
        free(chunk.memory);
    }

    return response;
}

// Helper function to build query string
static char* _build_query(const char **keys, const char **values, int count) {
    if (count == 0) return NULL;

    char *query = malloc(1024);
    if (!query) return NULL;

    int offset = 0;
    for (int i = 0; i < count; i++) {
        if (values[i] != NULL) {
            offset += snprintf(query + offset, 1024 - offset, "%s=%s&", keys[i], values[i]);
        }
    }

    // Remove trailing '&' if it exists
    if (offset > 0) {
        query[offset - 1] = '\0';
    }

    return query;
}

// Client lifecycle functions
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

void axion_free_client(AxionClient *client) {
    if (!client) return;
    if (client->api_key) {
        free(client->api_key);
    }
    if (client->curl_handle) {
        curl_easy_cleanup(client->curl_handle);
    }
    free(client);
    curl_global_cleanup();
}

void axion_free_response(AxionResponse *response) {
    if (!response) return;
    if (response->data) {
        free(response->data);
    }
    if (response->json) {
        cJSON_Delete(response->json);
    }
    if (response->error) {
        free(response->error);
    }
    free(response);
}

// Credit API
AxionResponse* axion_credit_search(AxionClient *client, const char *query) {
    char path[] = "credit/search";
    char full_query[512];
    snprintf(full_query, sizeof(full_query), "query=%s", query);
    return _axion_request(client, path, full_query);
}

AxionResponse* axion_credit_ratings(AxionClient *client, const char *entity_id) {
    char path[256];
    snprintf(path, sizeof(path), "credit/ratings/%s", entity_id);
    return _axion_request(client, path, NULL);
}

// ESG API
AxionResponse* axion_esg_data(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "esg/%s", ticker);
    return _axion_request(client, path, NULL);
}

// ETF API
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

// Supply Chain API
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

// Stocks API
AxionResponse* axion_stocks_tickers(AxionClient *client, const char *country, const char *exchange) {
    const char *keys[] = {"country", "exchange"};
    const char *values[] = {country, exchange};
    char *query = _build_query(keys, values, 2);
    AxionResponse *response = _axion_request(client, "stocks/tickers", query);
    if (query) free(query);
    return response;
}

AxionResponse* axion_stocks_quote(AxionClient *client, const char *ticker) {
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
    AxionResponse *response = _axion_request(client, path, query);
    if (query) free(query);
    return response;
}

// Crypto API
AxionResponse* axion_crypto_tickers(AxionClient *client, const char *type) {
    const char *keys[] = {"type"};
    const char *values[] = {type};
    char *query = _build_query(keys, values, 1);
    AxionResponse *response = _axion_request(client, "crypto/tickers", query);
    if (query) free(query);
    return response;
}

AxionResponse* axion_crypto_quote(AxionClient *client, const char *ticker) {
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
    AxionResponse *response = _axion_request(client, path, query);
    if (query) free(query);
    return response;
}

// Forex API
AxionResponse* axion_forex_tickers(AxionClient *client, const char *country, const char *exchange) {
    const char *keys[] = {"country", "exchange"};
    const char *values[] = {country, exchange};
    char *query = _build_query(keys, values, 2);
    AxionResponse *response = _axion_request(client, "forex/tickers", query);
    if (query) free(query);
    return response;
}

AxionResponse* axion_forex_quote(AxionClient *client, const char *ticker) {
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
    AxionResponse *response = _axion_request(client, path, query);
    if (query) free(query);
    return response;
}

// Futures API
AxionResponse* axion_futures_tickers(AxionClient *client, const char *exchange) {
    const char *keys[] = {"exchange"};
    const char *values[] = {exchange};
    char *query = _build_query(keys, values, 1);
    AxionResponse *response = _axion_request(client, "futures/tickers", query);
    if (query) free(query);
    return response;
}

AxionResponse* axion_futures_quote(AxionClient *client, const char *ticker) {
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
    AxionResponse *response = _axion_request(client, path, query);
    if (query) free(query);
    return response;
}

// Indices API
AxionResponse* axion_indices_tickers(AxionClient *client, const char *exchange) {
    const char *keys[] = {"exchange"};
    const char *values[] = {exchange};
    char *query = _build_query(keys, values, 1);
    AxionResponse *response = _axion_request(client, "indices/tickers", query);
    if (query) free(query);
    return response;
}

AxionResponse* axion_indices_quote(AxionClient *client, const char *ticker) {
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
    AxionResponse *response = _axion_request(client, path, query);
    if (query) free(query);
    return response;
}

// Economic API
AxionResponse* axion_econ_search(AxionClient *client, const char *query) {
    char full_query[512];
    snprintf(full_query, sizeof(full_query), "query=%s", query);
    return _axion_request(client, "econ/search", full_query);
}

AxionResponse* axion_econ_dataset(AxionClient *client, const char *series_id) {
    char path[256];
    snprintf(path, sizeof(path), "econ/dataset/%s", series_id);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_econ_calendar(AxionClient *client, const char *from_date, const char *to_date, const char *country, int min_importance, const char *currency, const char *category) {
    char query[1024] = "";
    int offset = 0;

    if (from_date) offset += snprintf(query + offset, sizeof(query) - offset, "from=%s&", from_date);
    if (to_date) offset += snprintf(query + offset, sizeof(query) - offset, "to=%s&", to_date);
    if (country) offset += snprintf(query + offset, sizeof(query) - offset, "country=%s&", country);
    if (min_importance >= 0) offset += snprintf(query + offset, sizeof(query) - offset, "minImportance=%d&", min_importance);
    if (currency) offset += snprintf(query + offset, sizeof(query) - offset, "currency=%s&", currency);
    if (category) offset += snprintf(query + offset, sizeof(query) - offset, "category=%s&", category);

    // Remove trailing '&' if it exists
    if (offset > 0) {
        query[offset - 1] = '\0';
    }

    return _axion_request(client, "econ/calendar", query[0] ? query : NULL);
}

// News API
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

// Sentiment API
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

// Company Profile API
AxionResponse* axion_profiles_asset(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/asset", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_recommendation(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/recommendation", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_cashflow(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/cashflow", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_index_trend(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/trend/index", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_statistics(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/statistics", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_income(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/income", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_fund(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/fund", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_summary(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/summary", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_insiders(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/insiders", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_calendar(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/calendar", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_balancesheet(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/balancesheet", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_earnings_trend(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/trend/earnings", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_institution_ownership(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/institution", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_ownership(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/ownership", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_earnings(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/earnings", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_info(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/info", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_activity(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/activity", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_transactions(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/transactions", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_financials(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/financials", ticker);
    return _axion_request(client, path, NULL);
}

AxionResponse* axion_profiles_traffic(AxionClient *client, const char *ticker) {
    char path[256];
    snprintf(path, sizeof(path), "profiles/%s/traffic", ticker);
    return _axion_request(client, path, NULL);
}
