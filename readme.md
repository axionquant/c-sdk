# Axion C SDK

A C client library for the Axion financial data API, providing access to market data, company profiles, economic indicators, news, and more.

## Features

- **Market Data**: Stocks, crypto, forex, futures, and indices
- **Company Information**: Financial statements, profiles, ownership data
- **Economic Data**: FRED datasets, economic calendar
- **ESG & Credit**: ESG scores and credit ratings
- **News & Sentiment**: Financial news and sentiment analysis
- **Supply Chain**: Customer, supplier, and peer relationships
- **ETF Data**: Fund information, holdings, and exposure

## Installation

### Prerequisites

- C compiler (GCC, Clang, etc.)
- libcurl
- cJSON library

### Building

```bash
# Compile your application with the SDK
gcc -o myapp myapp.c axion.c -lcurl -lcjson
```

## Quick Start

```c
#include "axion.h"
#include <stdio.h>

int main() {
    // Initialize client
    AxionClient *client = axion_init("your_api_key_here");
    if (!client) {
        fprintf(stderr, "Failed to initialize client\n");
        return 1;
    }

    // Get stock prices
    AxionResponse *response = axion_stocks_prices(
        client,
        "AAPL",
        "2024-01-01",
        "2024-12-31",
        "1d"
    );

    if (response->error) {
        fprintf(stderr, "Error: %s\n", response->error);
    } else {
        printf("Response: %s\n", response->data);
    }

    // Cleanup
    axion_free_response(response);
    axion_free_client(client);

    return 0;
}
```

## API Reference

### Client Management

#### `AxionClient* axion_init(const char *api_key)`
Initialize a new Axion client.

#### `void axion_free_client(AxionClient *client)`
Free client resources.

#### `void axion_free_response(AxionResponse *response)`
Free response resources.

### Response Structure

```c
struct AxionResponse {
    int http_status;      // HTTP status code
    char *data;           // Raw response data
    cJSON *json;          // Parsed JSON (if successful)
    char *error;          // Error message (if any)
};
```

### Market Data

#### Stocks

```c
// Get all stock tickers
AxionResponse* axion_stocks_tickers(
    AxionClient *client,
    const char *country,    // Optional: filter by country
    const char *exchange    // Optional: filter by exchange
);

// Get stock quote
AxionResponse* axion_stocks_quote(
    AxionClient *client,
    const char *ticker
);

// Get historical prices
AxionResponse* axion_stocks_prices(
    AxionClient *client,
    const char *ticker,
    const char *from_date,  // Format: YYYY-MM-DD
    const char *to_date,    // Format: YYYY-MM-DD
    const char *frame       // "1d", "1h", "5m", etc.
);
```

#### Cryptocurrency

```c
// Get crypto tickers
AxionResponse* axion_crypto_tickers(
    AxionClient *client,
    const char *type        // Optional: filter by type
);

// Get crypto quote
AxionResponse* axion_crypto_quote(
    AxionClient *client,
    const char *ticker
);

// Get crypto prices
AxionResponse* axion_crypto_prices(
    AxionClient *client,
    const char *ticker,
    const char *from_date,
    const char *to_date,
    const char *frame
);
```

#### Forex

```c
// Get forex tickers
AxionResponse* axion_forex_tickers(
    AxionClient *client,
    const char *country,
    const char *exchange
);

// Get forex quote
AxionResponse* axion_forex_quote(
    AxionClient *client,
    const char *ticker
);

// Get forex prices
AxionResponse* axion_forex_prices(
    AxionClient *client,
    const char *ticker,
    const char *from_date,
    const char *to_date,
    const char *frame
);
```

#### Futures

```c
// Get futures tickers
AxionResponse* axion_futures_tickers(
    AxionClient *client,
    const char *exchange
);

// Get futures quote
AxionResponse* axion_futures_quote(
    AxionClient *client,
    const char *ticker
);

// Get futures prices
AxionResponse* axion_futures_prices(
    AxionClient *client,
    const char *ticker,
    const char *from_date,
    const char *to_date,
    const char *frame
);
```

#### Indices

```c
// Get index tickers
AxionResponse* axion_indices_tickers(
    AxionClient *client,
    const char *exchange
);

// Get index quote
AxionResponse* axion_indices_quote(
    AxionClient *client,
    const char *ticker
);

// Get index prices
AxionResponse* axion_indices_prices(
    AxionClient *client,
    const char *ticker,
    const char *from_date,
    const char *to_date,
    const char *frame
);
```

