/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat_helper.h"

#include <openssl/hkdf.h>
#include <openssl/digest.h>
#include <openssl/sha.h>
#include <openssl/base64.h>
#include <sstream>
#include <random>
#include <utility>
#include <iomanip>
#include <ctime>

#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "base/sequenced_task_runner.h"
#include "base/task_scheduler/post_task.h"
#include "base/values.h"
#include "base/bind.h"
#include "chrome/browser/browser_process.h"
#include "content/public/browser/browser_thread.h"

#include "static_values.h"
#include "tweetnacl.h"


REQUEST_CREDENTIALS_ST::REQUEST_CREDENTIALS_ST() {}
REQUEST_CREDENTIALS_ST::~REQUEST_CREDENTIALS_ST() {}

WALLET_INFO_ST::WALLET_INFO_ST() {}
WALLET_INFO_ST::~WALLET_INFO_ST() {}

CLIENT_STATE_ST::CLIENT_STATE_ST():
  bootStamp_(0),
  reconcileStamp_(0),
  settings_(AD_FREE_SETTINGS),
  fee_amount_(0),
  days_(0)  {}
CLIENT_STATE_ST::~CLIENT_STATE_ST() {}

PUBLISHER_STATE_ST::PUBLISHER_STATE_ST():
  min_pubslisher_duration_(ledger::_default_min_pubslisher_duration),
  min_visits_(1),
  allow_non_verified_(true) {}
PUBLISHER_STATE_ST::~PUBLISHER_STATE_ST() {}

PUBLISHER_ST::PUBLISHER_ST():
  duration_(0),
  score_(0),
  visits_(0),
  verified_(false),
  exclude_(false),
  pinPercentage_(false),
  verifiedTimeStamp_(0),
  percent_(0),
  deleted_(false) {}
PUBLISHER_ST::PUBLISHER_ST(const PUBLISHER_ST& publisher) :
  duration_(publisher.duration_),
  favicon_url_(publisher.favicon_url_),
  score_(publisher.score_),
  visits_(publisher.visits_),
  verified_(publisher.verified_),
  exclude_(publisher.exclude_),
  pinPercentage_(publisher.pinPercentage_),
  verifiedTimeStamp_(publisher.verifiedTimeStamp_),
  percent_(publisher.percent_),
  deleted_(publisher.deleted_) {}
PUBLISHER_ST::~PUBLISHER_ST() {}

PUBLISHER_DATA_ST::PUBLISHER_DATA_ST() :
  daysSpent_(0),
  hoursSpent_(0),
  minutesSpent_(0),
  secondsSpent_(0) {}
PUBLISHER_DATA_ST::PUBLISHER_DATA_ST(const PUBLISHER_DATA_ST& publisherData) :
  publisherKey_(publisherData.publisherKey_),
  publisher_(publisherData.publisher_),
  daysSpent_(publisherData.daysSpent_),
  hoursSpent_(publisherData.hoursSpent_),
  minutesSpent_(publisherData.minutesSpent_),
  secondsSpent_(publisherData.secondsSpent_) {}
PUBLISHER_DATA_ST::~PUBLISHER_DATA_ST() {}

WALLET_PROPERTIES_ST::WALLET_PROPERTIES_ST() {}
WALLET_PROPERTIES_ST::~WALLET_PROPERTIES_ST() {}

FETCH_CALLBACK_EXTRA_DATA_ST::FETCH_CALLBACK_EXTRA_DATA_ST():
  value1(0),
  boolean1(true) {}
FETCH_CALLBACK_EXTRA_DATA_ST::~FETCH_CALLBACK_EXTRA_DATA_ST() {}



BatHelper::BatHelper() {
}

BatHelper::~BatHelper() {
}

std::string BatHelper::getJSONValue(const std::string& fieldName, const std::string& json) {
  std::string res;

  std::unique_ptr<base::Value> json_object = base::JSONReader::Read(json);
  if (nullptr == json_object.get()) {
      LOG(ERROR) << "BatHelper::getJSONValue: incorrect json object";

      return "";
  }

  const base::DictionaryValue* childTopDictionary = nullptr;
  json_object->GetAsDictionary(&childTopDictionary);
  if (nullptr == childTopDictionary) {
      return "";
  }
  const base::Value* value = nullptr;
  if (childTopDictionary->Get(fieldName, &value)) {
    if (value->GetAsString(&res)) {
      return res;
    }
  }

  return res;
}

