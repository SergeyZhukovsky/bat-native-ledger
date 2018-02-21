/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ledger.h"

#include "base/bind.h"
#include "base/logging.h"

#include "bat_client.h"
#include "bat_publisher.h"


using namespace bat_client;
using namespace bat_publisher;

namespace ledger {

  Ledger::Ledger():
    bat_client_(nullptr),
    bat_publisher_(nullptr) {
  }

  Ledger::~Ledger() {
    if (bat_client_) {
      delete bat_client_;
    }
    if (bat_publisher_) {
      delete bat_publisher_;
    }
  }

  void Ledger::createWallet() {
    initSynopsis();
    if (!bat_client_) {
      bat_client_ = new BatClient();
    }
    bat_client_->loadStateOrRegisterPersona();
  }

  void Ledger::initSynopsis() {
    if (!bat_publisher_) {
      bat_publisher_ = new BatPublisher();
    }
    bat_publisher_->initSynopsis();
  }

  bool Ledger::isBatClientExist() {
    if (!bat_client_) {
      LOG(ERROR) << "ledger bat_client is not exist";

      return false;
    }

    return true;
  }

  bool Ledger::isBatPublisherExist() {
    if (!bat_publisher_) {
      LOG(ERROR) << "ledger bat_publisher is not exist";

      return false;
    }

    return true;
  }

  void Ledger::getBalance() {
    if (!isBatClientExist()) {
      assert(false);

      return;
    }
    FETCH_CALLBACK_EXTRA_DATA_ST extraData;
    bat_client_->getWalletProperties(base::Bind(&Ledger::walletPropertiesCallback,
      base::Unretained(this)), extraData);
  }

  void Ledger::walletPropertiesCallback(bool result, const std::string& response, const FETCH_CALLBACK_EXTRA_DATA_ST& extraData) {
    if (!result) {
      // TODO errors handling
      return;
    }
    WALLET_PROPERTIES_ST walletProperties;
    BatHelper::getJSONWalletProperties(response, walletProperties);
    // TODO send the balance to the UI via observer or callback
  }

  void Ledger::saveVisit(const std::string& publisher, const uint64_t& duration) {
    if (!isBatPublisherExist()) {
      assert(false);

      return;
    }
    bat_publisher_->saveVisit(publisher, duration, base::Bind(&Ledger::saveVisitCallback,
      base::Unretained(this)));
  }

  void Ledger::saveVisitCallback(const std::string& publisher, const uint64_t& verifiedTimestamp) {
    if (!isBatClientExist()) {
      assert(false);

      return;
    }
    //to do debug
    LOG(ERROR) << "!!!getting Balance";
    getBalance();
    //
    uint64_t publisherTimestamp = bat_client_->getPublisherTimestamp();
    if (publisherTimestamp <= verifiedTimestamp) {
      return;
    }
    FETCH_CALLBACK_EXTRA_DATA_ST extraData;
    extraData.value1 = publisherTimestamp;
    extraData.string1 = publisher;
    // Update publisher verified or not flag
    bat_client_->publisherInfo(publisher, base::Bind(&Ledger::publisherInfoCallback,
      base::Unretained(this)), extraData);
  }

  void Ledger::publisherInfoCallback(bool result, const std::string& response,
      const FETCH_CALLBACK_EXTRA_DATA_ST& extraData) {
    if (!result) {
      // TODO errors handling
      return;
    }
    bool verified = false;
    BatHelper::getJSONPublisherVerified(response, verified);
    if (!isBatPublisherExist()) {
      assert(false);

      return;
    }
    bat_publisher_->setPublisherTimestampVerified(extraData.string1, extraData.value1, verified);
  }

  void Ledger::favIconUpdated(const std::string& publisher, const std::string& favicon_url) {
    if (!isBatPublisherExist()) {
      assert(false);

      return;
    }
    bat_publisher_->setPublisherFavIcon(publisher, favicon_url);
  }

  void Ledger::setPublisherInclude(const std::string& publisher, const bool& include) {
    if (!isBatPublisherExist()) {
      assert(false);

      return;
    }
    bat_publisher_->setPublisherInclude(publisher, include);
  }

  void Ledger::setPublisherDeleted(const std::string& publisher, const bool& deleted) {
    if (!isBatPublisherExist()) {
      assert(false);

      return;
    }
    bat_publisher_->setPublisherDeleted(publisher, deleted);
  }

  void Ledger::setPublisherPinPercentage(const std::string& publisher, const bool& pinPercentage) {
    if (!isBatPublisherExist()) {
      assert(false);

      return;
    }
    bat_publisher_->setPublisherPinPercentage(publisher, pinPercentage);
  }

  void Ledger::setPublisherMinVisitTime(const uint64_t& duration) { // In milliseconds
    if (!isBatPublisherExist()) {
      assert(false);

      return;
    }
    bat_publisher_->setPublisherMinVisitTime(duration);
  }

  void Ledger::setPublisherMinVisits(const unsigned int& visits) {
    if (!isBatPublisherExist()) {
      assert(false);

      return;
    }
    bat_publisher_->setPublisherMinVisits(visits);
  }

  void Ledger::setPublisherAllowNonVerified(const bool& allow) {
    if (!isBatPublisherExist()) {
      assert(false);

      return;
    }
    bat_publisher_->setPublisherAllowNonVerified(allow);
  }

  void Ledger::setContributionAmount(const double& amount) {
    if (!isBatClientExist()) {
      assert(false);

      return;
    }
    bat_client_->setContributionAmount(amount);
  }

  std::string Ledger::getBATAddress() {
    if (!isBatClientExist()) {
      assert(false);

      return "";
    }
    return bat_client_->getBATAddress();
  }

  std::string Ledger::getBTCAddress() {
    if (!isBatClientExist()) {
      assert(false);

      return "";
    }
    return bat_client_->getBTCAddress();
  }

  std::string Ledger::getETHAddress() {
    if (!isBatClientExist()) {
      assert(false);

      return "";
    }
    return bat_client_->getETHAddress();
  }

  std::string Ledger::getLTCAddress() {
    if (!isBatClientExist()) {
      assert(false);

      return "";
    }
    return bat_client_->getLTCAddress();
  }

}
