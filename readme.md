# AxionQuant C SDK

The **AxionQuant C SDK** is a C client library for the [Axion Financial Data API](https://axionquant.com). It provides programmatic access to **stock market data, cryptocurrency, forex, futures, indices, company fundamentals, financial statements, SEC filings, economic data, ETF data, financial news, and market sentiment** from C applications.

The SDK is designed for **quantitative research, financial analysis, financial modeling, and data-driven applications** that need direct access to financial market data through a C API.

Use the AxionQuant C SDK to retrieve real-time and historical market data, analyze company fundamentals, access regulatory filings, work with economic indicators, and integrate financial datasets into C and C++ applications.

* [Get a free Axion API key](https://axionquant.com/dashboard/api-keys)
* [Axion Financial Data API Documentation](https://axionquant.com/docs)
* [Axion C SDK Documentation](https://axionquant.com/developers/libraries)
* [AxionQuant C SDK on GitHub](https://github.com/axionquant/c-sdk)

## Features

The AxionQuant C SDK provides access to a broad range of financial datasets and market research tools.

* **Stock Market Data** - Stock quotes, ticker information, historical prices, gainers, losers, and market listings
* **Cryptocurrency Market Data** - Cryptocurrency quotes, historical prices, tickers, gainers, losers, categories, and ratings
* **Forex Market Data** - Foreign exchange quotes, historical prices, currency pairs, exchanges, and market data
* **Futures Market Data** - Futures contracts, quotes, historical prices, exchanges, currencies, and market data
* **Index Data** - Index quotes, historical prices, constituents, exposure, and market performance
* **Company Fundamentals** - Company profiles, financial statements, financial metrics, valuation ratios, and key business information
* **Earnings Data** - Earnings history, trends, reports, earnings calendars, transcripts, and transcript sentiment
* **SEC Filings** - SEC filing search, filing history, supported form types, document text, and filing sentiment
* **Insider & Ownership Data** - Insider transactions, institutional ownership, fund ownership, major holders, and ownership activity
* **Economic Data** - FRED economic datasets, economic indicators, and economic calendar data
* **ETF Data** - ETF quotes, historical prices, holdings, fund information, exposure, weights, and performance
* **Financial News & Sentiment** - Company news, financial news, social sentiment, news sentiment, and analyst sentiment
* **ESG & Credit Data** - ESG information and credit ratings
* **Supply Chain Data** - Company customers, suppliers, and business relationships
* **Web Traffic Data** - Company web traffic and audience data

## Installation

### Prerequisites

* C compiler such as GCC or Clang
* [libcurl](https://curl.se/libcurl/)
* [cJSON](https://github.com/DaveGamble/cJSON)

### Building the C SDK

Install the required dependencies and compile the AxionQuant C client as a shared library:

```bash
# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev libcjson-dev
```

```bash
# Compile the Axion C SDK as a shared library
gcc -c axion.c -o axion.o -lcurl -lcjson -fPIC
gcc -shared -o libaxion.so axion.o -lcurl -lcjson

# Compile your C application against the Axion library
gcc -o myapp myapp.c -L. -laxion -lcurl -lcjson
```

## Quick Start

Get a [free Axion API key](https://axionquant.com/dashboard/api-keys) and use the C SDK to retrieve financial market data:

```c
#include "axion.h"
#include <stdio.h>

int main() {
    AxionClient *client = axion_init("your_api_key_here");

    if (!client) {
        fprintf(stderr, "Failed to initialize client\n");
        return 1;
    }

    // Get historical stock market data
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

The AxionQuant C SDK provides a C interface to the Axion Financial Data API. Each endpoint returns an `AxionResponse` containing the HTTP status, raw response data, parsed JSON, and any error information.

## Client Management

Initialize and manage an Axion API client and its responses:

```c
// Initialize a new client with your API key
AxionClient* axion_init(const char *api_key);

// Free the client and associated resources
void axion_client(AxionClient *client);

// Free a response object
void axion_response(AxionResponse *response);
```

## Response Structure

```c
struct AxionResponse {
    int http_status;   // HTTP status code
    char *data;        // Raw response string
    cJSON *json;       // Parsed JSON (if successful)
    char *error;       // Error message (if any)
};
```

---

## Market Data API

Access **real-time and historical financial market data** for stocks, cryptocurrencies, forex, futures, and market indices.

### Stock Market Data

Retrieve stock tickers, quotes, historical prices, market gainers and losers, and stock market listings.

```c
AxionResponse* axion_stocks_tickers(AxionClient *client, const char *country, const char *exchange);
AxionResponse* axion_stocks_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_stocks_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_stocks_gainers(AxionClient *client, int days, int limit, const char *market);
AxionResponse* axion_stocks_losers(AxionClient *client, int days, int limit, const char *market);
AxionResponse* axion_stocks_quote(AxionClient *client, const char *ticker);
AxionResponse* axion_stocks_list_market(AxionClient *client);
AxionResponse* axion_stocks_list_country(AxionClient *client);
AxionResponse* axion_stocks_list_currency(AxionClient *client);
AxionResponse* axion_stocks_list_sector(AxionClient *client);
AxionResponse* axion_stocks_list_industry(AxionClient *client);
AxionResponse* axion_stocks_list_type(AxionClient *client);
```

### Cryptocurrency Market Data

```c
AxionResponse* axion_crypto_tickers(AxionClient *client, const char *type);
AxionResponse* axion_crypto_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_crypto_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_crypto_gainers(AxionClient *client, int days, int limit);
AxionResponse* axion_crypto_losers(AxionClient *client, int days, int limit);
AxionResponse* axion_crypto_quote(AxionClient *client, const char *ticker);
AxionResponse* axion_crypto_list_category(AxionClient *client);
AxionResponse* axion_crypto_list_rating(AxionClient *client);
AxionResponse* axion_crypto_list_type(AxionClient *client);
```

### Forex Market Data

```c
AxionResponse* axion_forex_tickers(AxionClient *client, const char *country, const char *exchange);
AxionResponse* axion_forex_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_forex_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_forex_gainers(AxionClient *client, int days, int limit);
AxionResponse* axion_forex_losers(AxionClient *client, int days, int limit);
AxionResponse* axion_forex_quote(AxionClient *client, const char *ticker);
AxionResponse* axion_forex_list_exchange(AxionClient *client);
AxionResponse* axion_forex_list_rating(AxionClient *client);
AxionResponse* axion_forex_list_country(AxionClient *client);
```

### Futures Market Data

```c
AxionResponse* axion_futures_tickers(AxionClient *client, const char *exchange);
AxionResponse* axion_futures_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_futures_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_futures_gainers(AxionClient *client, int days, int limit);
AxionResponse* axion_futures_losers(AxionClient *client, int days, int limit);
AxionResponse* axion_futures_quote(AxionClient *client, const char *ticker);
AxionResponse* axion_futures_list_exchange(AxionClient *client);
AxionResponse* axion_futures_list_currency(AxionClient *client);
AxionResponse* axion_futures_list_timezone(AxionClient *client);
AxionResponse* axion_futures_list_country(AxionClient *client);
```

### Index Market Data

```c
AxionResponse* axion_indices_tickers(AxionClient *client, const char *exchange);
AxionResponse* axion_indices_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_indices_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_indices_gainers(AxionClient *client, int days, int limit);
AxionResponse* axion_indices_losers(AxionClient *client, int days, int limit);
AxionResponse* axion_indices_quote(AxionClient *client, const char *ticker);
AxionResponse* axion_indices_components(AxionClient *client, const char *ticker);
AxionResponse* axion_indices_exposure(AxionClient *client, const char *ticker);
AxionResponse* axion_indices_list_exchange(AxionClient *client);
AxionResponse* axion_indices_list_timezone(AxionClient *client);
AxionResponse* axion_indices_list_country(AxionClient *client);
```

> **`frame` values:** `"1d"`, `"1h"`, `"5m"`, etc. Pass `NULL` for any optional parameter.

---

## Company Fundamentals and Profiles

Access company profiles, business information, statistics, market summaries, calendars, and analyst recommendations.

```c
AxionResponse* axion_profiles_profile(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_info(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_summary(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_statistics(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_calendar(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_recommendation(AxionClient *client, const char *ticker);
```

## Financial Statements and Financial Metrics

Retrieve company income statements, balance sheets, cash flow statements, financial metrics, valuation ratios, and historical financial data for fundamental analysis.

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

// Full financial statements
AxionResponse* axion_financials_balance_sheet(AxionClient *client, const char *ticker, const char *year, const char *quarter);
AxionResponse* axion_financials_income_statement(AxionClient *client, const char *ticker, const char *year, const char *quarter);
AxionResponse* axion_financials_cash_flow_statement(AxionClient *client, const char *ticker, const char *year, const char *quarter);

// Historical valuation ratios
AxionResponse* axion_financials_eps(AxionClient *client, const char *ticker, const char *from, const char *to);
AxionResponse* axion_financials_pe(AxionClient *client, const char *ticker, const char *from, const char *to, const char *frame);
AxionResponse* axion_financials_market_cap(AxionClient *client, const char *ticker, const char *from, const char *to, const char *frame);
AxionResponse* axion_financials_roe(AxionClient *client, const char *ticker, const char *from, const char *to);
AxionResponse* axion_financials_enterprise_value(AxionClient *client, const char *ticker, const char *from, const char *to, const char *frame);
AxionResponse* axion_financials_ebitda(AxionClient *client, const char *ticker, const char *from, const char *to);
AxionResponse* axion_financials_debt_to_equity(AxionClient *client, const char *ticker, const char *from, const char *to);
```

---

## Earnings and Earnings Call Data

Retrieve historical earnings, earnings trends, earnings reports, earnings call transcripts, and transcript sentiment.

```c
AxionResponse* axion_earnings_history(AxionClient *client, const char *ticker);
AxionResponse* axion_earnings_trend(AxionClient *client, const char *ticker);
AxionResponse* axion_earnings_index(AxionClient *client, const char *ticker);
AxionResponse* axion_earnings_report(AxionClient *client, const char *ticker, const char *year, const char *quarter);
AxionResponse* axion_earnings_transcript_sentiment(AxionClient *client, const char *id);
AxionResponse* axion_earnings_transcript(AxionClient *client, const char *ticker, const char *year, const char *quarter);
```

## SEC Filings and Regulatory Data

Access SEC filings programmatically, search filing histories, retrieve supported filing forms, and retrieve filing document text and sentiment.

```c
// Get recent filings for a ticker
AxionResponse* axion_filings_recent(AxionClient *client, const char *ticker, const char *form, int limit);

// Get filing history by form type and date range
AxionResponse* axion_filings_history(AxionClient *client, const char *ticker, const char *form_type, const char *start_date, const char *end_date);

// Get all supported form type descriptions
AxionResponse* axion_filings_list_forms(AxionClient *client);

// Search filings
AxionResponse* axion_filings_search(AxionClient *client, const char *ticker, const char *form, const char *year, const char *quarter);

// Get raw text of a filing document
AxionResponse* axion_filings_document_text(AxionClient *client, const char *document_id);

// Get sentiment of a filing document
AxionResponse* axion_filings_document_sentiment(AxionClient *client, const char *document_id);
```

## Insider Trading and Ownership Data

Retrieve insider transactions, individual holders, institutional ownership, fund ownership, and ownership activity.

```c
AxionResponse* axion_insiders_funds(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_individuals(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_institutions(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_ownership(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_activity(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_transactions(AxionClient *client, const char *ticker);
```

## Economic and Macroeconomic Data

Access FRED economic datasets, economic indicators, and economic calendar data for financial analysis and quantitative research.

```c
// Search FRED economic datasets
AxionResponse* axion_econ_search(AxionClient *client, const char *query);

// Find economic series by natural language description
AxionResponse* axion_econ_find(AxionClient *client, const char *query);

// Get a dataset by series ID
AxionResponse* axion_econ_dataset(AxionClient *client, const char *series_id);

// Get the economic calendar
AxionResponse* axion_econ_calendar(
    AxionClient *client,
    const char *from_date,
    const char *to_date,
    const char *country,
    int min_importance,
    const char *currency,
    const char *category
);
```

## Financial News API

Retrieve general financial news, company-specific news, country-specific news, and category-based financial news.

```c
AxionResponse* axion_news_general(AxionClient *client);
AxionResponse* axion_news_company(AxionClient *client, const char *ticker);
AxionResponse* axion_news_country(AxionClient *client, const char *country);
AxionResponse* axion_news_category(AxionClient *client, const char *category);
```

## Market Sentiment API

Access social, news, and analyst market sentiment data for companies and financial markets.

```c
AxionResponse* axion_sentiment_social(AxionClient *client, const char *ticker);
AxionResponse* axion_sentiment_news(AxionClient *client, const char *ticker);
AxionResponse* axion_sentiment_analyst(AxionClient *client, const char *ticker);
```

## ESG and Credit Data

```c
AxionResponse* axion_esg_data(AxionClient *client, const char *ticker);
AxionResponse* axion_credit_search(AxionClient *client, const char *query);
AxionResponse* axion_credit_ratings(AxionClient *client, const char *entity_id);
```

## Supply Chain Data

Retrieve company customers, suppliers, and peer relationships for fundamental and business analysis.

```c
AxionResponse* axion_supply_chain_customers(AxionClient *client, const char *ticker);
AxionResponse* axion_supply_chain_suppliers(AxionClient *client, const char *ticker);
AxionResponse* axion_supply_chain_peers(AxionClient *client, const char *ticker);
```

## ETF Market Data

Access ETF quotes, historical prices, fund information, holdings, portfolio exposure, weights, and market performance.

```c
AxionResponse* axion_etfs_tickers(AxionClient *client, const char *country, const char *exchange);
AxionResponse* axion_etfs_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_etfs_quote(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_gainers(AxionClient *client, int days, int limit);
AxionResponse* axion_etfs_losers(AxionClient *client, int days, int limit);
AxionResponse* axion_etfs_fund(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_holdings(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_holdings_all(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_exposure(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_weights(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_list_market(AxionClient *client);
AxionResponse* axion_etfs_list_country(AxionClient *client);
AxionResponse* axion_etfs_list_currency(AxionClient *client);
AxionResponse* axion_etfs_list_sector(AxionClient *client);
AxionResponse* axion_etfs_list_industry(AxionClient *client);
AxionResponse* axion_etfs_list_type(AxionClient *client);
```

## Web Traffic Data

```c
AxionResponse* axion_webtraffic_traffic(AxionClient *client, const char *ticker);
```

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

Every response returned by the SDK should be released with `axion_response()`, and the client should be released with `axion_client()` when it is no longer needed.

```c
AxionResponse *r = axion_stocks_prices(client, "AAPL", NULL, NULL, NULL);

// ... use r ...

axion_response(r);
axion_client(client);
```

## Documentation and Resources

* [AxionQuant Financial Data API](https://axionquant.com)
* [AxionQuant API Documentation](https://axionquant.com/docs)
* [AxionQuant C SDK](https://axionquant.com/developers/libraries)
* [Get a Free API Key](https://axionquant.com/dashboard/api-keys)
* [AxionQuant GitHub](https://github.com/axionquant/c-sdk)

## Get Started

The AxionQuant C SDK provides a C interface to a broad range of financial market datasets for **quantitative research, financial analysis, and financial applications**.

[Get a free Axion API key](https://axionquant.com/dashboard/api-keys) or [read the Axion Financial Data API documentation](https://axionquant.com/docs) to get started.

## License

MIT