void BatHelper::getJSONPublisherState(const std::string& json, PUBLISHER_STATE_ST& state) {
  std::unique_ptr<base::Value> json_object = base::JSONReader::Read(json);
  if (nullptr == json_object.get()) {
      LOG(ERROR) << "BatHelper::getJSONState: incorrect json object";

      return;
  }

  const base::DictionaryValue* childTopDictionary = nullptr;
  json_object->GetAsDictionary(&childTopDictionary);
  if (nullptr == childTopDictionary) {
      return;
  }
  const base::Value* value = nullptr;
  if (childTopDictionary->Get("min_pubslisher_duration", &value)) {
    value->GetAsInteger((int*)&state.min_pubslisher_duration_);
    assert(0 != state.min_pubslisher_duration_);
  }
  if (childTopDictionary->Get("min_visits", &value)) {
    value->GetAsInteger((int*)&state.min_visits_);
    assert(0 != state.min_visits_);
  }
  if (childTopDictionary->Get("allow_non_verified", &value)) {
    value->GetAsBoolean(&state.allow_non_verified_);
  }
}

void BatHelper::getJSONState(const std::string& json, CLIENT_STATE_ST& state) {
  std::unique_ptr<base::Value> json_object = base::JSONReader::Read(json);
  if (nullptr == json_object.get()) {
      LOG(ERROR) << "BatHelper::getJSONState: incorrect json object";

      return;
  }

  const base::DictionaryValue* childTopDictionary = nullptr;
  json_object->GetAsDictionary(&childTopDictionary);
  if (nullptr == childTopDictionary) {
      return;
  }
  const base::Value* value = nullptr;
  if (childTopDictionary->Get("bootStamp", &value)) {
    std::string bootStamp;
    value->GetAsString(&bootStamp);
    std::stringstream temp(bootStamp);
    temp >> state.bootStamp_;
    DCHECK(state.bootStamp_ != 0);
  }
  if (childTopDictionary->Get("reconcileStamp", &value)) {
    std::string reconcileStamp;
    value->GetAsString(&reconcileStamp);
    std::stringstream temp(reconcileStamp);
    temp >> state.reconcileStamp_;
    DCHECK(state.reconcileStamp_ != 0);
  }
  if (childTopDictionary->Get("personaId", &value)) {
    value->GetAsString(&state.personaId_);
    DCHECK(!state.personaId_.empty());
  }
  if (childTopDictionary->Get("userId", &value)) {
    value->GetAsString(&state.userId_);
    DCHECK(!state.userId_.empty());
  }
  if (childTopDictionary->Get("registrarVK", &value)) {
    value->GetAsString(&state.registrarVK_);
    DCHECK(!state.registrarVK_.empty());
  }
  if (childTopDictionary->Get("masterUserToken", &value)) {
    value->GetAsString(&state.masterUserToken_);
    DCHECK(!state.masterUserToken_.empty());
  }
  if (childTopDictionary->Get("fee_currency", &value)) {
    value->GetAsString(&state.fee_currency_);
    DCHECK(!state.fee_currency_.empty());
  }
  if (childTopDictionary->Get("settings", &value)) {
    value->GetAsString(&state.settings_);
    DCHECK(!state.settings_.empty());
  }
  if (childTopDictionary->Get("fee_amount", &value)) {
    value->GetAsDouble(&state.fee_amount_);
    DCHECK(0 != state.fee_amount_);
  }
  if (childTopDictionary->Get("days", &value)) {
    value->GetAsInteger((int*)&state.days_);
    DCHECK(0 != state.days_);
  }
  if (childTopDictionary->Get("wallet_info.paymentId", &value)) {
    value->GetAsString(&state.walletInfo_.paymentId_);
    DCHECK(!state.walletInfo_.paymentId_.empty());
  }
  if (childTopDictionary->Get("wallet_info.addressBAT", &value)) {
    value->GetAsString(&state.walletInfo_.addressBAT_);
    DCHECK(!state.walletInfo_.addressBAT_.empty());
  }
  if (childTopDictionary->Get("wallet_info.addressBTC", &value)) {
    value->GetAsString(&state.walletInfo_.addressBTC_);
    DCHECK(!state.walletInfo_.addressBTC_.empty());
  }
  if (childTopDictionary->Get("wallet_info.addressCARD_ID", &value)) {
    value->GetAsString(&state.walletInfo_.addressCARD_ID_);
    DCHECK(!state.walletInfo_.addressCARD_ID_.empty());
  }
  if (childTopDictionary->Get("wallet_info.addressETH", &value)) {
    value->GetAsString(&state.walletInfo_.addressETH_);
    DCHECK(!state.walletInfo_.addressETH_.empty());
  }
  if (childTopDictionary->Get("wallet_info.addressLTC", &value)) {
    value->GetAsString(&state.walletInfo_.addressLTC_);
    DCHECK(!state.walletInfo_.addressLTC_.empty());
  }
  if (childTopDictionary->Get("wallet_info.keyInfoSeed_", &value)) {
    std::string keyInfoSeed;
    value->GetAsString(&keyInfoSeed);
    DCHECK(!keyInfoSeed.empty());
    state.walletInfo_.keyInfoSeed_ = BatHelper::getFromBase64(keyInfoSeed);
  }
}

