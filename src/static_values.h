/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef STATIC_VALUES_H_
#define STATIC_VALUES_H_

#define LEDGER_STAGING_SERVER           "https://ledger-staging.mercury.basicattentiontoken.org"
#define LEDGER_PRODUCTION_SERVER        "https://ledger.mercury.basicattentiontoken.org"
#define LEDGER_PRODUCTION_PROXY_SERVER  "https://mercury-proxy.privateinternetaccess.com"
#define BALANCE_STAGING_SERVER          "https://balance-staging.mercury.basicattentiontoken.org"
#define BALANCE_PRODUCTION_SERVER       "https://balance.mercury.basicattentiontoken.org"

#define PREFIX_V2                       "/v2"
#define PREFIX_V3                       "/v3"
#define REGISTER_PERSONA                "/registrar/persona"
#define PUBLISHER_TIMESTAMP             "/publisher/timestamp"
#define PUBLISHER_INFO                  "/publisher/identity?publisher="
#define WALLET_PROPERTIES               "/v2/wallet/"
#define WALLET_PROPERTIES_END           "/balance"
#define RECONCILE_CONTRIBUTION          "/surveyor/contribution/current/"
#define UPDATE_RULES_V1                 "/v1/publisher/ruleset?consequential=true"
#define UPDATE_RULES_V2                 "/v2/publisher/ruleset?limit=512&excludedOnly=false"

#define REGISTRARVK_FIELDNAME           "registrarVK"
#define VERIFICATION_FIELDNAME          "verification"
#define SURVEYOR_ID                     "surveyorId"

#define CURRENCY                        "BAT"

#define SIGNATURE_ALGORITHM             "ed25519"

#define AD_FREE_SETTINGS                "adFree"

#define LEDGER_STATE_FILENAME           "6e16793f-52e1-41fb-b6a2-24b99b47e8f8"
#define LEDGER_PUBLISHER_STATE_FILENAME "ee1e6705-bc4f-4aba-b03c-57cc8cb2ae4d"
#define PUBLISHERS_DB_NAME              "d2c799cd-f37f-4230-9a04-ca23ba5be240"

#define SEED_LENGTH                     32
#define SALT_LENGTH                     64

namespace ledger {

static const bool g_isProduction = false;
static const uint8_t g_hkdfSalt[] = {126, 244, 99, 158, 51, 68, 253, 80, 133, 183, 51, 180, 77,
  62, 74, 252, 62, 106, 96, 125, 241, 110, 134, 87, 190, 208,
  158, 84, 125, 69, 246, 207, 162, 247, 107, 172, 37, 34, 53,
  246, 105, 20, 215, 5, 248, 154, 179, 191, 46, 17, 6, 72, 210,
  91, 10, 169, 145, 248, 22, 147, 117, 24, 105, 12};

static const double _d = 1.0 / (30.0 * 1000.0);
static const unsigned int _default_min_pubslisher_duration = 8000;  // In milliseconds

static const uint64_t _milliseconds_day = 24 * 60 * 60 * 1000;
static const uint64_t _milliseconds_hour = 60 * 60 * 1000;
static const uint64_t _milliseconds_minute = 60 * 1000;
static const uint64_t _milliseconds_second = 1000;

}

#endif  // STATIC_VALUES_H_
