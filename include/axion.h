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


// =====================================================================
// CREDIT API
// =====================================================================
AxionResponse* axion_credit_search(AxionClient *client, const char *query);
AxionResponse* axion_credit_ratings(AxionClient *client, const char *entity_id);

// =====================================================================
// ESG API
// =====================================================================
AxionResponse* axion_esg_data(AxionClient *client, const char *ticker);

// =====================================================================
// ETF API
// =====================================================================
AxionResponse* axion_etfs_tickers(AxionClient *client, const char *country, const char *exchange);
AxionResponse* axion_etfs_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_etfs_fund(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_holdings(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_exposure(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_weights(AxionClient *client, const char *ticker);
AxionResponse* axion_etfs_gainers(AxionClient *client);
AxionResponse* axion_etfs_losers(AxionClient *client);
AxionResponse* axion_etfs_list_market(AxionClient *client);
AxionResponse* axion_etfs_list_country(AxionClient *client);
AxionResponse* axion_etfs_list_currency(AxionClient *client);
AxionResponse* axion_etfs_list_sector(AxionClient *client);
AxionResponse* axion_etfs_list_industry(AxionClient *client);
AxionResponse* axion_etfs_list_type(AxionClient *client);
AxionResponse* axion_etfs_quote(AxionClient *client, const char *ticker);

// =====================================================================
// SUPPLY CHAIN API
// =====================================================================
AxionResponse* axion_supply_chain_customers(AxionClient *client, const char *ticker);
AxionResponse* axion_supply_chain_peers(AxionClient *client, const char *ticker);
AxionResponse* axion_supply_chain_suppliers(AxionClient *client, const char *ticker);

// =====================================================================
// STOCKS API
// =====================================================================
AxionResponse* axion_stocks_tickers(AxionClient *client, const char *country, const char *exchange);
AxionResponse* axion_stocks_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_stocks_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_stocks_gainers(AxionClient *client);
AxionResponse* axion_stocks_losers(AxionClient *client);
AxionResponse* axion_stocks_list_market(AxionClient *client);
AxionResponse* axion_stocks_list_country(AxionClient *client);
AxionResponse* axion_stocks_list_currency(AxionClient *client);
AxionResponse* axion_stocks_list_sector(AxionClient *client);
AxionResponse* axion_stocks_list_industry(AxionClient *client);
AxionResponse* axion_stocks_list_type(AxionClient *client);
AxionResponse* axion_stocks_quote(AxionClient *client, const char *ticker);

// =====================================================================
// CRYPTO API
// =====================================================================
AxionResponse* axion_crypto_tickers(AxionClient *client, const char *type);
AxionResponse* axion_crypto_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_crypto_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_crypto_gainers(AxionClient *client);
AxionResponse* axion_crypto_losers(AxionClient *client);
AxionResponse* axion_crypto_list_category(AxionClient *client);
AxionResponse* axion_crypto_list_rating(AxionClient *client);
AxionResponse* axion_crypto_list_type(AxionClient *client);
AxionResponse* axion_crypto_quote(AxionClient *client, const char *ticker);

// =====================================================================
// FOREX API
// =====================================================================
AxionResponse* axion_forex_tickers(AxionClient *client, const char *country, const char *exchange);
AxionResponse* axion_forex_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_forex_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_forex_gainers(AxionClient *client);
AxionResponse* axion_forex_losers(AxionClient *client);
AxionResponse* axion_forex_list_exchange(AxionClient *client);
AxionResponse* axion_forex_list_rating(AxionClient *client);
AxionResponse* axion_forex_list_country(AxionClient *client);
AxionResponse* axion_forex_quote(AxionClient *client, const char *ticker);

// =====================================================================
// FUTURES API
// =====================================================================
AxionResponse* axion_futures_tickers(AxionClient *client, const char *exchange);
AxionResponse* axion_futures_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_futures_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_futures_gainers(AxionClient *client);
AxionResponse* axion_futures_losers(AxionClient *client);
AxionResponse* axion_futures_list_exchange(AxionClient *client);
AxionResponse* axion_futures_list_currency(AxionClient *client);
AxionResponse* axion_futures_list_timezone(AxionClient *client);
AxionResponse* axion_futures_list_country(AxionClient *client);
AxionResponse* axion_futures_quote(AxionClient *client, const char *ticker);

// =====================================================================
// INDICES API
// =====================================================================
AxionResponse* axion_indices_tickers(AxionClient *client, const char *exchange);
AxionResponse* axion_indices_ticker(AxionClient *client, const char *ticker);
AxionResponse* axion_indices_prices(AxionClient *client, const char *ticker, const char *from_date, const char *to_date, const char *frame);
AxionResponse* axion_indices_gainers(AxionClient *client);
AxionResponse* axion_indices_losers(AxionClient *client);
AxionResponse* axion_indices_list_exchange(AxionClient *client);
AxionResponse* axion_indices_list_timezone(AxionClient *client);
AxionResponse* axion_indices_list_country(AxionClient *client);
AxionResponse* axion_indices_quote(AxionClient *client, const char *ticker);

// =====================================================================
// ECONOMIC API
// =====================================================================
AxionResponse* axion_econ_search(AxionClient *client, const char *query);
AxionResponse* axion_econ_dataset(AxionClient *client, const char *series_id);
AxionResponse* axion_econ_calendar(AxionClient *client, const char *from_date, const char *to_date,
                                   const char *country, int min_importance,
                                   const char *currency, const char *category);

// =====================================================================
// NEWS API
// =====================================================================
AxionResponse* axion_news_general(AxionClient *client);
AxionResponse* axion_news_company(AxionClient *client, const char *ticker);
AxionResponse* axion_news_country(AxionClient *client, const char *country);
AxionResponse* axion_news_category(AxionClient *client, const char *category);

// =====================================================================
// SENTIMENT API
// =====================================================================
AxionResponse* axion_sentiment_all(AxionClient *client, const char *ticker);
AxionResponse* axion_sentiment_social(AxionClient *client, const char *ticker);
AxionResponse* axion_sentiment_news(AxionClient *client, const char *ticker);
AxionResponse* axion_sentiment_analyst(AxionClient *client, const char *ticker);

// =====================================================================
// PROFILES API
// =====================================================================
AxionResponse* axion_profiles_profile(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_recommendation(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_statistics(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_summary(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_calendar(AxionClient *client, const char *ticker);
AxionResponse* axion_profiles_info(AxionClient *client, const char *ticker);

// =====================================================================
// EARNINGS API
// =====================================================================
AxionResponse* axion_earnings_history(AxionClient *client, const char *ticker);
AxionResponse* axion_earnings_trend(AxionClient *client, const char *ticker);
AxionResponse* axion_earnings_index(AxionClient *client, const char *ticker);
AxionResponse* axion_earnings_report(AxionClient *client, const char *ticker, const char *year, const char *quarter);

// =====================================================================
// FILINGS API
// =====================================================================
AxionResponse* axion_filings_filings(AxionClient *client, const char *ticker, int limit, const char *form);
AxionResponse* axion_filings_forms(AxionClient *client, const char *ticker, const char *form_type,
                                   const char *year, const char *quarter, int limit);
AxionResponse* axion_filings_desc_forms(AxionClient *client);
AxionResponse* axion_filings_search(AxionClient *client, const char *year, const char *quarter,
                                    const char *form, const char *ticker);

// =====================================================================
// FINANCIALS API
// =====================================================================
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

// =====================================================================
// INSIDERS API
// =====================================================================
AxionResponse* axion_insiders_funds(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_individuals(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_institutions(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_ownership(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_activity(AxionClient *client, const char *ticker);
AxionResponse* axion_insiders_transactions(AxionClient *client, const char *ticker);

// =====================================================================
// WEB TRAFFIC API
// =====================================================================
AxionResponse* axion_webtraffic_traffic(AxionClient *client, const char *ticker);


#endif // AXION_H