void BatHelper::getJSONWalletProperties(const std::string& json, WALLET_PROPERTIES_ST& walletProperties) {
  std::unique_ptr<base::Value> json_object = base::JSONReader::Read(json);
  if (nullptr == json_object.get()) {
      LOG(ERROR) << "BatHelper::getJSONWalletProperties: incorrect json object";

      return;
  }

  const base::DictionaryValue* childTopDictionary = nullptr;
  json_object->GetAsDictionary(&childTopDictionary);
  if (nullptr == childTopDictionary) {
      return;
  }
  const base::Value* value = nullptr;
  if (childTopDictionary->Get("altcurrency", &value)) {
    value->GetAsString(&walletProperties.altcurrency_);
  }
  if (childTopDictionary->Get("balance", &value)) {
    value->GetAsDouble(&walletProperties.balance_);
  }
  if (childTopDictionary->Get("rates.ETH", &value)) {
    double dValue = 0;
    value->GetAsDouble(&dValue);
    walletProperties.rates_.insert(std::pair<std::string, double>("ETH", dValue));
  }
  if (childTopDictionary->Get("rates.LTC", &value)) {
    double dValue = 0;
    value->GetAsDouble(&dValue);
    walletProperties.rates_.insert(std::pair<std::string, double>("LTC", dValue));
  }
  if (childTopDictionary->Get("rates.BTC", &value)) {
    double dValue = 0;
    value->GetAsDouble(&dValue);
    walletProperties.rates_.insert(std::pair<std::string, double>("BTC", dValue));
  }
  if (childTopDictionary->Get("rates.USD", &value)) {
    double dValue = 0;
    value->GetAsDouble(&dValue);
    walletProperties.rates_.insert(std::pair<std::string, double>("USD", dValue));
  }
  if (childTopDictionary->Get("rates.EUR", &value)) {
    double dValue = 0;
    value->GetAsDouble(&dValue);
    walletProperties.rates_.insert(std::pair<std::string, double>("EUR", dValue));
  }
  if (childTopDictionary->Get("parameters.adFree.currency", &value)) {
    value->GetAsString(&walletProperties.parameters_currency_);
  }
  if (childTopDictionary->Get("parameters.adFree.fee.BAT", &value)) {
    value->GetAsDouble(&walletProperties.parameters_fee_);
  }
  if (childTopDictionary->Get("parameters.adFree.choices.BAT", &value)) {
    const base::ListValue *lValue = nullptr;
    value->GetAsList(&lValue);
    for (size_t i = 0; i < lValue->GetSize(); i++) {
      double tempValue = 0;
      lValue->GetDouble(i, &tempValue);
      walletProperties.parameters_choices_.push_back(tempValue);
    }
  }
  if (childTopDictionary->Get("parameters.adFree.range.BAT", &value)) {
    const base::ListValue *lValue = nullptr;
    value->GetAsList(&lValue);
    for (size_t i = 0; i < lValue->GetSize(); i++) {
      double tempValue = 0;
      lValue->GetDouble(i, &tempValue);
      walletProperties.parameters_range_.push_back(tempValue);
    }
  }
  if (childTopDictionary->Get("parameters.adFree.days", &value)) {
    value->GetAsInteger((int*)&walletProperties.parameters_days_);
  }
}

