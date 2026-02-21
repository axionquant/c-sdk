# Axion C SDK

A C client library for the [Axion](https://axionquant.com) financial data API, providing access to market data, company profiles, economic indicators, news, sentiment, filings, financials, and more.

## Features

- **Market Data** - Stocks, crypto, forex, futures, and indices (quotes, tickers, historical prices)
- **Company Profiles** - Summaries, info, statistics, calendar, and recommendations
- **Financials** - Income, balance sheet, cash flow, and key metrics
- **Earnings** - History, trends, index trends, and full reports
- **Filings** - SEC filings search and form retrieval
- **Insiders** - Ownership, transactions, funds, institutions, and activity
- **Economic Data** - FRED datasets, economic calendar search
- **ESG & Credit** - ESG scores and credit ratings
- **News & Sentiment** - Financial news and social/analyst sentiment analysis
- **Supply Chain** - Customer, supplier, and peer relationships
- **ETF Data** - Fund info, holdings, and exposure
- **Web Traffic** - Company web traffic data

## Installation

### Prerequisites

- C compiler (GCC, Clang, etc.)
- [libcurl](https://curl.se/libcurl/)
- [cJSON](https://github.com/DaveGamble/cJSON)

```bash
# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev libcjson-dev
```

### Building

```bash
# Compile as a shared library
gcc -c axion.c -o axion.o -lcurl -lcjson -fPIC
gcc -shared -o libaxion.so axion.o -lcurl -lcjson

# Compile your program against the library
gcc -o myapp myapp.c -L. -laxion -lcurl -lcjson
```

## Quick Start

[Get your free API key](https://axionquant.com/dashboard/api-keys)

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

    // Get historical stock prices
    AxionResponse *response = axion_stocks_prices(
        client,
        "AAPL",
        "2024-01-01",
        "2024-12-31",
        "1d"
    );

    if (response->error) {
        fprintf(stderr, "Error: %s\n", response->error);
    } else if (response->json) {
        char *json_str = cJSON_Print(response->json);
        printf("%s\n", json_str);
        free(json_str);
    }

    axion_response(response);
    axion_client(client);
    return 0;
}
```

## API Reference

### Client Management

```c
// Initialize a new client with your API key
AxionClient* axion_init(const char *api_key);

// Free the client and associated resources
void axion_client(AxionClient *client);

// Free a response object
void axion_response(AxionResponse *response);
```

### Response Structure

```c
struct AxionResponse {
    int http_status;   // HTTP status code
    char *data;        // Raw response string
    cJSON *json;       // Parsed JSON (if successful)
    char *error;       // Error message (if any)
};
```

---

### Market Data

#### Stocks

```c
AxionResponse* axion_stocks_tickers(AxionClient *client, const char *country, const char *exchange);
AxionResponse* axion_stocks_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_stocks_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
```

#### Cryptocurrency

```c
AxionResponse* axion_crypto_tickers(AxionClient *client, const char *type);
AxionResponse* axion_crypto_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_crypto_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
```

#### Forex

```c
AxionResponse* axion_forex_tickers(AxionClient *client, const char *country, const char *exchange);
AxionResponse* axion_forex_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_forex_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
```

#### Futures

```c
AxionResponse* axion_futures_tickers(AxionClient *client, const char *exchange);
AxionResponse* axion_futures_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_futures_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
```

#### Indices

```c
AxionResponse* axion_indices_tickers(AxionClient *client, const char *exchange);
AxionResponse* axion_indices_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_indices_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
```

> **`frame` values:** `"1d"`, `"1h"`, `"5m"`, etc. Pass `NULL` for any optional parameter.

---

### Company Profiles

```c
AxionResponse* axion_profiles_profile(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_info(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_summary(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_statistics(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_calendar(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_recommendation(AxionClient *client, const char *ticker);
```

---

### Financials

```c
// All financial functions accept a `periods` argument (pass 0 for default)
AxionResponse* axion_financials_revenue(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_net_income(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_total_assets(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_total_liabilities(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_stockholders_equity(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_current_assets(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_current_liabilities(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_operating_cash_flow(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_capital_expenditures(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_free_cash_flow(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_shares_outstanding_basic(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_shares_outstanding_diluted(AxionClient *client, const char *ticker, int periods);
AxionResponse* axion_financials_metrics(AxionClient *client, const char *ticker);
AxionResponse* axion_financials_snapshot(AxionClient *client, const char *ticker);
```

---

### Earnings

```c
AxionResponse* axion_earnings_history(AxionClient *client, const char *ticker);
AxionResponse* axion_earnings_trend(AxionClient *client, const char *ticker);
AxionResponse* axion_earnings_index(AxionClient *client, const char *ticker);
AxionResponse* axion_earnings_report(AxionClient *client, const char *ticker, const char *year, const char *quarter);
```

---

### Filings

```c
// Get all filings for a ticker (pass limit=0 and form=NULL for defaults)
AxionResponse* axion_filings_filings(AxionClient *client, const char *ticker, int limit, const char *form);

// Get filings by form type
AxionResponse* axion_filings_forms(AxionClient *client, const char *ticker, const char *form_type, const char *year, const char *quarter, int limit);

// Get all supported form type descriptions
AxionResponse* axion_filings_desc_forms(AxionClient *client);

// Search filings
AxionResponse* axion_filings_search(AxionClient *client, const char *year, const char *quarter, const char *form, const char *ticker);
```

---

### Insiders

```c
AxionResponse* axion_insiders_funds(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_individuals(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_institutions(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_ownership(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_activity(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_transactions(AxionClient *client, const char *ticker);
```

---

### Economic Data

```c
// Search FRED economic datasets
AxionResponse* axion_econ_search(AxionClient *client, const char *query);

// Get a dataset by series ID
AxionResponse* axion_econ_dataset(AxionClient *client, const char *series_id);

// Get the economic calendar (all parameters optional - pass NULL / -1 to skip)
AxionResponse* axion_econ_calendar(
    AxionClient *client,
    const char *from_date,
    const char *to_date,
    const char *country,
    int min_importance,   // -1 to skip, 0-3 for importance level
    const char *currency,
    const char *category
);
```

---

### News

```c
AxionResponse* axion_news_general(AxionClient *client);
AxionResponse* axion_news_company(AxionClient *client, const char *ticker);
AxionResponse* axion_news_country(AxionClient *client, const char *country);
AxionResponse* axion_news_category(AxionClient *client, const char *category);
```

---

### Sentiment

```c
AxionResponse* axion_sentiment_all(AxionClient *client, const char *ticker);
AxionResponse* axion_sentiment_social(AxionClient *client, const char *ticker);
AxionResponse* axion_sentiment_news(AxionClient *client, const char *ticker);
AxionResponse* axion_sentiment_analyst(AxionClient *client, const char *ticker);
```

---

### ESG & Credit

```c
AxionResponse* axion_esg_data(AxionClient *client, const char *ticker);
AxionResponse* axion_credit_search(AxionClient *client, const char *query);
AxionResponse* axion_credit_ratings(AxionClient *client, const char *entity_id);
```

---

### Supply Chain

```c
AxionResponse* axion_supply_chain_customers(AxionClient *client, const char *ticker);
AxionResponse* axion_supply_chain_suppliers(AxionClient *client, const char *ticker);
AxionResponse* axion_supply_chain_peers(AxionClient *client, const char *ticker);
```

---

### ETF Data

```c
AxionResponse* axion_etfs_fund(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_holdings(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_exposure(AxionClient *client, const char *ticker);
```

---

### Web Traffic

```c
AxionResponse* axion_webtraffic_traffic(AxionClient *client, const char *ticker);
```

---

## Error Handling

Always check the `error` field before accessing `json` or `data`:

```c
AxionResponse *response = axion_stocks_prices(client, "AAPL", NULL, NULL, NULL);

if (response->error) {
    fprintf(stderr, "Error (HTTP %d): %s\n", response->http_status, response->error);
} else if (response->json) {
    char *out = cJSON_Print(response->json);
    printf("%s\n", out);
    free(out);
}

axion_response(response);
```

## Memory Management

You must free every response and the client when finished:

```c
AxionResponse *r = axion_stocks_prices(client, "AAPL", NULL, NULL, NULL);
// ... use r ...
axion_response(r);      // free the response

axion_client(client);   // free the client when done
```

## Get Started

For detailed API documentation, support, or to obtain an API key, visit the [Axion](https://axionquant.com) website.

## License

MIT

