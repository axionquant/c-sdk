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
    AxionResponse *response = axion_get_stock_prices(
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
AxionResponse* axion_get_stock_tickers(
    AxionClient *client,
    const char *country,    // Optional: filter by country
    const char *exchange    // Optional: filter by exchange
);

// Get ticker information
AxionResponse* axion_get_stock_ticker_by_symbol(
    AxionClient *client,
    const char *ticker
);

// Get historical prices
AxionResponse* axion_get_stock_prices(
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
AxionResponse* axion_get_crypto_tickers(
    AxionClient *client,
    const char *type        // Optional: filter by type
);

// Get crypto ticker info
AxionResponse* axion_get_crypto_ticker_by_symbol(
    AxionClient *client,
    const char *ticker
);

// Get crypto prices
AxionResponse* axion_get_crypto_prices(
    AxionClient *client,
    const char *ticker,
    const char *from_date,
    const char *to_date,
    const char *frame
);
```

#### Forex

```c
AxionResponse* axion_get_forex_tickers(AxionClient *client, const char *country, const char *exchange);
AxionResponse* axion_get_forex_ticker_by_symbol(AxionClient *client, const char *ticker);
AxionResponse* axion_get_forex_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
```

#### Futures

```c
AxionResponse* axion_get_futures_tickers(AxionClient *client, const char *exchange);
AxionResponse* axion_get_futures_ticker_by_symbol(AxionClient *client, const char *ticker);
AxionResponse* axion_get_futures_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
```

#### Indices

```c
AxionResponse* axion_get_index_tickers(AxionClient *client, const char *exchange);
AxionResponse* axion_get_index_ticker_by_symbol(AxionClient *client, const char *ticker);
AxionResponse* axion_get_index_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
```

### Company Profiles

```c
// Asset profile
AxionResponse* axion_get_company_asset_profile(AxionClient *client, const char *ticker);

// Financial statements
AxionResponse* axion_get_company_income_statement(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_balance_sheet(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_cashflow(AxionClient *client, const char *ticker);

// Company data
AxionResponse* axion_get_company_profile_info(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_summary(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_statistics(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_financial_data(AxionClient *client, const char *ticker);

// Ownership & insider data
AxionResponse* axion_get_company_major_holders(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_institution_ownership(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_fund_ownership(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_insider_holders(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_insider_transactions(AxionClient *client, const char *ticker);

// Earnings & trends
AxionResponse* axion_get_company_earnings_history(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_earnings_trend(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_recommendation_trend(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_index_trend(AxionClient *client, const char *ticker);

// Additional data
AxionResponse* axion_get_company_calendar_events(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_share_activity(AxionClient *client, const char *ticker);
AxionResponse* axion_get_company_website_traffic(AxionClient *client, const char *ticker);
```

### Economic Data

```c
// Search economic datasets
AxionResponse* axion_search_econ(AxionClient *client, const char *query);

// Get dataset by series ID
AxionResponse* axion_get_econ_dataset(AxionClient *client, const char *series_id);

// Get economic calendar
AxionResponse* axion_get_econ_calendar(
    AxionClient *client,
    const char *from_date,      // Optional
    const char *to_date,        // Optional
    const char *country,        // Optional
    int min_importance,         // -1 for no filter
    const char *currency,       // Optional
    const char *category        // Optional
);
```

### News

```c
// Get latest news
AxionResponse* axion_get_news(AxionClient *client);

// Get company-specific news
AxionResponse* axion_get_company_news(AxionClient *client, const char *ticker);

// Get news by country
AxionResponse* axion_get_country_news(AxionClient *client, const char *country);

// Get news by category
AxionResponse* axion_get_category_news(AxionClient *client, const char *category);
```

### Sentiment Analysis

```c
// Get all sentiment data
AxionResponse* axion_get_sentiment_all(AxionClient *client, const char *ticker);

// Get social media sentiment
AxionResponse* axion_get_sentiment_social(AxionClient *client, const char *ticker);

// Get news sentiment
AxionResponse* axion_get_sentiment_news(AxionClient *client, const char *ticker);

// Get analyst sentiment
AxionResponse* axion_get_sentiment_analyst(AxionClient *client, const char *ticker);
```

### ESG & Credit

```c
// Get ESG data
AxionResponse* axion_get_esg_data(AxionClient *client, const char *ticker);

// Search credit entities
AxionResponse* axion_search_credit(AxionClient *client, const char *query);

// Get credit ratings
AxionResponse* axion_get_credit_ratings(AxionClient *client, const char *entity_id);
```

### Supply Chain

```c
// Get company customers
AxionResponse* axion_get_supply_chain_customers(AxionClient *client, const char *ticker);

// Get company suppliers
AxionResponse* axion_get_supply_chain_suppliers(AxionClient *client, const char *ticker);

// Get peer companies
AxionResponse* axion_get_supply_chain_peers(AxionClient *client, const char *ticker);
```

### ETF Data

```c
// Get ETF fund data
AxionResponse* axion_get_etf_fund_data(AxionClient *client, const char *ticker);

// Get ETF holdings
AxionResponse* axion_get_etf_holdings(AxionClient *client, const char *ticker);

// Get ETF exposure
AxionResponse* axion_get_etf_exposure(AxionClient *client, const char *ticker);
```

## Error Handling

Always check the `error` field in the response:

```c
AxionResponse *response = axion_get_stock_prices(client, "AAPL", NULL, NULL, NULL);

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
AxionResponse *response = axion_get_stock_prices(client, "AAPL", NULL, NULL, NULL);
// ... use response
axion_free_response(response);  // Always free!

axion_free_client(client);  // Free client when done
```

## Configuration

The SDK connects to `http://localhost:3001` by default. To use a different endpoint, modify the `BASE_URL` constant in `axion.c`.

## License

[Your License Here]

## Support

For issues and questions, please visit [your support URL].