void BatHelper::getJSONPublisher(const std::string& json, PUBLISHER_ST& publisher_st) {
  std::unique_ptr<base::Value> json_object = base::JSONReader::Read(json);
  if (nullptr == json_object.get()) {
      LOG(ERROR) << "BatHelper::getJSONPublisher: incorrect json object";

      return;
  }

  const base::DictionaryValue* childTopDictionary = nullptr;
  json_object->GetAsDictionary(&childTopDictionary);
  if (nullptr == childTopDictionary) {
      return;
  }
  const base::Value* value = nullptr;
  if (childTopDictionary->Get("duration", &value)) {
    std::string duration;
    value->GetAsString(&duration);
    std::stringstream temp(duration);
    temp >> publisher_st.duration_;
  }
  if (childTopDictionary->Get("favicon_url", &value)) {
    value->GetAsString(&publisher_st.favicon_url_);
  }
  if (childTopDictionary->Get("score", &value)) {
    value->GetAsDouble(&publisher_st.score_);
  }
  if (childTopDictionary->Get("visits", &value)) {
    value->GetAsInteger((int*)&publisher_st.visits_);
  }
  if (childTopDictionary->Get("verified", &value)) {
    value->GetAsBoolean(&publisher_st.verified_);
  }
  if (childTopDictionary->Get("exclude", &value)) {
    value->GetAsBoolean(&publisher_st.exclude_);
  }
  if (childTopDictionary->Get("pinPercentage", &value)) {
    value->GetAsBoolean(&publisher_st.pinPercentage_);
  }
  if (childTopDictionary->Get("verifiedTimeStamp", &value)) {
    std::string verifiedTimeStamp;
    value->GetAsString(&verifiedTimeStamp);
    std::stringstream temp(verifiedTimeStamp);
    temp >> publisher_st.verifiedTimeStamp_;
  }
  if (childTopDictionary->Get("percent", &value)) {
    value->GetAsInteger((int*)&publisher_st.percent_);
  }
  if (childTopDictionary->Get("deleted", &value)) {
    value->GetAsBoolean(&publisher_st.deleted_);
  }
}

void BatHelper::getJSONPublisherVerified(const std::string& json, bool& verified) {
  verified = false;
  std::unique_ptr<base::Value> json_object = base::JSONReader::Read(json);
  if (nullptr == json_object.get()) {
      LOG(ERROR) << "BatHelper::getJSONPublisherVerified: incorrect json object";

      return;
  }

  const base::DictionaryValue* childTopDictionary = nullptr;
  json_object->GetAsDictionary(&childTopDictionary);
  if (nullptr == childTopDictionary) {
      return;
  }
  const base::Value* value = nullptr;
  if (childTopDictionary->Get("properties.verified", &value)) {
    value->GetAsBoolean(&verified);
  }
}

void BatHelper::getJSONWalletInfo(const std::string& json, WALLET_INFO_ST& walletInfo,
      std::string& fee_currency, double& fee_amount, unsigned int& days) {
  std::unique_ptr<base::Value> json_object = base::JSONReader::Read(json);
  if (nullptr == json_object.get()) {
      LOG(ERROR) << "BatHelper::getJSONWalletInfo: incorrect json object";

      return;
  }

  const base::DictionaryValue* childTopDictionary = nullptr;
  json_object->GetAsDictionary(&childTopDictionary);
  if (nullptr == childTopDictionary) {
      return;
  }
  const base::Value* value = nullptr;
  if (childTopDictionary->Get("wallet.paymentId", &value)) {
    value->GetAsString(&walletInfo.paymentId_);
    DCHECK(!walletInfo.paymentId_.empty());
  }
  if (childTopDictionary->Get("wallet.addresses.BAT", &value)) {
    value->GetAsString(&walletInfo.addressBAT_);
    DCHECK(!walletInfo.addressBAT_.empty());
  }
  if (childTopDictionary->Get("wallet.addresses.BTC", &value)) {
    value->GetAsString(&walletInfo.addressBTC_);
    DCHECK(!walletInfo.addressBTC_.empty());
  }
  if (childTopDictionary->Get("wallet.addresses.CARD_ID", &value)) {
    value->GetAsString(&walletInfo.addressCARD_ID_);
    DCHECK(!walletInfo.addressCARD_ID_.empty());
  }
  if (childTopDictionary->Get("wallet.addresses.ETH", &value)) {
    value->GetAsString(&walletInfo.addressETH_);
    DCHECK(!walletInfo.addressETH_.empty());
  }
  if (childTopDictionary->Get("wallet.addresses.LTC", &value)) {
    value->GetAsString(&walletInfo.addressLTC_);
    DCHECK(!walletInfo.addressLTC_.empty());
  }

  if (childTopDictionary->Get("payload.adFree.days", &value)) {
    value->GetAsInteger((int*)&days);
    DCHECK(days != 0);
  }
  const base::DictionaryValue* feeDictionary = nullptr;
  if (childTopDictionary->Get("payload.adFree.fee", &value)) {
    if (!value->GetAsDictionary(&feeDictionary)) {
      LOG(ERROR) << "BatHelper::getJSONWalletInfo: could not get fee object";

      return;
    }
    base::detail::const_dict_iterator_proxy dictIterator = feeDictionary->DictItems();
    if (dictIterator.begin() != dictIterator.end()) {
      fee_currency = dictIterator.begin()->first;
      dictIterator.begin()->second.GetAsDouble(&fee_amount);
    }
  }
}

