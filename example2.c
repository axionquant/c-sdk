/**
 * Extended example with JSON parsing
 */

#include "axion.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

void parse_and_print_stock_info(const AxionResponse *response) {
    if (!response || !response->json || response->error) {
        printf("No valid JSON data to parse\n");
        return;
    }

    cJSON *json = response->json;

    // Extract specific fields (adjust based on actual API response structure)
    cJSON *symbol = cJSON_GetObjectItemCaseSensitive(json, "symbol");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    cJSON *price = cJSON_GetObjectItemCaseSensitive(json, "price");
    cJSON *change = cJSON_GetObjectItemCaseSensitive(json, "change");
    cJSON *changePercent = cJSON_GetObjectItemCaseSensitive(json, "changePercent");

    printf("Stock Information:\n");
    if (cJSON_IsString(symbol)) {
        printf("  Symbol: %s\n", symbol->valuestring);
    }
    if (cJSON_IsString(name)) {
        printf("  Name: %s\n", name->valuestring);
    }
    if (cJSON_IsNumber(price)) {
        printf("  Price: $%.2f\n", price->valuedouble);
    }
    if (cJSON_IsNumber(change)) {
        printf("  Change: $%.2f\n", change->valuedouble);
    }
    if (cJSON_IsNumber(changePercent)) {
        printf("  Change %%: %.2f%%\n", changePercent->valuedouble);
    }
    printf("\n");
}

int main() {
    // Initialize client
    AxionClient *client = axion_init("YOUR_API_KEY");
    if (!client) {
        fprintf(stderr, "Failed to initialize client\n");
        return 1;
    }

    // Get stock data
    AxionResponse *response = axion_get_stock_ticker_by_symbol(client, "AAPL");

    if (response) {
        if (response->error) {
            printf("Error: %s\n", response->error);
        } else if (response->json) {
            parse_and_print_stock_info(response);

            // Example: Print all keys in the response
            printf("All fields in response:\n");
            cJSON *item = response->json->child;
            while (item) {
                printf("  %s: ", item->string);
                if (cJSON_IsString(item)) {
                    printf("%s\n", item->valuestring);
                } else if (cJSON_IsNumber(item)) {
                    printf("%f\n", item->valuedouble);
                } else if (cJSON_IsBool(item)) {
                    printf("%s\n", item->valueint ? "true" : "false");
                } else {
                    printf("[complex type]\n");
                }
                item = item->next;
            }
        }

        axion_free_response(response);
    }

    axion_free_client(client);
    return 0;
}
