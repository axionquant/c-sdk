#include "axion.h"
#include <stdio.h>

int main() {
    // Initialize client with API key
    AxionClient *client = axion_init("your-api-key-here");

    if (!client) {
        printf("Failed to initialize client\n");
        return 1;
    }

    // Example: Get stock prices
    AxionResponse *response = axion_get_stock_prices(client, "AAPL", "2024-01-01", "2024-01-31", "daily");

    if (response) {
        if (response->error) {
            printf("Error: %s\n", response->error);
        } else if (response->json) {
            // Parse the JSON response
            char *json_str = cJSON_Print(response->json);
            printf("Response: %s\n", json_str);
            free(json_str);
        }
        axion_free_response(response);
    }

    // Clean up
    axion_free_client(client);
    return 0;
}