void BatHelper::getJSONPublisherTimeStamp(const std::string& json, uint64_t& publisherTimestamp) {
  publisherTimestamp = 0;
  std::unique_ptr<base::Value> json_object = base::JSONReader::Read(json);
  if (nullptr == json_object.get()) {
      LOG(ERROR) << "BatHelper::getJSONPublisherTimeStamp: incorrect json object";

      return;
  }

  const base::DictionaryValue* childTopDictionary = nullptr;
  json_object->GetAsDictionary(&childTopDictionary);
  if (nullptr == childTopDictionary) {
      return;
  }
  const base::Value* value = nullptr;
  if (childTopDictionary->Get("timestamp", &value)) {
    std::string timestamp;
    value->GetAsString(&timestamp);
    std::stringstream temp(timestamp);
    temp >> publisherTimestamp;
  }
}

std::vector<uint8_t> BatHelper::generateSeed() {
  //std::ostringstream seedStr;

  std::vector<uint8_t> vSeed(SEED_LENGTH);
  std::random_device r;
  std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
  auto rand = std::bind(std::uniform_int_distribution<>(0, UCHAR_MAX),
                          std::mt19937(seed));

  //std::generate_n(std::ostream_iterator<int>(seedStr, ","), seedLength, rand);
  std::generate_n(vSeed.begin(), SEED_LENGTH, rand);
  /*for (size_t i = 0; i < vSeed.size(); i++) {
    if (0 != i) {
      seedStr << ",";
    }
    seedStr << vSeed[i];
  }
  std::string res = seedStr.str();
  //if (!res.empty()) {
  //  res.erase(res.end() - 1);
  //}
  LOG(ERROR) << res;

  return res;*/
  return vSeed;
}

std::vector<uint8_t> BatHelper::getHKDF(const std::vector<uint8_t>& seed) {
  DCHECK(!seed.empty());
  std::vector<uint8_t> out(SEED_LENGTH);
  //uint8_t out[SEED_LENGTH];
  //to do debug
  /*std::ostringstream seedStr1;
  for (size_t i = 0; i < SEED_LENGTH; i++) {
    if (0 != i) {
      seedStr1 << ",";
    }
    seedStr1 << (int)seed[i];
  }
  LOG(ERROR) << "!!!seed == " << seedStr1.str();*/
  //
  int hkdfRes = HKDF(&out.front(), SEED_LENGTH, EVP_sha512(), &seed.front(), seed.size(),
    ledger::g_hkdfSalt, SALT_LENGTH, nullptr, 0);

  DCHECK(hkdfRes);
  DCHECK(!seed.empty());

  //to do debug
  /*std::ostringstream seedStr;
  for (size_t i = 0; i < SEED_LENGTH; i++) {
    if (0 != i) {
      seedStr << ",";
    }
    seedStr << (int)out[i];
  }
  LOG(ERROR) << "!!!hkdfRes == " << hkdfRes << ", out == " << seedStr.str();*/
  //

  return out;
}