### Company Profiles

```c
// Asset profile
AxionResponse* axion_profiles_asset(AxionClient *client, const char *ticker);

// Financial statements
AxionResponse* axion_profiles_income(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_balancesheet(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_cashflow(AxionClient *client, const char *ticker);

// Company data
AxionResponse* axion_profiles_info(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_summary(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_statistics(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_financials(AxionClient *client, const char *ticker);

// Ownership & insider data
AxionResponse* axion_profiles_ownership(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_institution_ownership(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_fund(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_insiders(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_transactions(AxionClient *client, const char *ticker);

// Earnings & trends
AxionResponse* axion_profiles_earnings(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_earnings_trend(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_recommendation(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_index_trend(AxionClient *client, const char *ticker);

// Additional data
AxionResponse* axion_profiles_calendar(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_activity(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_traffic(AxionClient *client, const char *ticker);
```

### Economic Data

```c
// Search economic datasets
AxionResponse* axion_econ_search(AxionClient *client, const char *query);

// Get dataset by series ID
AxionResponse* axion_econ_dataset(AxionClient *client, const char *series_id);

// Get economic calendar
AxionResponse* axion_econ_calendar(
    AxionClient *client,
    const char *from_date,      // Optional
    const char *to_date,        // Optional
    const char *country,        // Optional
    int min_importance,         // -1 for no filter, 0-3 for importance level
    const char *currency,       // Optional
    const char *category        // Optional
);
```

### News

```c
// Get latest general news
AxionResponse* axion_news_general(AxionClient *client);

// Get company-specific news
AxionResponse* axion_news_company(AxionClient *client, const char *ticker);

// Get news by country
AxionResponse* axion_news_country(AxionClient *client, const char *country);

// Get news by category
AxionResponse* axion_news_category(AxionClient *client, const char *category);
```

### Sentiment Analysis

```c
// Get all sentiment data
AxionResponse* axion_sentiment_all(AxionClient *client, const char *ticker);

// Get social media sentiment
AxionResponse* axion_sentiment_social(AxionClient *client, const char *ticker);

// Get news sentiment
AxionResponse* axion_sentiment_news(AxionClient *client, const char *ticker);

// Get analyst sentiment
AxionResponse* axion_sentiment_analyst(AxionClient *client, const char *ticker);
```

### ESG & Credit

```c
// Get ESG data
AxionResponse* axion_esg_data(AxionClient *client, const char *ticker);

// Search credit entities
AxionResponse* axion_credit_search(AxionClient *client, const char *query);

// Get credit ratings
AxionResponse* axion_credit_ratings(AxionClient *client, const char *entity_id);
```

### Supply Chain

```c
// Get company customers
AxionResponse* axion_supply_chain_customers(AxionClient *client, const char *ticker);

// Get company suppliers
AxionResponse* axion_supply_chain_suppliers(AxionClient *client, const char *ticker);

// Get peer companies
AxionResponse* axion_supply_chain_peers(AxionClient *client, const char *ticker);
```

### ETF Data

```c
// Get ETF fund data
AxionResponse* axion_etfs_fund(AxionClient *client, const char *ticker);

// Get ETF holdings
AxionResponse* axion_etfs_holdings(AxionClient *client, const char *ticker);

// Get ETF exposure
AxionResponse* axion_etfs_exposure(AxionClient *client, const char *ticker);
```

## Error Handling

Always check the `error` field in the response:

```c
AxionResponse *response = axion_stocks_prices(client, "AAPL", NULL, NULL, NULL);

if (response->error) {
    fprintf(stderr, "Error: %s (HTTP %d)\n", response->error, response->http_status);
} else if (response->json) {
    // Process JSON data
    cJSON *data = response->json;
    // ... work with data
} else {
    printf("Raw response: %s\n", response->data);
}

axion_free_response(response);
```

## Memory Management

The SDK manages memory internally, but you must:

1. Free responses with `axion_free_response()`
2. Free the client with `axion_free_client()`

```c
// Good practice
AxionResponse *response = axion_stocks_prices(client, "AAPL", NULL, NULL, NULL);
// ... use response
axion_free_response(response);  // Always free!

axion_free_client(client);  // Free client when done
```
