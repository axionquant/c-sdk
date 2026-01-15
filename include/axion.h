#ifndef AXION_H
#define AXION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declare cJSON to avoid including the full header in the public API
struct cJSON;

/**
 * @struct AxionClient
 * @brief  Holds the client configuration, including the API key and base URL.
 *
 * This struct is intended to be opaque to the user.
 */
typedef struct AxionClient AxionClient;

/**
 * @struct AxionResponse
 * @brief  Contains the response from an API call.
 */
typedef struct {
    int http_status;
    char *data; // Raw JSON string data
    struct cJSON *json; // Parsed cJSON object
    char *error;
} AxionResponse;

/**
 * @brief Initializes a new Axion client.
 *
 * @param api_key The API key for authenticating with the Axion API.
 * @return A pointer to a new AxionClient instance, or NULL on failure.
 *
 * The returned client must be freed using axion_free_client().
 */
AxionClient* axion_init(const char *api_key);

/**
 * @brief Frees all resources used by the Axion client.
 *
 * @param client The Axion client to free.
 */
void axion_free_client(AxionClient *client);

/**
 * @brief Frees the memory allocated for an AxionResponse.
 *
 * @param response The response to free.
 */
void axion_free_response(AxionResponse *response);

/**
 * @brief Get a single stock ticker by its ticker symbol.
 *
 * @param client The initialized Axion client.
 * @param ticker The stock ticker symbol (e.g., "AAPL").
 * @return An AxionResponse containing the API result. The caller is responsible
 *         for freeing this response with axion_free_response().
 */
AxionResponse* axion_get_stock_ticker_by_symbol(AxionClient *client, const char *ticker);

/**
 * @brief Get prices for a specific stock ticker.
 *
 * @param client The initialized Axion client.
 * @param ticker The stock ticker symbol.
 * @param from_date Optional start date (YYYY-MM-DD). Can be NULL.
 * @param to_date Optional end date (YYYY-MM-DD). Can be NULL.
 * @param frame Optional time frame ('daily', 'weekly', etc.). Can be NULL.
 * @return An AxionResponse containing the API result. The caller is responsible
 *         for freeing this response with axion_free_response().
 */
AxionResponse* axion_get_stock_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);


// ... (Other function prototypes will be added here) ...


#endif // AXION_H