void BatHelper::getPublicKeyFromSeed(const std::vector<uint8_t>& seed,
      std::vector<uint8_t>& publicKey, std::vector<uint8_t>& secretKey) {
  DCHECK(!seed.empty());
  publicKey.resize(crypto_sign_PUBLICKEYBYTES);
  secretKey = seed;
  secretKey.resize(crypto_sign_SECRETKEYBYTES);

  crypto_sign_keypair(&publicKey.front(), &secretKey.front(), 1);

  DCHECK(!publicKey.empty() && !secretKey.empty());
  //to do debug
  /*std::ostringstream publicStr;
  for (size_t i = 0; i < crypto_sign_PUBLICKEYBYTES; i++) {
    if (0 != i) {
      publicStr << ",";
    }
    publicStr << (int)outPublic[i];
  }
  std::ostringstream secretStr;
  for (size_t i = 0; i < crypto_sign_SECRETKEYBYTES; i++) {
    if (0 != i) {
      secretStr << ",";
    }
    secretStr << (int)outSecret[i];
  }
  LOG(ERROR) << "!!!publicStr == " << publicStr.str();
  LOG(ERROR) << "!!!secretStr == " << secretStr.str();*/
}

std::string BatHelper::uint8ToHex(const std::vector<uint8_t>& in) {
  std::ostringstream res;
  for (size_t i = 0; i < in.size(); i++) {
    res << std::setfill('0') << std::setw(sizeof(uint8_t) * 2)
       << std::hex << (int)in[i];
  }

  return res.str();
}

std::string BatHelper::stringify(std::string* keys,
    std::string* values, const unsigned int& size) {
  std::string res;

  base::DictionaryValue root_dict;
  for (unsigned int i = 0; i < size; i++) {
    root_dict.SetString(keys[i], values[i]);
  }

  base::JSONWriter::Write(root_dict, &res);

  return res;
}

std::string BatHelper::stringifyRequestCredentialsSt(const REQUEST_CREDENTIALS_ST& request_credentials) {
  std::string res;

  base::DictionaryValue root_dict;
  root_dict.SetString("requestType", request_credentials.requestType_);
  std::unique_ptr<base::DictionaryValue> request_dict(new base::DictionaryValue());
  std::unique_ptr<base::DictionaryValue> request_headers_dict(new base::DictionaryValue());
  request_headers_dict->SetString("digest", request_credentials.request_headers_digest_);
  request_headers_dict->SetString("signature", request_credentials.request_headers_signature_);
  request_dict->Set("headers", std::move(request_headers_dict));
  std::unique_ptr<base::DictionaryValue> request_body_dict(new base::DictionaryValue());
  request_body_dict->SetString("currency", request_credentials.request_body_currency_);
  request_body_dict->SetString("label", request_credentials.request_body_label_);
  request_body_dict->SetString("publicKey", request_credentials.request_body_publicKey_);
  request_dict->Set("body", std::move(request_body_dict));
  request_dict->SetString("octets", request_credentials.request_body_octets_);
  root_dict.Set("request", std::move(request_dict));
  root_dict.SetString("proof", request_credentials.proof_);

  base::JSONWriter::Write(root_dict, &res);

  return res;
}

std::string BatHelper::stringifyState(const CLIENT_STATE_ST& state) {
  std::string res;

  base::DictionaryValue root_dict;
  root_dict.SetString("bootStamp", std::to_string(state.bootStamp_));
  root_dict.SetString("reconcileStamp", std::to_string(state.reconcileStamp_));
  root_dict.SetString("personaId", state.personaId_);
  root_dict.SetString("userId", state.userId_);
  root_dict.SetString("registrarVK", state.registrarVK_);
  root_dict.SetString("masterUserToken", state.masterUserToken_);
  root_dict.SetString("fee_currency", state.fee_currency_);
  root_dict.SetString("settings", state.settings_);
  root_dict.SetDouble("fee_amount", state.fee_amount_);
  root_dict.SetInteger("days", state.days_);
  std::unique_ptr<base::DictionaryValue> wallet_info_dict(new base::DictionaryValue());
  wallet_info_dict->SetString("paymentId", state.walletInfo_.paymentId_);
  wallet_info_dict->SetString("addressBAT", state.walletInfo_.addressBAT_);
  wallet_info_dict->SetString("addressBTC", state.walletInfo_.addressBTC_);
  wallet_info_dict->SetString("addressCARD_ID", state.walletInfo_.addressCARD_ID_);
  wallet_info_dict->SetString("addressETH", state.walletInfo_.addressETH_);
  wallet_info_dict->SetString("addressLTC", state.walletInfo_.addressLTC_);
  wallet_info_dict->SetString("keyInfoSeed_", BatHelper::getBase64(state.walletInfo_.keyInfoSeed_));
  root_dict.Set("wallet_info", std::move(wallet_info_dict));

  base::JSONWriter::Write(root_dict, &res);

  return res;
}

std::string BatHelper::stringifyPublisherState(const PUBLISHER_STATE_ST& state) {
  std::string res;

  base::DictionaryValue root_dict;
  root_dict.SetInteger("min_pubslisher_duration", state.min_pubslisher_duration_);
  root_dict.SetInteger("min_visits", state.min_visits_);
  root_dict.SetBoolean("allow_non_verified", state.allow_non_verified_);

  base::JSONWriter::Write(root_dict, &res);

  return res;
}

std::string BatHelper::stringifyPublisher(PUBLISHER_ST& publisher_st) {
  std::string res;

  base::DictionaryValue root_dict;
  root_dict.SetString("duration", std::to_string(publisher_st.duration_));
  root_dict.SetString("favicon_url", publisher_st.favicon_url_);
  root_dict.SetDouble("score", publisher_st.score_);
  root_dict.SetInteger("visits", publisher_st.visits_);
  root_dict.SetBoolean("verified", publisher_st.verified_);
  root_dict.SetBoolean("exclude", publisher_st.exclude_);
  root_dict.SetBoolean("pinPercentage", publisher_st.pinPercentage_);
  root_dict.SetString("verifiedTimeStamp", std::to_string(publisher_st.verifiedTimeStamp_));
  root_dict.SetInteger("percent", publisher_st.percent_);
  root_dict.SetBoolean("deleted", publisher_st.deleted_);

  base::JSONWriter::Write(root_dict, &res);
  LOG(ERROR) << "!!!stringifyPublisher res == " << res;

  return res;
}

std::vector<uint8_t> BatHelper::getSHA256(const std::string& in) {
  std::vector<uint8_t> res(SHA256_DIGEST_LENGTH);

  SHA256((uint8_t*)in.c_str(), in.length(), &res.front());

  return res;
}

std::string BatHelper::getBase64(const std::vector<uint8_t>& in) {
  std::string res;

  size_t size = 0;
  if (!EVP_EncodedLength(&size, in.size())) {
    DCHECK(false);
    LOG(ERROR) << "EVP_EncodedLength failure in BatHelper::getBase64";

    return "";
  }
  std::vector<uint8_t> out(size);
  DCHECK(EVP_EncodeBlock(&out.front(), &in.front(), in.size()) != 0);
  res = (char*)&out.front();

  return res;
}

std::vector<uint8_t> BatHelper::getFromBase64(const std::string& in) {
  std::vector<uint8_t> res;

  size_t size = 0;
  if (!EVP_DecodedLength(&size, in.length())) {
    DCHECK(false);
    LOG(ERROR) << "EVP_DecodedLength failure in BatHelper::getFromBase64";

    return res;
  }
  res.resize(size);
  DCHECK(EVP_DecodeBase64(&res.front(), &size, size, (const uint8_t*)in.c_str(), in.length()));
  LOG(ERROR) << "!!!decoded size == " << size;

  return res;
}

std::string BatHelper::sign(std::string* keys, std::string* values, const unsigned int& size,
    const std::string& keyId, const std::vector<uint8_t>& secretKey) {
  std::string headers;
  std::string message;
  for (unsigned int i = 0; i < size; i++) {
    if (0 != i) {
      headers += " ";
      message += "\n";
    }
    headers += keys[i];
    message += keys[i] + ": " + values[i];
  }
  std::vector<uint8_t> signedMsg(crypto_sign_BYTES + message.length());
  uint64_t signedMsgSize = 0;
  crypto_sign(&signedMsg.front(), reinterpret_cast<unsigned long long*>(&signedMsgSize), (const unsigned char*)message.c_str(),
    (uint64_t)message.length(), &secretKey.front());
  std::vector<uint8_t> signature(crypto_sign_BYTES);
  std::copy(signedMsg.begin(), signedMsg.begin() + crypto_sign_BYTES, signature.begin());

  return "keyId=\"" + keyId + "\",algorithm=\"" + SIGNATURE_ALGORITHM +
    "\",headers=\"" + headers + "\",signature=\"" + BatHelper::getBase64(signature) + "\"";
}

uint64_t BatHelper::currentTime() {
  return time(0);
}

void BatHelper::writeStateFile(const std::string& data) {
  base::FilePath dirToSave;
  base::PathService::Get(base::DIR_HOME, &dirToSave);
  dirToSave = dirToSave.Append(LEDGER_STATE_FILENAME);

  assert(base::WriteFile(dirToSave, data.c_str(), data.length()));
}

void BatHelper::readStateFile(BatHelper::ReadStateCallback callback) {
  base::FilePath dirToSave;
  base::PathService::Get(base::DIR_HOME, &dirToSave);
  dirToSave = dirToSave.Append(LEDGER_STATE_FILENAME);
  int64_t file_size = 0;
  if (!GetFileSize(dirToSave, &file_size)) {

    callback.Run(false, CLIENT_STATE_ST());

    return;
  }
  std::vector<char> data(file_size + 1);
  if (-1 != base::ReadFile(dirToSave, &data.front(), file_size)) {
    data[file_size] = '\0';
    CLIENT_STATE_ST state;
    BatHelper::getJSONState(&data.front(), state);
    callback.Run(true, state);

    return;
  }

  callback.Run(false, CLIENT_STATE_ST());
}

void BatHelper::writePublisherStateFile(const std::string& data) {
  base::FilePath dirToSave;
  base::PathService::Get(base::DIR_HOME, &dirToSave);
  dirToSave = dirToSave.Append(LEDGER_PUBLISHER_STATE_FILENAME);

  assert(base::WriteFile(dirToSave, data.c_str(), data.length()));
}

void BatHelper::readPublisherStateFile(BatHelper::ReadPublisherStateCallback callback) {
  base::FilePath dirToSave;
  base::PathService::Get(base::DIR_HOME, &dirToSave);
  dirToSave = dirToSave.Append(LEDGER_PUBLISHER_STATE_FILENAME);
  int64_t file_size = 0;
  if (!GetFileSize(dirToSave, &file_size)) {

    callback.Run(false, PUBLISHER_STATE_ST());

    return;
  }
  std::vector<char> data(file_size + 1);
  if (-1 != base::ReadFile(dirToSave, &data.front(), file_size)) {
    data[file_size] = '\0';
    PUBLISHER_STATE_ST state;
    BatHelper::getJSONPublisherState(&data.front(), state);
    callback.Run(true, state);

    return;
  }

  callback.Run(false, PUBLISHER_STATE_ST());
}

void BatHelper::saveState(const CLIENT_STATE_ST& state) {
  std::string data = BatHelper::stringifyState(state);
  scoped_refptr<base::SequencedTaskRunner> task_runner =
     base::CreateSequencedTaskRunnerWithTraits(
         {base::MayBlock(), base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN});
  task_runner->PostTask(FROM_HERE, base::Bind(&BatHelper::writeStateFile,
     data));
}

void BatHelper::loadState(BatHelper::ReadStateCallback callback) {
  scoped_refptr<base::SequencedTaskRunner> task_runner =
     base::CreateSequencedTaskRunnerWithTraits(
         {base::MayBlock(), base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN});
  task_runner->PostTask(FROM_HERE, base::Bind(&BatHelper::readStateFile, callback));
}

void BatHelper::savePublisherState(const PUBLISHER_STATE_ST& state) {
  std::string data = BatHelper::stringifyPublisherState(state);
  scoped_refptr<base::SequencedTaskRunner> task_runner =
     base::CreateSequencedTaskRunnerWithTraits(
         {base::MayBlock(), base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN});
  task_runner->PostTask(FROM_HERE, base::Bind(&BatHelper::writePublisherStateFile,
     data));
}

void BatHelper::loadPublisherState(BatHelper::ReadPublisherStateCallback callback) {
  scoped_refptr<base::SequencedTaskRunner> task_runner =
     base::CreateSequencedTaskRunnerWithTraits(
         {base::MayBlock(), base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN});
  task_runner->PostTask(FROM_HERE, base::Bind(&BatHelper::readPublisherStateFile, callback));
}
